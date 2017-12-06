#ifndef _DETOUR_H_
#define _DETOUR_H_

// NOTE: DO NOT USE THIS ON x64 BUILDS! THIS IS ONLY COMPATIBLE WITH x86!

// Dirty nasty hack to convert anything to a void pointer
template <typename T>
void *FTOV(T fn) {
	return *(void **)&fn;
}

// Dirty nasty hack that does the reverse of the above
template <typename T>
T VTOF(void *pFn) {
	return *(T *)&pFn;
}

// Resolve an address that resides inside of a library loaded at a dynamic base
void *ResolveLibraryAddress(const char *modName, void *pAddress);

// Resolve library base
void *ResolveLibraryBase(const char *modName);

// Returns: Pointer to call original function
// It's up to you to keep track of this pointer if you want to remove the detour.
void *DetourFunction(void *pFunc, void *detourFn);

// WARNING: Don't call this if the function isn't detoured.
// WARNING: Untested as of now.
// Call with the detoured function, and the pointer we gave you earlier to call the original.
// After you call this, the pointer to the "original" function will become invalid.
void UnDetourFunction(void *pFunc, void *pOriginal);

// Some misc. functions
void WriteJump(void *base, void *jumpadr);

#endif // _DETOUR_H_