#pragma once

#include "../../../../inc/interface/net/snmp/PortInfoEntry.h"

class CPortInfoEntry : public IPortInfoEntry
{
	SMC_DECLARE_INTERFACEIMPL_BEGIN(CPortInfoEntry, SMC_MUTIINSTANCE)
		SMC_IMPLEMENT_INTERFACE(CPortInfoEntry, IPortInfoEntry)
	SMC_DECLARE_INTERFACEIMPL_END(CPortInfoEntry)

public:
	STDMETHOD_(BOOL, SetPortID)(UINT32 nPortID);

	STDMETHOD_(UINT32, GetPortID)();

	STDMETHOD_(BOOL, SetInterfaceID)(UINT32 nInterfaceID);

	STDMETHOD_(UINT32, GetInterfaceID)();

	STDMETHOD_(BOOL, SetPortDescription)(LPCWSTR pPortDescription);

	STDMETHOD_(LPCWSTR, GetPortDescription)();

public:
	CPortInfoEntry();

private:
	UINT32	m_nPortID;
	UINT32  m_nInterfaceID;
	CStringW m_strPortDescription;
};