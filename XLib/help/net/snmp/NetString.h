namespace NetString
{
	CStringW IPAddrToIPAddrStr(UINT32 nIPAddr);
	UINT32 IPAddrStrToIPAddr(LPCWSTR pIPAddrStr);

	CStringW MACAddrToMACAddrStr(UINT64 nMACAddr);
	UINT64 MACAddrStrToMACAddr(LPCWSTR pMACAddrStr);

}