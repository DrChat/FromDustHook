#ifndef _SCRIPTCONTEXT_H_
#define _SCRIPTCONTEXT_H_

// Script function invokation
// All script functions appear to take one C argument, and they all return
// that argument + 4 (is it the new stack top or something?)
// Argument pointers are pushed on to some sort of temporary stack

// Return value is pushed on to the temp stack and a pointer to that is placed
// into a different stack

// for memcpy
#include <string.h>

#include "Array.h"

namespace script {
	// Located at static pointer 0x0093FE6C
	struct Context {
		int unk1;

		int stackTop;
		int memTop;

		int unk4; // Some sort of memory pool size 0x20000

		char *pMemBase;		// size 0x40000
		void **pStackBase;	// size 0x4000
	};
	extern Context *g_pContext;

	// The game mantains a list of script functions and types
	struct Entry {
		void *unk1; // 0
		void *pFnS; // 4 script func
		void *pFnC; // 8 c++ func
		int unk4; // 12
		char *unk5; // 16
		int entryType; // 20 entry type? 4 is a function, 1 is a type, 2 is keyword/token
		char *pName; // 24
	};
	extern CArray<Entry> &g_pEntries;

	// Helper class for invoking script functions
	class Invoke {
		public:
			typedef int (*Func)(int arg);

			template <typename T>
			static inline T *Push(T elem) {
				return Push(&elem);
			}

			template <typename T>
			static inline T *Push(T *elem) {
				return (T *)Push(elem, sizeof(T));
			}

			static inline const char **PushString(const char *pString) {
				// Copy a pointer to the string onto the stack, NOT the actual string
				return (const char **)Push(&pString, 4);
			}

			static inline void *Push(void *pElem, int size) {
				void *pPtr = g_pContext->pMemBase + g_pContext->memTop;
				memcpy(pPtr, pElem, size);

				// Increment memory top
				g_pContext->memTop += size;

				// Okay, now drop the pointer into the stack (and increment stack top)
				g_pContext->pStackBase[g_pContext->stackTop++] = pPtr;

				return (void *)pPtr;
			}

			// Pop from the stack. n < 0 is from top, n > 0 from bottom, n = 0 topmost element
			template <typename T>
			static inline T Pop(int n = 0) {
				// Decrement the memory stack
				g_pContext->memTop -= sizeof(T);

				// Return the pointer to it from the pointer stack
				// (duplicate it because it will no longer be guaranteed a spot on the stack)
				g_pContext->stackTop--;
				return *(T *)g_pContext->pStackBase[g_pContext->stackTop];
			}

			static Func FindFn(const char *pName);

			// Call a script function.
			static void Call(const char *pName);

		private:
	};
};

#endif // _SCRIPTCONTEXT_H_