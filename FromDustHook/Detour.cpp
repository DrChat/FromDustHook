#include "Detour.h"

#include <assert.h>

#ifdef _WIN32
#	include <Windows.h>
#	define HEAP_CREATE_ENABLE_EXECUTE 0x00040000 // Missing from windows.h
#endif

// This code was stolen from Decoda's source.
// https://github.com/unknownworlds/decoda/blob/master/src/LuaInject/Hook.cpp

// conversion from int to smaller int, possible loss of data
// Okay.
#pragma warning(disable: 4244)

static const char	jumpInstruction = '\xE9';
static const int	jumpInstructionSize = 1 + sizeof(long);
static const char	pushInstruction = '\x68';
static const int	pushInstructionSize = 1 + sizeof(long);
static const char	breakpointInstruction = '\xCC';
static const char	nopInstruction = '\x90';

#ifdef _WIN32
// Purpose: The original bytes of the function are copied here plus a jump so the user can call the original function.
static HANDLE		g_TrampolineHeap = NULL;
#endif

/**
* Returns the base address of a library
*/
void *ResolveLibraryBase(const char *modName) {
#ifdef _WIN32
	HMODULE hModule = GetModuleHandleA(modName);
	if (!hModule)
		return NULL;

	return (void *)hModule;
#else
	// Unimplemented :(
	return NULL;
#endif
}

/**
* Returns the address + the library base (for dynamic base modules)
*/
void *ResolveLibraryAddress(const char *pModName, void *pAddr) {
	void *pBase = ResolveLibraryBase(pModName);

	return (void *)((int)pBase + (int)pAddr);
}

/**
* Returns the size of the instruction at the specified point.
*/
int GetInstructionSize(void *address, unsigned char *opcodeOut = NULL, int *operandSizeOut = NULL) {
	// Modified from http://www.devmaster.net/forums/showthread.php?p=47381

	unsigned char *func = static_cast<unsigned char*>(address);

	if (opcodeOut != NULL) {
		*opcodeOut = 0;
	}

	if (operandSizeOut != NULL) {
		*operandSizeOut = 0;
	}

	if (*func != 0xCC) {
		// Skip prefixes F0h, F2h, F3h, 66h, 67h, D8h-DFh, 2Eh, 36h, 3Eh, 26h, 64h and 65h
		int operandSize = 4;
		int FPU = 0;
		while (*func == 0xF0 ||
			   *func == 0xF2 ||
			   *func == 0xF3 ||
			  (*func & 0xFC) == 0x64 ||
			  (*func & 0xF8) == 0xD8 ||
			  (*func & 0x7E) == 0x62) {
			if (*func == 0x66) {
				operandSize = 2;
			} else if ((*func & 0xF8) == 0xD8) {
				FPU = *func++;
				break;
			}

			func++;
		}

		// Skip two-byte opcode byte
		bool twoByte = false;
		if(*func == 0x0F) {
			twoByte = true;
			func++;
		}

		// Skip opcode byte
		unsigned char opcode = *func++;

		// Skip mod R/M byte
		unsigned char modRM = 0xFF;
		if (FPU) {
			if ((opcode & 0xC0) != 0xC0) {
				modRM = opcode;
			}
		} else if (!twoByte) {
			if ((opcode & 0xC4) == 0x00 ||
					(opcode & 0xF4) == 0x60 && ((opcode & 0x0A) == 0x02 || (opcode & 0x09) == 0x9) ||
					(opcode & 0xF0) == 0x80 ||
					(opcode & 0xF8) == 0xC0 && (opcode & 0x0E) != 0x02 ||
					(opcode & 0xFC) == 0xD0 ||
					(opcode & 0xF6) == 0xF6) {
				modRM = *func++;
			}
		} else {
			if ((opcode & 0xF0) == 0x00 && (opcode & 0x0F) >= 0x04 && (opcode & 0x0D) != 0x0D ||
					(opcode & 0xF0) == 0x30 ||
					opcode == 0x77 ||
					(opcode & 0xF0) == 0x80 ||
					(opcode & 0xF0) == 0xA0 && (opcode & 0x07) <= 0x02 ||
					(opcode & 0xF8) == 0xC8) {
				// No mod R/M byte
			} else {
				modRM = *func++;
			}
		}

		// Skip SIB
		if ((modRM & 0x07) == 0x04 &&
				(modRM & 0xC0) != 0xC0) {
			func += 1; // SIB
		}

		// Skip displacement
		if ((modRM & 0xC5) == 0x05) func += 4; // Dword displacement, no base
		if ((modRM & 0xC0) == 0x40) func += 1; // Byte displacement
		if ((modRM & 0xC0) == 0x80) func += 4; // Dword displacement

		// Skip immediate
		if (FPU) {
			// Can't have immediate operand
		} else if (!twoByte) {
			if ((opcode & 0xC7) == 0x04 ||
				(opcode & 0xFE) == 0x6A || // PUSH/POP/IMUL
				(opcode & 0xF0) == 0x70 || // Jcc
				opcode == 0x80 ||
				opcode == 0x83 ||
				(opcode & 0xFD) == 0xA0 || // MOV
				opcode == 0xA8 || // TEST
				(opcode & 0xF8) == 0xB0 || // MOV
				(opcode & 0xFE) == 0xC0 || // RCL
				opcode == 0xC6 || // MOV
				opcode == 0xCD || // INT
				(opcode & 0xFE) == 0xD4 || // AAD/AAM
				(opcode & 0xF8) == 0xE0 || // LOOP/JCXZ
				opcode == 0xEB ||
				opcode == 0xF6 && (modRM & 0x30) == 0x00) // TEST
			{
				func += 1;
			} else if ((opcode & 0xF7) == 0xC2) {
				func += 2; // RET
			} else if ((opcode & 0xFC) == 0x80 ||
						(opcode & 0xC7) == 0x05 ||
						(opcode & 0xF8) == 0xB8 ||
						(opcode & 0xFE) == 0xE8 || // CALL/Jcc
						(opcode & 0xFE) == 0x68 ||
						(opcode & 0xFC) == 0xA0 ||
						(opcode & 0xEE) == 0xA8 ||
						opcode == 0xC7 ||
						opcode == 0xF7 && (modRM & 0x30) == 0x00) 
			{
				func += operandSize;
			}
		} else {
			if (opcode == 0xBA || // BT
				opcode == 0x0F || // 3DNow!
				(opcode & 0xFC) == 0x70 || // PSLLW
				(opcode & 0xF7) == 0xA4 || // SHLD
				opcode == 0xC2 ||
				opcode == 0xC4 ||
				opcode == 0xC5 ||
				opcode == 0xC6)
			{
				func += 1;
			} else if ((opcode & 0xF0) == 0x80) {
				func += operandSize; // Jcc -i
			}
		}

		if (opcodeOut != NULL) {
			*opcodeOut = opcode;
		}

		if (operandSizeOut != NULL) {
			*operandSizeOut = operandSize;
		}

	}

	return func - static_cast<unsigned char*>(address);
}

/**
* Returns the number of bytes until the next break in instructions.
*/
int GetInstructionBoundary(void *function, int count) {
	int boundary = 0;

	while (boundary < count) {
		unsigned char *func = static_cast<unsigned char *>(function) + boundary;
		int sz = GetInstructionSize(func);
		if (sz == 0)
			return -1;

		boundary += sz;
	}

	return boundary;
}

/**
* Writes a push instruction
*/
void *WritePush(void *base, void *adr) {
	char *push = (char *)base;

	push[0] = pushInstruction;
	*((unsigned long *)(&push[1])) = (unsigned long)adr;

	return (void *)((int)base + pushInstructionSize);
}

/**
* This function will assume the memory given is writable, and that we can write 5 bytes.
*/
void WriteJump(void *base, void *jumpadr) {
	char *jmp = (char *)base;

	jmp[0] = jumpInstruction;
	// Jump is relative to our function.
	*((unsigned long *)(&jmp[1])) = (unsigned long)(jumpadr) - (unsigned long)(base) - 5;
}

/**
* Returns NULL if the address we were given isn't a jump instruction, otherwise we return the exact address of the jump instruction.
*/
void *ReadJump(void *src) {
	void *address = NULL;

	const char *jump = (const char *)src;
	if (jump[0] == jumpInstruction) {
		address = (void *)(*((unsigned long *)&jump[1]) + (unsigned long)jump + 5); // AKA address offset + base
	}

	return address;
}

bool IsDetoured(void *fn, void *hook) {
	void *adr = ReadJump(fn);
	if (adr == hook) {
		return true;
	}

	return false;
}

/**
* Adds the specified offset to all relative jmp instructions in the range of
* the function specified. This is useful when a piece of code is moved in
* memory.
*/
void AdjustRelativeJumps(void *function, int length, int offset) {
	unsigned char *p = reinterpret_cast<unsigned char*>(function);

	int i = 0;

	while (i < length) {
		unsigned char opcode = 0;
		int operandSize = 0;

		int n = GetInstructionSize(p + i, &opcode, &operandSize);

		// Relative jump/call instruction.
		if (opcode == 0xE9 || opcode == 0xE8) {
			if (operandSize == 4) {
				unsigned long address = *((unsigned long *)(p + i + n - operandSize));
				*((unsigned long *)(p + i + n - operandSize)) = address + offset;
			} else if (operandSize == 2) {
				unsigned short address = *((unsigned short *)(p + i + n - operandSize));
				*((unsigned short *)(p + i + n - operandSize)) = address + offset;
			} else {
				assert(0);
			}
		}

		i += n;
	}
}

// This'll return a pointer that you can call for the original function.
// TODO: Kewl idea - Push another argument onto the stack that contains
// the original function pointer. This will be the first argument
// the function sees
void *DetourFunction(void *pFunc, void *pDetourFunc) {
	if (!pFunc || !pDetourFunc) return NULL;

	// Don't allow the same detour twice.
	if (IsDetoured(pFunc, pDetourFunc)) {
		return NULL;
	}

#ifdef _WIN32
	if (g_TrampolineHeap == NULL) {
		// One-time init of the trampoline heap, which is allowed to expand indefinitely.
		g_TrampolineHeap = HeapCreate(HEAP_CREATE_ENABLE_EXECUTE, 1024, 0);
	}

	// Get the instruction boundary so we don't end up overwriting half of an instruction
	int boundary = GetInstructionBoundary(pFunc, jumpInstructionSize);
	if (boundary == -1) // -1 means not enough bytes in the function before 0xCC padding instructions
		return NULL;

	char *trampoline = (char *)HeapAlloc(g_TrampolineHeap, 0, boundary + jumpInstructionSize);

	if (!trampoline) {
		// This is bad. Out of memory.
		assert(0);
		return NULL;
	}

	// Copy the original bytes to the trampoline and append a jump to the original function (after our detour jump).
	memcpy(trampoline, pFunc, boundary);
	AdjustRelativeJumps(trampoline, boundary, ((char *)pFunc) - trampoline); // Adjust any relative jump/call opcodes we may have copied.

	// Write the jump to the original function.
	WriteJump(trampoline + boundary, ((unsigned char *)pFunc) + boundary);

	DWORD dwProtection;

	// Get write access to the function.
	if (VirtualProtect(pFunc, boundary, PAGE_EXECUTE_READWRITE, &dwProtection)) {
		// Write nops incase something jumps to the spot right after the detour jump.
		memset(pFunc, nopInstruction, boundary);

		// Now the jump
		WriteJump(pFunc, pDetourFunc);

		// Restore the old protection
		VirtualProtect(pFunc, boundary, dwProtection, &dwProtection);

		// Flush instruction cache so our new code is guaranteed to be executed.
		FlushInstructionCache(GetCurrentProcess(), pFunc, boundary);

		return trampoline;
	}
#endif

	return NULL;
}

// WARNING: Don't call this if the function isn't detoured.
void UnDetourFunction(void *pFunc, void *pOriginal) {
#ifdef _WIN32
	// Should be about 5 bytes that were moved from original function to make room for the push and jump.
	int boundary = GetInstructionBoundary(pOriginal, jumpInstructionSize);
	if (boundary == -1)
		return;

	DWORD dwProtection;

	// Get write access to the function.
	if (VirtualProtect(pFunc, boundary, PAGE_EXECUTE_READWRITE, &dwProtection)) {
		// Move the original bytes back.
		memcpy(pFunc, pOriginal, boundary);

		// Restore the old protection
		VirtualProtect(pFunc, boundary, dwProtection, &dwProtection);

		// Free the trampoline function
		HeapFree(g_TrampolineHeap, 0, pOriginal);

		// Flush instruction cache so our new code is guaranteed to be executed.
		FlushInstructionCache(GetCurrentProcess(), pFunc, boundary);
	}
#endif
}

#pragma warning(default: 4244)