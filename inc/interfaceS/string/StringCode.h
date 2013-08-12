#pragma once

#include <atlstr.h>

namespace XLibS
{
	namespace StringCode
	{
		CStringW ConvertUTF8toUnicode(LPCSTR pszStr);
		CStringA ConvertUnicodeToUTF8(LPCWSTR pszStr);
		CStringW ConvertAnsiStrToWideStr(LPCSTR pszStr);
		CStringA ConvertWideStrToAnsiStr(LPCWSTR pszStr);
	}
}

#ifdef _UNICODE
#define XLibST2A(str) XLibS::StringCode::ConvertWideStrToAnsiStr(str)
#define XLibST2W(str) (str)
#define XLibSA2T(str) XLibS::StringCode::ConvertAnsiStrToWideStr(str)
#define XLibSW2T(str) (str)
#else
#define XLibST2A(str) (str)
#define XLibST2W(str) XLibS::StringCode::ConvertAnsiStrToWideStr(str)
#define XLibSA2T(str) (str)
#define XLibSW2T(str) XLibS::StringCode::ConvertWideStrToAnsiStr(str)
#endif