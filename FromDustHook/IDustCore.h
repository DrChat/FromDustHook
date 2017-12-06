#ifndef _IDUSTCORE_H_
#define _IDUSTCORE_H_

#include "LinBaseInterface.h"

class IDustCore : public IBaseInterface {
	public:
		virtual void unk4() = 0;
		virtual void unk5() = 0;
		virtual void unk6() = 0;
		virtual void unk7() = 0;
		virtual void unk8(char *buff) = 0; // Appears to require a buffer of at least 520 bytes (260 * 2)
		virtual void unk9() = 0;
		virtual void unk10() = 0;
		virtual void unk11() = 0;
		virtual void unk12() = 0;
		virtual void unk13() = 0;
		virtual void unk14() = 0;
		virtual void unk15() = 0;
		virtual void unk16() = 0;
		virtual void unk17() = 0;
		virtual void unk18() = 0;
		virtual void unk20() = 0;
		virtual int PlayMovie(const char *pName, bool unk) = 0;
		virtual void unk21() = 0;
		virtual void unk22() = 0;
};

#endif // _IDUSTCORE_H_