#ifndef _IFIRE_H_
#define _IFIRE_H_

#include "LinBaseInterface.h"

namespace FIRe {
	class FIRe_Interface : public IBaseInterface {
		public:
			// Fire an event into the event queue. Appears to be threadsafe.
			virtual int FireEvent(void *unk1, wchar_t *evt, int paramCount, void *params) = 0; // 12
			virtual void unk6() = 0; // 16 duplicate of above
			virtual void toggle(int toggle) = 0; // 20
	};
};

#endif // _IFIRE_H_