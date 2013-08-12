#include "stdafx.h"

#include "MACTableEntryImpl.h"

#include "..\..\..\help\net\snmp\NetString.h"

SMC_IMPLEMENT_INTERFACEIMPL(CMACTableEntry)

CMACTableEntry::CMACTableEntry()
	: m_nMACAddr(0),
	m_nPortId(0)

{
}

LPCWSTR CMACTableEntry::GetMACAddrStr()
{
	return m_strMACAddr;
}

UINT64 CMACTableEntry::GetMACAddr()
{
	return m_nMACAddr;
}

BOOL CMACTableEntry::SetMACAddr(LPCWSTR pMACAddrStr)
{
	if (!pMACAddrStr)
		return FALSE;

	m_strMACAddr = pMACAddrStr;
	m_nMACAddr = NetString::MACAddrStrToMACAddr(pMACAddrStr);

	return TRUE;
}

BOOL CMACTableEntry::SetMACAddr(UINT64 nMACAddr)
{
	m_nMACAddr = nMACAddr;
	m_strMACAddr = NetString::MACAddrToMACAddrStr(nMACAddr);

	return TRUE;
}

BOOL CMACTableEntry::SetPortID(UINT32 nPortId)
{
	m_nPortId = nPortId;

	return TRUE;
}

UINT32 CMACTableEntry::GetPortID()
{
	return m_nPortId;
}
