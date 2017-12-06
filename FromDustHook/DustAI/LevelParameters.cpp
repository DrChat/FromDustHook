#include "LevelParameters.h"

#include "../Redirect.h"

static char modName[] = "dustai.win32.f.dll";

void *CLevelParameters::pOrigChangeGamemode = 0;

//GAME_FUNCTION
void CLevelParameters::ChangeGameMode(GameMode newGameMode) {
	/*
	__asm PatchStart:
	__asm push edx;
	__asm push ecx;
	__asm push offset PatchEnd;
	__asm push offset PatchStart;
	__asm push 0x34BA0;
	__asm push offset modName;
	__asm call PatchRedirect;
	__asm PatchEnd:
	__asm add esp, 16;
	__asm pop ecx;
	__asm pop edx;
	__asm jmp eax;
	*/

	if (pOrigChangeGamemode) {
		return (this->*VTOF<void (CLevelParameters::*)(int)>(pOrigChangeGamemode))(newGameMode);
	}
}

int CLevelParameters::GetGameMode() {
	if (m_gameMode == 2)
		return m_gameMode;
	else if (m_gameMode == 3 || m_gameMode == 4)
		return 1; // Gamemodes 3 and 4 redirect to 1

	return m_gameMode;
}