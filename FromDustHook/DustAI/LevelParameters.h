#ifndef _LEVELPARAMETERS_H_
#define _LEVELPARAMETERS_H_

#include "ILevelParameters.h"

#include "../Vector.h"

enum GameMode {
	GAMEMODE_MENU = 0,
	GAMEMODE_EDITOR = 1,
	GAMEMODE_INGAME = 2,
};

class CLevelParameters : public ILevelParameters {
	public:
		void ChangeGameMode(GameMode newGameMode);
		static void *pOrigChangeGamemode;

		int GetGameMode();


	private:
		char 	unk1[16]; // 0
		int 	m_enableAIThread; 	// 20
		Vector3 	m_windDirection; 	// 24
		char 	m_fonts[8]; 	// 36 type 1064
		char 	m_knowledgeBase[8]; 	// 44 type 1064
		char 	m_powerBase[8]; 	// 52 type 1064
		char 	m_circle[8]; 	// 60 type 1064
		char 	m_cursor[8]; 	// 68 type 1064
		char 	m_callTotem[8]; 	// 76 type 1064
		char 	m_cameraOBJ[8]; 	// 84 type 1064
		char 	m_kite[8]; 	// 92 type 1064
		char 	m_pathFindGoodObj[8]; 	// 100 type 1064
		char 	m_pathFindWrongObj[8]; 	// 108 type 1064
		float 	m_pathFindObjSpeed; 	// 116
		float 	m_pathFindObjFrequency; 	// 120
		float 	m_pathFindObjZoffset; 	// 124
		int 	m_pathFindObjSmoothPath; 	// 128
		float 	m_pathFindObjZOffsetNegatif; 	// 132
		float 	m_pathFindObjDyingTime; 	// 136
		char 	m_pathFindDestinationObj[8]; 	// 140 type 1064
		int 	m_gameMode; 	// 148
		int 	m_previousGameMode; 	// 152
		int 	m_gameModeHasChanged; 	// 156
		int 	m_editionLayer; 	// 160
		int 	m_fakeLayer; 	// 164
		float 	m_globalEmphasisCoeff; 	// 168
		int 	m_enableAmasLog; 	// 172
		int 	m_enableOrdersLog; 	// 176
		int 	m_enablePerceptionLog; 	// 180
		int 	m_enableRecorderLog; 	// 184
		int 	m_enableMusicLog; 	// 188
		int 	m_enableSound1Log; 	// 192
		int 	m_enableSound2Log; 	// 196
		int 	m_enableSound3Log; 	// 200
		int 	m_enableSound4Log; 	// 204
		int 	m_enableWorldInfoLog; 	// 208
		int 	m_enableGridInfoLog; 	// 212
		int 	m_enableConnexityLog; 	// 216
		int 	m_enablePathfindLog; 	// 220
		int 	m_enableActionsLog; 	// 224
		int 	m_enableVegetationDetectionLog; 	// 228
		char 	m_brushesTab[64]; 	// 232 type 4146
		char 	m_ghostStatue[48]; 	// 296 type 4128
		char 	m_vegetToCount[16]; 	// 344 type 4101
		int 	m_nbVegetCell; 	// 360
		char 	m_humanCounter[8]; 	// 364 type 1064
		char 	unk46[44]; // 372
		float	m_hardgroundHeight; // 416 infinite hardground height
		float	m_softgroundHeight; // 420 infinite softground height
		char	unk48[28]; // 424
};

#endif // _LEVELPARAMETERS_H_