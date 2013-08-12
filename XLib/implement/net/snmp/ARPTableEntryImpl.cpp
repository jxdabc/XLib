#include "stdafx.h"

#include "ARPTableEntryImpl.h"
#include "..\..\..\help\net\snmp\NetString.h"

SMC_IMPLEMENT_INTERFACEIMPL(CARPTableEntry)

CARPTableEntry::CARPTableEntry()
	: m_nIPAddr(0),
	m_nMACAddr(0)
{

}

LPCWSTR CARPTableEntry::GetIPAddrStr()
{
	return m_strIPAddr;
}
UINT32 CARPTableEntry::GetIPAddr()
{
	return m_nIPAddr;
}

LPCWSTR CARPTableEntry::GetMACAddrStr()
{
	return m_strMACAddr;
}

UINT64 CARPTableEntry::GetMACAddr()
{
	return m_nMACAddr;
}

BOOL CARPTableEntry::SetIPAddr(LPCWSTR pIPAddrStr)
{
	if (!pIPAddrStr)
		return FALSE;

	m_strIPAddr = pIPAddrStr;
	m_nIPAddr = NetString::IPAddrStrToIPAddr(pIPAddrStr);
	
	return TRUE;
}

BOOL CARPTableEntry::SetIPAddr(UINT32 nIPAddr)
{
	m_nIPAddr = nIPAddr;
	m_strIPAddr = NetString::IPAddrToIPAddrStr(nIPAddr);
	return TRUE;
}

BOOL CARPTableEntry::SetMACAddr(LPCWSTR pMACAddrStr)
{
	if (!pMACAddrStr)
		return FALSE;

	m_strMACAddr = pMACAddrStr;
	m_nMACAddr = NetString::MACAddrStrToMACAddr(pMACAddrStr);

	return TRUE;
}

BOOL CARPTableEntry::SetMACAddr(UINT64 nMACAddr)
{
	m_nMACAddr = nMACAddr;
	m_strMACAddr = NetString::MACAddrToMACAddrStr(nMACAddr);

	return TRUE;
}