#ifndef _IMEMORYOBJECT_H_
#define _IMEMORYOBJECT_H_

// It appears objects allocated by the memory interface must be derived from this interface...
class IMemoryObject {
	public:
		virtual void Destroy(bool freeMemory) = 0;
};

#endif // _IMEMORYOBJECT_H_