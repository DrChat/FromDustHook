#include <Windows.h>
#include <string>

#include <d3d9.h>

#include "Array.h"
#include "Detour.h"
#include "library.h"

#include "IComClassManager.h"
#include "IDustCore.h"
#include "IFIRe.h"
#include "IReflxClassRegistrator.h"
#include "IDustManager.h"
#include "DustAI/UI_AbstractInterface.h"

#include "AIEngine.h"
#include "ReflxClassRegistrator.h"
#include "ScriptInvoke.h"

#include "DustAI/LevelParameters.h"

#include <vector>

// Suppress some errors.
#define stricmp _stricmp
#define sprintf sprintf_s
#define strcat strcat_s

IComClassManager *g_pClassMgr = NULL;

void ChangeROVariable(void *pVar, void *pNewVar, unsigned int varSize) {
	// Give ourselves write permissions
	DWORD dwOldProtect;
	VirtualProtect(pVar, varSize, PAGE_READWRITE, &dwOldProtect);

	// Copy the variable
	memcpy(pVar, pNewVar, varSize);

	// Restore original protections
	VirtualProtect(pVar, varSize, dwOldProtect, &dwOldProtect);
}

struct DustManagerImpl_t {
	char unk1[24]; // 0
	CLevelParameters *pLevelParams; // 24
	char unk2[36]; // 28
	void *pEditorUI; // 64
	void *pGameUI; // 68
	void *pUIInputMgr; // 72
	char unk3[620]; // 76
	std::vector<int> vec; // 696
};

WNDPROC origWndProc = NULL;
LRESULT CALLBACK HookedWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
		case WM_KEYDOWN: {
			if (!g_pClassMgr)
				break;

			// F2 to toggle between ingame/editor gamemodes
			if (wParam == VK_F2) {
				// TODO: LevelParameters exists on DustManager at offset 24
				// Editor UI interface at offset 64
				// Game UI interface at offset 68
				// Both interfaces are derived from abstract ui interface
				DustManagerImpl_t *pDustMgr = (DustManagerImpl_t *)g_pClassMgr->GetSingletonImpl("DustManagerImplementation");

				if (pDustMgr) {
					CLevelParameters *pLevelParams = pDustMgr->pLevelParams;
					if (pLevelParams) {
						DustAi::UI_AbstractInterface *pGameUI = (DustAi::UI_AbstractInterface *)pDustMgr->pGameUI;
						DustAi::UI_AbstractInterface *pEditorUI = (DustAi::UI_AbstractInterface *)pDustMgr->pEditorUI;

						// Toggle between editor (1) and ingame (2)
						// TODO: When going from ingame to editor, if you were scrolling the screen ingame (cursor at edge)
						// the camera will be locked to a certain radius in the editor view.
						// TODO: Can't switch from editor to ingame easily. Need to invoke the pause menu somehow.

						// Kids, don't try this at home!
						pLevelParams->ChangeGameMode(pLevelParams->GetGameMode() == GAMEMODE_EDITOR ? GAMEMODE_INGAME : GAMEMODE_EDITOR);
					}
				}
			} else if (wParam == VK_F3) {
				// script::Invoke::PushString("cool!");
				// script::Invoke::Push(Vector3(0.5f, 0.5f, 0));
				// script::Invoke::Push(5.f);
				// script::Invoke::Call("STR_Display");

				// int ret = script::Invoke::Pop<int>();
			} else if (wParam == VK_F4) {
				// Framerate cap
				float newFrameCap = *(float *)0x008A3104 == 30.f ? 60.f : 30.f;
				float invFrameCap = 1 / newFrameCap;
				ChangeROVariable((void *)0x008A3104, (void *)&newFrameCap, sizeof(float));
				ChangeROVariable((void *)0x008A13D8, (void *)&invFrameCap, sizeof(float));

				// Update time
				script::Invoke::Push<float>(newFrameCap == 60.f ? 1.f : 1.f);
				script::Invoke::Call("TIME_SetFactor");
			}

			break;
		}
	}

	if (origWndProc)
		return origWndProc(hWnd, uMsg, wParam, lParam);

	return 0;
}

void DumpReflxInfo(HANDLE hFile, reflx::IClassRegistrator *pReg, int uniqueId) {
	char buff[512];
	
	// pReg offset 32 contains 2 pointers
	// each pointing to a classinfo struct (at offset 12)
	// Is this a tree or something?

	reflx::classinfo_t *pCurClass = pReg->GetClassInfo(uniqueId);

	DWORD dwNumBytes = 0;
	DWORD dwNumBytesWritten = 0;

	bool inherits = pCurClass->pChain && pCurClass->pChain->pParent;

	dwNumBytes = sprintf(buff, "// %d bytes (unique ID 0x%08x)", pCurClass->size, uniqueId);

	if (inherits) {
		reflx::classinfo_t *pParent = pCurClass->pChain->pParent;
		sprintf(buff, "%s (inherits %s of %d bytes)", buff, pParent->pName, pParent->size);
	}

	dwNumBytes = sprintf(buff, "%s\r\nstruct %s {\r\n", buff, pCurClass->pName);
	WriteFile(hFile, buff, dwNumBytes, &dwNumBytesWritten, NULL);

	// This chain is actually in reverse order, so we'll shove it into a vector.
	reflx::varinfo_t *pPrev = NULL;
	reflx::varinfo_t *pCurVar = pCurClass->pFirstVariable;

	std::vector<reflx::varinfo_t *> variable_infos;

	// Shove the list into a vector.
	while (pCurVar) {
		variable_infos.push_back(pCurVar);
		pCurVar = pCurVar->pNext;
	}

	// Address beginning right after the last var
	int prevEnd = 0;
	int curVar = 0;

	// Run through the list in reverse order.
	for (int i = variable_infos.size() - 1; i--; i >= 0) {
		// Insert padding if needed
		curVar++;

		pCurVar = variable_infos[i];
		if (pCurVar->offset > prevEnd) {
			dwNumBytes = sprintf(buff, "\tchar \tunk%d[%d]; // %d\r\n", curVar, pCurVar->offset - prevEnd, prevEnd);
			WriteFile(hFile, buff, dwNumBytes, &dwNumBytesWritten, NULL);
		}

		prevEnd = pCurVar->offset + pCurVar->varSize; // Point to the begin byte of next var

		char *varType = NULL;
		switch (pCurVar->type) {
			case 1029: { // int
				varType = "int";
				break;
			}
			case 1040: { // float
				varType = "float";
				break;
			}
			case 1072: { // vector
				varType = "Vector3";
				break;
			}
		}

		if (varType) {
			dwNumBytes = sprintf(buff, "\t%s \t%s; \t// %d\r\n", varType, pCurVar->pName, pCurVar->offset);
		} else {
			dwNumBytes = sprintf(buff, "\tchar \t%s[%d]; \t// %d type %d\r\n", pCurVar->pName, pCurVar->varSize, pCurVar->offset, pCurVar->type);
		}

		//dwNumBytes = sprintf(buff, "\t\"%s\" - offset %d (0x%x) size %d\r\n", pCurVar->pName, pCurVar->offset, pCurVar->offset, pCurVar->varSize);
		WriteFile(hFile, buff, dwNumBytes, &dwNumBytesWritten, NULL);
	}

	// Append padding if needed
	if (prevEnd < pCurClass->size) {
		curVar++;
		dwNumBytes = sprintf(buff, "\tchar \tunk%d[%d]; // %d\r\n", curVar, pCurClass->size - prevEnd, prevEnd);
		WriteFile(hFile, buff, dwNumBytes, &dwNumBytesWritten, NULL);
	}

	dwNumBytes = sprintf(buff, "};\r\n\r\n");
	WriteFile(hFile, buff, dwNumBytes, &dwNumBytesWritten, NULL);

	if (pCurClass->pChain && pCurClass->pChain->pParent)
		pCurClass = pCurClass->pChain->pParent;
	else
		pCurClass = NULL;
}

// 28 bytes
struct scriptentry_t {
	void *unk1; // 0
	void *pFn1; // 4 script func
	void *pFn2; // 8 c++ func
	int unk4; // 12
	char *unk5; // 16
	int unk6; // 20 entry type? 4 is a function, 1 is a type
	char *pName; // 24
};

struct fileentry_t {
	int unk1; // 0
	char *unk2; // 4
	char *unk3; // 8
	char *unk4; // 12
	char *unk5; // 16
	char *unk6; // 20
	char *unk7; // 24
	char *unk8; // 28 points to a class instance
	char *unk9; // 32
	char *unk10; // 36
};

typedef int(__cdecl *MainLoopFn)();
MainLoopFn origMainLoop = NULL;

// Game's init should be done at this phase!
int HookedMainLoop() {
	// dustai detours
	// Test hook
	DetourFunction(ResolveLibraryAddress("dustai.win32.f.dll", (void *)0x000343F0), FTOV(&CLevelParameters::GetGameMode));
	CLevelParameters::pOrigChangeGamemode = DetourFunction(ResolveLibraryAddress("dustai.win32.f.dll", (void *)0x00034BA0), FTOV(&CLevelParameters::ChangeGameMode));

	// This should be initialized at this point...
	g_pClassMgr = *(IComClassManager **)0x0094B4A4;
	if (g_pClassMgr) {
#ifdef _DEBUG
		char buff[512];
		DWORD dwNumBytes = sprintf(buff, "%-48s %-48s %-8s %-8s %-10s %-10s %-10s\r\n\r\n",
			"ClassName", "Interface", "SizeOf", "ModuleID", "Singleton", "&TypeInfo", "Factory");
		DWORD dwNumBytesWritten;

		HANDLE hFile = CreateFileA("clsinfo.txt", GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		WriteFile(hFile, buff, dwNumBytes, &dwNumBytesWritten, NULL);

		struct classmgr {
			void *vtable; // 0
			char unk[32]; // 4
			clstypeinfo_t **typeInfoArr; // 36
			int numClasses; // 40
		};
		classmgr *mgr = *(classmgr **)&g_pClassMgr;

		for (int i = 0; i < mgr->numClasses; i++) {
			clstypeinfo_t *info = mgr->typeInfoArr[i];

			dwNumBytes = sprintf(buff, "%-48s %-48s %-8d %-8d 0x%08x 0x%08x 0x%08x\r\n",
				info->ifaceinfo->pImplName, info->ifaceinfo->pName, info->ifaceinfo->size, info->moduleID,
				info->ifaceinfo->pSingleton, info->ifaceinfo, info->ifaceinfo->pFactory);
			WriteFile(hFile, buff, dwNumBytes, &dwNumBytesWritten, NULL);
		}

		dwNumBytes = sprintf(buff, "\r\n%u class(es) registered through com system\r\n\r\n", mgr->numClasses);
		WriteFile(hFile, buff, dwNumBytes, &dwNumBytesWritten, NULL);

		dwNumBytes = sprintf(buff, "dustai base address: 0x%08x\r\n\r\n", ResolveLibraryBase("dustai.win32.f.dll"));
		WriteFile(hFile, buff, dwNumBytes, &dwNumBytesWritten, NULL);

		/*******************************
		* DUMP INTROSPECTION INFO
		*******************************/
		dwNumBytes = sprintf(buff, "Introspection dump\r\n");
		WriteFile(hFile, buff, dwNumBytes, &dwNumBytesWritten, NULL);

		reflx::CClassRegistrator *pReg = (reflx::CClassRegistrator *)g_pClassMgr->GetSingletonImpl("reflx::ClassRegistrator");
		// void *pRet = pReg->unk19((void*)pReg, "DustAi::UI_GameInterface");

		/*
		struct {
			void *cont;
		} dummy;
		dummy.cont = (void *)&pReg->m_classMap;

		for (auto i = pReg->m_classMap.begin(); ; *(char **)&i = (char *)&dummy, i++) {
			// BS the container
			auto &end = pReg->m_classMap.end();
			*(char **)&end = (char *)&dummy;
			*(char **)&i = (char *)&dummy;

			if (i == end) {
				*(char **)&end = NULL;
				*(char **)&i = NULL;

				break;
			}

			DumpReflxInfo(hFile, pReg, i->first);

			*(char **)&end = NULL;
			*(char **)&i = NULL;
		}

		//DumpReflxInfo(hFile, pReg, 0xBBD44F13); // DustAi::UI_InputManager
		*/

		CloseHandle(hFile);
#endif
	}

	if (origMainLoop)
		return origMainLoop();

	return 1;
}

typedef bool (__cdecl *ParseCommandLineFn)(const char *cmdLine);
ParseCommandLineFn	origParseCommandLine = NULL;

// Purpose: Read startup.txt and inject that into the command line instead.
bool __cdecl HookedParseCommandLine(const char *origCmdLine) {
	char cmdLine[1024];

	HANDLE hStartupFile = CreateFileA("startup.txt", GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hStartupFile != INVALID_HANDLE_VALUE) {
		DWORD dwNumBytesRead;
		char buff[1000];
		if (ReadFile(hStartupFile, buff, 1000, &dwNumBytesRead, NULL)) {
			strncpy_s(cmdLine, buff, 1000);
			cmdLine[dwNumBytesRead] = '\0';
		}
	} else {
		strcpy_s(cmdLine, origCmdLine);
	}

	CloseHandle(hStartupFile);

	return origParseCommandLine(cmdLine);
}

typedef HANDLE (WINAPI *CreateFileAFn)(
	LPCSTR lpFileName,
	DWORD dwDesiredAccess,
	DWORD dwShareMode,
	LPSECURITY_ATTRIBUTES lpSecurityAttributes,
	DWORD dwCreationDisposition,
	DWORD dwFlagsAndAttributes,
	HANDLE hTemplateFile
);
CreateFileAFn	origCreateFile = NULL;

HANDLE WINAPI HookedCreateFileA(LPCSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile) {
	const char *ext = strchr((const char *)lpFileName, '.');
	const char *fmt = NULL;

	if (ext) {
		if (!stricmp(ext, ".lin.bf")) {
			fmt = "CreateFileA: Loading data file %s\n";
		} else if (!stricmp(ext, ".sav")) {
			fmt = "CreateFileA: Loading save file %s\n";
		} else if (!stricmp(ext, ".inf")) {
			fmt = "CreateFileA: Loading info file %s\n";
		} else if (!stricmp(ext, ".xml")) {
			fmt = "CreateFileA: Loading xml file %s\n";
		}

		if (fmt) {
			char buff[1024];
			sprintf_s(buff, fmt, lpFileName);
			OutputDebugStringA(buff);
		}
	}
	
	if (origCreateFile)
		return origCreateFile(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);

	return INVALID_HANDLE_VALUE;
}

typedef BOOL (WINAPI *ReadFileFn)(
	HANDLE hFile,
	LPVOID lpBuffer,
	DWORD nNumberOfBytesToRead,
	LPDWORD lpNumberOfBytesRead,
	LPOVERLAPPED lpOverlapped
);
ReadFileFn	origReadFile = NULL;

BOOL WINAPI HookedReadFile(HANDLE hFile, LPVOID lpBuffer, DWORD nNumberOfBytesToRead, LPDWORD lpNumberOfBytesRead, LPOVERLAPPED lpOverlapped) {
	TCHAR fileName[MAX_PATH];
	if (GetFinalPathNameByHandle(hFile, fileName, sizeof(fileName) / sizeof(TCHAR), FILE_NAME_OPENED)) {
		DWORD dwPos;
		dwPos = SetFilePointer(hFile, 0, NULL, FILE_CURRENT);

		char buff[512];
		sprintf_s(buff, "Reading %d bytes at 0x%p from file %s\n", nNumberOfBytesToRead, dwPos, fileName);
		OutputDebugStringA(buff);
	}

	if (origReadFile)
		return origReadFile(hFile, lpBuffer, nNumberOfBytesToRead, lpNumberOfBytesRead, lpOverlapped);

	return FALSE;
}

typedef BOOL (WINAPI *WriteFileFn)(
	_In_         HANDLE hFile,
	_In_         LPCVOID lpBuffer,
	_In_         DWORD nNumberOfBytesToWrite,
	_Out_opt_    LPDWORD lpNumberOfBytesWritten,
	_Inout_opt_  LPOVERLAPPED lpOverlapped
	);
WriteFileFn origWriteFile = NULL;

BOOL WINAPI HookedWriteFile(HANDLE hFile, LPCVOID lpBuffer, DWORD nNumberOfBytesToWrite, LPDWORD lpNumberOfBytesWritten, LPOVERLAPPED lpOverlapped) {
	TCHAR fileName[MAX_PATH];
	if (GetFinalPathNameByHandle(hFile, fileName, sizeof(fileName) / sizeof(TCHAR), FILE_NAME_OPENED)) {
		const char *ext = strchr(fileName, '.');
		DWORD dwPos;
		dwPos = SetFilePointer(hFile, 0, NULL, FILE_CURRENT);

		char buff[512];
		sprintf_s(buff, "Writing %d bytes at 0x%p to file %s\n", nNumberOfBytesToWrite, dwPos, fileName);
		OutputDebugStringA(buff);
	}

	if (origWriteFile)
		return origWriteFile(hFile, lpBuffer, nNumberOfBytesToWrite, lpNumberOfBytesWritten, lpOverlapped);

	return FALSE;
}

BOOL WINAPI DllMain(HINSTANCE hInstDLL, DWORD dwReason, LPVOID) {
	switch (dwReason) {
		case DLL_PROCESS_ATTACH: {
#ifdef _DEBUG
			MessageBoxA(GetDesktopWindow(), "Waiting for debugger...", "From Dust Hook", MB_ICONINFORMATION | MB_OK);
#endif

			// Don't need these. (DLL_THREAD_ATTACH, etc)
			DisableThreadLibraryCalls(hInstDLL);

			// Redirect any functions we have overridden
			if (!Init_DefaultLibrary())
				return FALSE;

			// TODO: Check game file hashes...

			// Game hooks
			origParseCommandLine = (ParseCommandLineFn)DetourFunction((void *)0x005BB440, HookedParseCommandLine);
			origMainLoop = (MainLoopFn)DetourFunction((void *)0x00412A10, HookedMainLoop);
			origWndProc = (WNDPROC)DetourFunction((void *)0x00412DA0, HookedWndProc);

			// Windows API hooks
#ifdef _DEBUG
			/*
			origCreateFile = (CreateFileAFn)DetourFunction(CreateFileA, HookedCreateFileA);
			origReadFile = (ReadFileFn)DetourFunction(ReadFile, HookedReadFile);
			origWriteFile = (WriteFileFn)DetourFunction(WriteFile, HookedWriteFile);
			*/
#endif

			break;
		}
		case DLL_PROCESS_DETACH: {
			Close_DefaultLibrary();

			UnDetourFunction((void *)0x005BB440, origParseCommandLine);
			UnDetourFunction((void *)0x00412A10, origMainLoop);
			UnDetourFunction((void *)0x00412DA0, origWndProc);

#ifdef _DEBUG
			/*
			UnDetourFunction(CreateFileA, origCreateFile);
			UnDetourFunction(ReadFile, origReadFile);
			UnDetourFunction(WriteFile, origWriteFile);
			*/
#endif
			break;
		}
	}

	return TRUE;
}