#include "Redirect.h"

#include <Windows.h>

#include "Detour.h"

static const char	breakpointInstruction = '\xCC';

void *PatchRedirect(const char *libName, void *offset, void *patchStart, void *patchEnd) {
	void *jmpAddr = ResolveLibraryAddress(libName, offset);
	if (!jmpAddr)
		__asm int 3; // welp, crash time.

	DWORD dwOldProtect;
	VirtualProtect(patchStart, (int)patchEnd - (int)patchStart, PAGE_EXECUTE_READWRITE, &dwOldProtect);

	// Write breakpoints
	memset(patchStart, breakpointInstruction, (int)patchEnd - (int)patchStart);

	// Now write the jump
	WriteJump(patchStart, jmpAddr);

	// Restore the old protection
	VirtualProtect(patchStart, (int)patchEnd - (int)patchStart, PAGE_EXECUTE, &dwOldProtect);

	// Flush the instruction cache so our new instructions get executed.
	FlushInstructionCache(GetCurrentProcess(), patchStart, (int)patchEnd - (int)patchStart);

	// Return the address to jump to
	return jmpAddr;
}