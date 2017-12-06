#include "ScriptInvoke.h"

namespace script {
	Context *g_pContext = (Context *)0x0093FE6C;
	CArray<Entry> &g_pEntries = *(CArray<Entry> *)0x009384C0;

	Invoke::Func Invoke::FindFn(const char *pName) {
		// Find the function in the list
		Func pFnS = NULL;

		for (int i = 0; i < g_pEntries.Size(); i++) {
			Entry &entry = g_pEntries[i];
			if (!strcmp(pName, entry.pName)) {
				pFnS = (Func)entry.pFnS;
				break;
			}
		}

		return pFnS;
	}

	void Invoke::Call(const char *pName) {
		// Find the function in the list
		Func pFnS = FindFn(pName);

		if (!pFnS)
			return;

		// Found the function. We're assuming the stack is setup correctly,
		// otherwise we're going to CRASH TO SHIT
		// Now call it.
		// TODO: How is argument 0 used?
		// I think argument 0 is a pointer to the current script instruction.
		// The function return value is a pointer to the next instruction to be executed.
		int ret = pFnS(0);
	}
};