#pragma once

#include "../../../../inc/interface/net/snmp/MACTableEntry.h"

class CMACTableEntry : public IMACTableEntry
{
	SMC_DECLARE_INTERFACEIMPL_BEGIN(CMACTableEntry, SMC_MUTIINSTANCE)
		SMC_IMPLEMENT_INTERFACE(CMACTableEntry, IMACTableEntry)
	SMC_DECLARE_INTERFACEIMPL_END(CMACTableEntry)

public:
	STDMETHOD_(LPCWSTR, GetMACAddrStr)();
	STDMETHOD_(UINT64, GetMACAddr)();

	STDMETHOD_(BOOL, SetMACAddr)(LPCWSTR pMACAddrStr);
	STDMETHOD_(BOOL, SetMACAddr)(UINT64 nMACAddr);

	STDMETHOD_(BOOL, SetPortID)(UINT32 nPortId);

	STDMETHOD_(UINT32, GetPortID)();

public:
	CMACTableEntry();

private:
	CStringW m_strMACAddr;
	UINT64 m_nMACAddr;
	UINT32 m_nPortId;
};