#ifndef _ARRAY_H_
#define _ARRAY_H_

template <typename T>
class CArray {
	public:
		CArray();

		inline int Size() {
			return m_numEntries;
		}

		inline T &operator[](int n) {
			return *(T *)((char *)m_pArr + n * m_entrySize);
		}

		inline const T operator[](int n) const {
			return *(T *)((char *)m_pArr + n * m_entrySize);
		}

	private:
		T *m_pArr; // 0
		int m_entrySize; // 4
		int m_unk3; // 8
		int m_numEntries; // 12
		int m_maxEntries; // 16
		int m_unk6; // 20
};

#endif // _ARRAY_H_