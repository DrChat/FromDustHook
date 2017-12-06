#ifndef _AIENGINE_H_
#define _AIENGINE_H_

#include <string>

#include "LinBaseInterface.h"

namespace ai {
	class Engine : public IBaseInterface {
		public:
			virtual void *GetScriptFunctions() = 0;

		private:
			char unk1[32]; // 4
			std::string unk2; // 36
	};
};

#endif // _AIENGINE_H_