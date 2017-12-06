#ifndef _REFLXCLASSREGISTRATOR_H_
#define _REFLXCLASSREGISTRATOR_H_

#include <map>

#include "IReflxClassRegistrator.h"

namespace reflx {
	class CClassRegistrator : public IClassRegistrator {
		public:

		public:
			char unk[24]; // 4 a hash map
			// std::hash_map<int, classinfo_t> m_classMap; // 28
	};
};

#endif // _REFLXCLASSREGISTRATOR_H_