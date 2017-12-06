#ifndef _IREFLXCLASSREGISTRATOR_H_
#define _IREFLXCLASSREGISTRATOR_H_

#include "LinBaseInterface.h"

#undef GetClassInfo

namespace reflx {
	struct varinfo_t : public IBaseInterface {
		virtual int GetOffset(void *pClass) = 0; // Get variable offset. pClass can be NULL

		int unk1; // 4
		int unk2; // 8 appears to be derived from the name
		int type; // 12 var type? 1040=float, 1029=int, 1072=vector
		int unk4; // 16
		char unk5[4]; // 20 2 bool vals and filler
		int unk6; // 24	appears to always be 0
		int unk7; // 28	appears to always be 0
		int unk8; // 32	appears to always be 0
		int varSize; // 36 var size(?)
		int unk10; // 40 var size(again?)
		char *pName; // 44
		varinfo_t *pNext; // 48
		int offset; // 52 Offset from class start
	};

	struct classinfo_t;
	struct classchain_t {
		classinfo_t *pParent; // 0 parent class
		classinfo_t *pChild; // 4
		void *unk3; // 8
		void *unk4; // 12
	};

	struct classinfo_t {
		classchain_t *pChain; // 0
		varinfo_t *pFirstVariable; // 4 This is actually the last var, the chain is in reverse
		ifaceinfo_t *pInfo; // 8 typeinfo
		void *pConstructor; // 12
		void *pDestructor; // 16
		int id; // 20
		int size; // 24 size
		int unk8; // 28 some sort of com registration iface?
		int unk9; // 32
		int unk10; // 36
		int unk11; // 40
		int unk12; // 44
		int unk13; // 48
		void *unk14; // 52 Function (param 1 is this struct, param 2 unk)
		char *pName; // 56
	};

	class IClassRegistrator : public IBaseInterface {
		public:
			virtual classinfo_t *GetClassInfo(int uniqueId) = 0; // 12
			virtual void unk5() = 0; // 16 Points to GetClassInfo (duplicate of above)
			virtual void unk6() = 0; // 20
			virtual void unk7() = 0; // 24
			virtual bool IsClassRegistered(int uniqueId) = 0; // 28
			virtual void RegisterClass() = 0; // 32
			virtual void unk11() = 0; // 36
			virtual void unk12() = 0; // 40
			virtual void unk13() = 0; // 44
			virtual void unk14() = 0; // 48
			virtual void unk15() = 0; // 52
			virtual void unk16() = 0; // 56
			virtual void unk17() = 0; // 60
			virtual void unk18() = 0; // 64
			virtual void *unk19(const char *) = 0; // 68
			virtual void unk20() = 0; // 72
			virtual void *unk21(int uniqueId) = 0; // 76
	};
};

#endif // _IREFLXCLASSREGISTRATOR_H_