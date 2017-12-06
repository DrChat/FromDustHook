#ifndef _GALILEO_H_
#define _GALILEO_H_

#include "LinBaseInterface.h"

class GalileoInterface : public IBaseInterface {
	public:

	private:
		int munk_1; // 4
		void *m_pTextureManager; // 8
		void *m_pSimulatorWater; // 12
};

#endif // _GALILEO_H_