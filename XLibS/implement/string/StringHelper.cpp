#include "../../libcommon.h"
#include "../../../inc/interfaceS/string/StringHelper.h"

BOOL XLibS::StringHelper::SplitStringA( LPCSTR pszStr, LPCSTR pSeperator, std::vector<CStringA> *pRst )
{
	if (!pszStr || !pSeperator || !pRst)
		return FALSE;

	pRst->clear();

	LPCSTR pSegement = pszStr;

	while (TRUE)
	{
		LPCSTR pSegementEnd = StrStrA(pSegement, pSeperator);
		if (pSegementEnd)
			pRst->push_back(CStringA(pSegement, pSegementEnd - pSegement));
		else
		{
			pRst->push_back(pSegement);
			break;
		}

		pSegement = pSegementEnd + strlen(pSeperator);
	};

	return TRUE;
}

BOOL XLibS::StringHelper::SplitStringW( LPCWSTR pszStr, LPCWSTR pSeperator, std::vector<CStringW> *pRst )
{
	if (!pszStr || !pSeperator || !pRst)
		return FALSE;

	pRst->clear();

	LPCWSTR pSegement = pszStr;

	while (TRUE)
	{
		LPCWSTR pSegementEnd = StrStrW(pSegement, pSeperator);
		if (pSegementEnd)
			pRst->push_back(CStringW(pSegement, pSegementEnd - pSegement));
		else
		{
			pRst->push_back(pSegement);
			break;
		}

		pSegement = pSegementEnd + wcslen(pSeperator);
	};

	return TRUE;
}