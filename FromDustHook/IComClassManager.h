#ifndef _ICOMCLASSMANAGER_H_
#define _ICOMCLASSMANAGER_H_

#include "LinBaseInterface.h"

struct clstypeinfo_t {
	ifaceinfo_t *ifaceinfo;
	int moduleID;
};

// The game's class manager interface...
class IComClassManager : public IBaseInterface {
	public:
		virtual clstypeinfo_t *GetTypeInfoImpl(const char *) = 0; // 12
		virtual void unk5() = 0; // 16
		virtual IBaseInterface *CreateClass(const char *pName) = 0; // 20
		virtual void DestroyClass(IBaseInterface *pClass) = 0; // 24 destroys class created with CreateClass
		virtual IBaseInterface *GetSingletonImpl(const char *pName) = 0; // 28 Gets an implementation singleton
		virtual clstypeinfo_t *GetTypeInfo(const char *pName) = 0; // 32

		// Not even sure what this function does...
		// It sprintf()s a bunch of diagnostic class information into a string on the stack
		// but does not return this string or store it. The information also gets
		// overwritten too! Returns the result of the last sprintf().
		virtual int DumpClassInfo() = 0; // 36
};

#endif // _ICOMCLASSMANAGER_H_