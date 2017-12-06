#include "library.h"

#include <Windows.h>

HMODULE		g_hDefLib		= NULL;
void *		g_pDefCreateFx	= NULL;

// HRESULT WINAPI CreateFX(CLSID clsid, IUnknown **pEffect, void *pInitData, UINT32 InitDataByteSize)
extern "C" __declspec(dllexport) __declspec(naked) HRESULT CreateFX(CLSID clsid, IUnknown **pEffect, void *pInitData, UINT32 InitDataByteSize) {
	// #pragma comment(linker, "/EXPORT:"__FUNCTION__"="__FUNCDNAME__)

	__asm jmp g_pDefCreateFx;
}

bool Init_DefaultLibrary() {
	// Load the default library.
	char dir[MAX_PATH];
	GetSystemDirectoryA(dir, MAX_PATH);
	strcat_s(dir, "\\XAPOFX1_5.dll");
	g_hDefLib = LoadLibraryA(dir);
	if (!g_hDefLib)
		return false;

	g_pDefCreateFx = GetProcAddress(g_hDefLib, "CreateFX");
	if (!g_pDefCreateFx)
		return false;

	return true;
}

void Close_DefaultLibrary() {
	FreeLibrary(g_hDefLib);
	g_pDefCreateFx = NULL;
	g_hDefLib = NULL;
}