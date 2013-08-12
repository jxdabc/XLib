#include "stdafx.h"

#include "NetString.h"

CStringW NetString::IPAddrToIPAddrStr( UINT32 nIPAddr )
{
	UINT D = nIPAddr % 0x100; nIPAddr /= 0x100;
	UINT C = nIPAddr % 0x100; nIPAddr /= 0x100;
	UINT B = nIPAddr % 0x100; nIPAddr /= 0x100;
	UINT A = nIPAddr % 0x100; nIPAddr /= 0x100;

	if (nIPAddr)
	{
		assert(!_T("Value too large. "));
		return L"";
	}

	CStringW strRes;
	strRes.Format(L"%u.%u.%u.%u", A, B, C, D);

	return strRes;
}

UINT32 NetString::IPAddrStrToIPAddr( LPCWSTR pIPAddrStr )
{
	if (!pIPAddrStr)
	{
		assert(!_T("NULL string. "));
		return 0;
	}

	UINT32 nRes = 0;

	const UINT MAX_IP_ADDR_STR_LEN = 15;
	
	if (wcslen(pIPAddrStr) > 15)
		return 0;

	WCHAR szIPAddrStr[MAX_IP_ADDR_STR_LEN + 1] = {0};
	wcscpy(szIPAddrStr, pIPAddrStr);

	UINT32 nBase = 0x100 * 0x100 * 0x100;

	WCHAR *segment = wcstok(szIPAddrStr, L".");
	while(segment != NULL)
	{
		if (nBase == 0)
		{
			assert(!_T("Too many dots. "));
			return 0;
		}

		ULONG nSegment = wcstoul(segment, NULL, 10);
		if (nSegment > 255)
		{
			assert(!_T("Segment value too large. "));
			return 0;
		}

		nRes += nBase * nSegment;
		nBase /= 0x100;

		segment = _tcstok(NULL, L".");
	}

	if (nBase != 0)
	{
		assert(!_T("Too few dots. "));
		return 0;
	}

	return nRes;
}

CStringW NetString::MACAddrToMACAddrStr( UINT64 nMACAddr )
{
	UINT F = nMACAddr % 0x100; nMACAddr /= 0x100;
	UINT E = nMACAddr % 0x100; nMACAddr /= 0x100;
	UINT D = nMACAddr % 0x100; nMACAddr /= 0x100;
	UINT C = nMACAddr % 0x100; nMACAddr /= 0x100;
	UINT B = nMACAddr % 0x100; nMACAddr /= 0x100;
	UINT A = nMACAddr % 0x100; nMACAddr /= 0x100;

	if (nMACAddr)
	{
		assert(!_T("Value too large. "));
		return L"";
	}

	CStringW strRes;
	strRes.Format(L"%02X:%02X:%02X:%02X:%02X:%02X", A, B, C, D, E, F);

	return strRes;
}

UINT64 NetString::MACAddrStrToMACAddr( LPCWSTR pMACAddrStr )
{
	if (!pMACAddrStr)
	{
		assert(!_T("NULL string. "));
		return 0;
	}

	const UINT MAC_STR_LEN = 12;
	
	WCHAR szMACStr[MAC_STR_LEN + 1] = {};

	for (UINT i = 0; i < MAC_STR_LEN;)
	{
		if (!*pMACAddrStr)
		{
			assert(!_T("Too short a string. "));
			return 0;
		}

		WCHAR c = *pMACAddrStr;

		if ((c >= L'0' && c <= L'9') ||
			(c >= L'A' && c <= L'F') ||
			(c >= L'a' && c <= L'f'))
		{
			szMACStr[i] = c;
			i++;
		}
		else
		{
			if (i % 2)
			{
				szMACStr[i] = szMACStr[i - 1];
				szMACStr[i - 1] = L'0';
				i++;
			}
		}

		pMACAddrStr++;
	}

	if (*pMACAddrStr)
	{
		assert(!_T("Bad mac string. "));
		return 0;
	}

	wcslwr(szMACStr);
	return _wcstoui64(szMACStr, NULL, 16);
}

