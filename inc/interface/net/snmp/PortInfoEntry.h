#pragma once

SMC_DECLARE_INTERFACE(IPortInfoEntry, {EA6D34CF-9077-4fb4-968D-42727BA9DB83})
	: public ISMCInterface
{
public:
	STDMETHOD_(BOOL, SetPortID)(UINT32 nPortID) PURE;

	STDMETHOD_(UINT32, GetPortID)() PURE;

	STDMETHOD_(BOOL, SetInterfaceID)(UINT32 nInterfaceID) PURE;

	STDMETHOD_(UINT32, GetInterfaceID)() PURE;

	STDMETHOD_(BOOL, SetPortDescription)(LPCWSTR pPortDescription) PURE;

	STDMETHOD_(LPCWSTR, GetPortDescription)() PURE;
};