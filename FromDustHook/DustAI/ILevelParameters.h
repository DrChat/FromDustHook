#ifndef _ILEVELPARAMETERS_H_
#define _ILEVELPARAMETERS_H_

#include "../LinBaseInterface.h"

class ILevelParameters : public IBaseInterface {
	public:
		virtual int unk4() = 0;
		virtual int unk5() = 0;
		virtual int unk6() = 0;
		virtual int unk7() = 0;
		virtual int unk8() = 0;
		virtual int unk9() = 0;
		virtual int unk10() = 0;
		virtual int unk11() = 0;
		virtual int ToggleSomething(int thing, int unk2) = 0; // thing = 3 toggles gamemode
};

#endif // _ILEVELPARAMETERS_H_