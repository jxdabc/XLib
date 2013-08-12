#pragma once

#include <atlstr.h>
#include <vector>

namespace XLibS
{
	namespace StringHelper
	{
		BOOL SplitStringA(LPCSTR pszStr, LPCSTR pSeperator, std::vector<CStringA> *pRst);
		BOOL SplitStringW(LPCWSTR pszStr, LPCWSTR pSeperator, std::vector<CStringW> *pRst);
	}
}

#ifdef _UNICODE
#define SplitString SplitStringW
#else
#define SplitString SplitStringA
#endif