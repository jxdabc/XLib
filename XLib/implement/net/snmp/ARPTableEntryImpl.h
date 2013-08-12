#pragma once

#include "../../../../inc/interface/net/snmp/ARPTableEntry.h"

class CARPTableEntry : public IARPTableEntry
{
	SMC_DECLARE_INTERFACEIMPL_BEGIN(CARPTableEntry, SMC_MUTIINSTANCE)
		SMC_IMPLEMENT_INTERFACE(CARPTableEntry, IARPTableEntry)
	SMC_DECLARE_INTERFACEIMPL_END(CARPTableEntry)

public:
	STDMETHOD_(LPCWSTR, GetIPAddrStr)();
	STDMETHOD_(UINT32, GetIPAddr)();

	STDMETHOD_(LPCWSTR, GetMACAddrStr)();
	STDMETHOD_(UINT64, GetMACAddr)();

	STDMETHOD_(BOOL, SetIPAddr)(LPCWSTR pIPAddrStr);
	STDMETHOD_(BOOL, SetIPAddr)(UINT32 nIPAddr);

	STDMETHOD_(BOOL, SetMACAddr)(LPCWSTR pMACAddrStr);
	STDMETHOD_(BOOL, SetMACAddr)(UINT64 nMACAddr);

public:
	CARPTableEntry();

private:
	CStringW m_strIPAddr;
	UINT32 m_nIPAddr;

	CStringW m_strMACAddr;
	UINT64 m_nMACAddr;
};