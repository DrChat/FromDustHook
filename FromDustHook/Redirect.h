#ifndef _REDIRECT_H_
#define _REDIRECT_H_

#include "Detour.h"

void *PatchRedirect(const char *libName, void *offset, void *patchStart, void *patchEnd);

// Purpose: Redirect function calls into the game (for mapping out classes, etc)
#define GAME_FUNCTION __declspec(naked)
#define AT(addr)		\
{						\
	__asm jmp addr;		\
}

// Use this for jumping to a function in a submodule. Due to inline assembler limitations,
// libname must be a variable defined in C++. Preferably define it at the top of the file as:
// char libName[] = "libname.dll"
#define AT_RESOLVE(libname, reladdr)

#endif // _REDIRECT_H_