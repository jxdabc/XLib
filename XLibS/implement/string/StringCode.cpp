#include "../../libcommon.h"
#include "../../../inc/interfaceS/string/StringCode.h"

CStringW XLibS::StringCode::ConvertUTF8toUnicode( LPCSTR pszStr )
{
	if (!pszStr)
		return _T("");

	int wcslen = ::MultiByteToWideChar(CP_UTF8, NULL, pszStr, strlen(pszStr), NULL, 0);
	WCHAR *wp = new WCHAR[wcslen + 1];

	::MultiByteToWideChar(CP_UTF8, NULL, pszStr, strlen(pszStr), wp, wcslen);
	wp[wcslen] = '\0';

	CStringW s = wp ;
	delete[] wp ;
	return s ;
}

CStringA XLibS::StringCode::ConvertUnicodeToUTF8( LPCWSTR pszStr )
{
	if (!pszStr)
		return "";

	int len = _tcslen(pszStr);
	CStringA sr;
	UINT uLimit = 0x7fffffff;
	if (uLimit <= 0 || len == 0)
	{
		return sr;
	}
	UINT utf8len = 0;
	for (int i=0; i<len; ++i)
	{
		int marki = i;
		unsigned short us = pszStr[i];
		if (us == 0)
		{
			utf8len += 2;
		}
		else if (us <= 0x7f)
		{
			utf8len += 1;
		}
		else if (us <= 0x7ff)
		{
			utf8len += 2;
		}
		else if (0xd800 <= us && us <= 0xdbff && i+1<len)
		{
			unsigned short ul = pszStr[i+1];
			if (0xdc00 <= ul && ul <= 0xdfff)
			{
				++i;
				utf8len += 4;
			}
			else
			{
				utf8len += 3;
			}
		}
		else
		{
			utf8len += 3;
		}
		if (utf8len > uLimit)
		{
			len = marki;
			break;
		}
	}

	PBYTE pc = (PBYTE)sr.GetBuffer(utf8len+1);
	for (int i=0; i<len; ++i)
	{
		unsigned short us = pszStr[i];
		if (us == 0)
		{
			*pc ++ = 0xc0;
			*pc ++ = 0x80;
		}
		else if (us <= 0x7f)
		{
			*pc ++ = (char)us;
		}
		else if (us <= 0x7ff)
		{
			*pc ++ = 0xc0 | (us >> 6);
			*pc ++ = 0x80 | (us & 0x3f);
		}
		else if(0xd800 <= us && us <= 0xdbff && i+1<len)
		{
			unsigned short ul = pszStr[i+1];
			if (0xdc00 <= ul && ul <= 0xdfff)
			{
				++i;
				UINT bc = (us-0xD800)*0x400 + (ul-0xDC00) + 0x10000;
				*pc ++ = (BYTE)(0xf0 | ((bc >> 18) & 0x07));
				*pc ++ = (BYTE)(0x80 | ((bc >> 12) & 0x3f));
				*pc ++ = (BYTE)(0x80 | ((bc >>  6) & 0x3f));
				*pc ++ = (BYTE)(0x80 | ((bc      ) & 0x3f));
			}
			else
			{
				*pc ++ = (BYTE) (0xe0 | ((us >> 12) & 0x0f));
				*pc ++ = (BYTE) (0x80 | ((us >>  6) & 0x3f));
				*pc ++ = (BYTE) (0x80 | ((us      ) & 0x3f));
			}
		}
		else
		{
			*pc ++ = (BYTE) (0xe0 | ((us >> 12) & 0x0f));
			*pc ++ = (BYTE) (0x80 | ((us >>  6) & 0x3f));
			*pc ++ = (BYTE) (0x80 | ((us      ) & 0x3f));
		}
	}
	* pc++ = 0;
	sr.ReleaseBuffer();
	return sr;
}

CStringW XLibS::StringCode::ConvertAnsiStrToWideStr( LPCSTR pszStr )
{
	if (!pszStr)
		return L"";

	LCID lcid = GetThreadLocale();
	SetThreadLocale(MAKELCID(MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_SIMPLIFIED), SORT_CHINESE_PRC));

	USES_CONVERSION;
	CStringW strRet = A2W(pszStr);

	SetThreadLocale(lcid);

	return strRet;
}


CStringA XLibS::StringCode::ConvertWideStrToAnsiStr( LPCWSTR pszStr )
{
	if (!pszStr)
		return "";

	LCID lcid = GetThreadLocale();
	SetThreadLocale(MAKELCID(MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_SIMPLIFIED), SORT_CHINESE_PRC));

	USES_CONVERSION;
	CStringA strRet = W2A(pszStr);

	SetThreadLocale(lcid);

	return strRet;
}


