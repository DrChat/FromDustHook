#ifndef _IABSTRACTUI_H_
#define _IABSTRACTUI_H_

#include "../LinBaseInterface.h"

#include "../Vector.h"

namespace DustAi {
	class UI_AbstractInterface : public IBaseInterface {
		public:

		public:
			char 	unk1[20]; // 4
			float 	f_DistanceToPJ; 	// 24
			float 	f_DistanceMinToPJ; 	// 28
			float 	f_DistanceMaxToPJ; 	// 32
			float 	f_StepZoom; 	// 36
			float 	f_StepDiscretZoom; 	// 40
			int 	i_useStickForDisplacement; 	// 44
			int 	i_forceCameraAngles; 	// 48
			float 	f_CameraDelta; 	// 52
			float 	f_CameraAlpha; 	// 56
			Vector3 v_CameraSight; 	// 60
			Vector3 v_CameraHorizon; 	// 72
			float 	f_CameraCurrentDistance; 	// 84
			Vector3 v_CameraPosition; 	// 88
			Vector3 v_CursorVirtualPos; 	// 100
			int 	i_cameraDeltaVariationsActive; 	// 112
			Vector3 v_joystickValues; 	// 116
			Vector3 v_PosCursor; 	// 128
			Vector3 v_OldPosCursor; 	// 140
			char 	c_DeplacementCurseur[8]; 	// 152 type 1073
			char 	c_CursorSpeedFromNorm[8]; 	// 160 type 1073
			int 	i_displayCursor; 	// 168
			int 	i_ForcePosCursor; 	// 172
			Vector3 v_ForcePosCursor; 	// 176
			float 	f_SnappingSizeFactor; 	// 188
			float 	f_SnappingSizeFactorGem; 	// 192
			float 	f_DistancePerSecondThreshold; 	// 196
			char 	c_ManageCursorSize[8]; 	// 200 type 1073
			char 	c_ManageCursorSnap[8]; 	// 208 type 1073
			char 	o_CursorVisual[8]; 	// 216 type 1064
			float 	m_accelCameraFactorHelico; 	// 224
			float 	m_accelCameraFactorGlobal; 	// 228
			int 	i_useProjectedCursor; 	// 232
			float 	f_CameraDistanceInit; 	// 236
			float 	f_CameraDistanceMin; 	// 240
			float 	f_CameraDistanceMax; 	// 244
			int 	m_isInTravellingMode; 	// 248
			int 	m_exitTravellingMode; 	// 252
			float 	m_cameraFocal; 	// 256
			char 	unk39[136]; // 260
	};
};

#endif // _IABSTRACTUI_H_