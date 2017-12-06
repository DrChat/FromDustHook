#ifndef _LINBASEINTERFACE_H_
#define _LINBASEINTERFACE_H_

#include "IMemoryObject.h"

typedef void *(FactoryFn)(void *pMem);
struct ifaceinfo_t {
	const char *pImplName;	// 0
	FactoryFn *pFactory;	// 4
	int size;				// 8
	void *pSingleton;		// 12
	const char *pName;		// 16
	ifaceinfo_t *pNext;		// 20 engine interfaces appear to be chained but not dustai ones
};

// LyN/Lin interfaces appear to be derived from this base
class IBaseInterface : public IMemoryObject {
	public:
		// Unknown, returns some random number (I've only seen 1 or 3 returned...)
		virtual int unk2() = 0; // 4

		// Returns a pointer to a struct containing info about this interface
		virtual const ifaceinfo_t *GetInfo() = 0; // 8
};

#endif // _LINBASEINTERFACE_H_