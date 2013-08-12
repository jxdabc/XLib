#pragma once
#include "..\..\common\XArray.h"

SMC_DECLARE_INTERFACE(ISNMP, {F036D331-8A18-4640-A7C4-566BFBD06E74})
	: public ISMCInterface
{
public:
	STDMETHOD_(BOOL, Init)(LPCWSTR pAddress, LPCWSTR pCommunity, INT nTimeoutMilliSec, INT nRetry) PURE;

	STDMETHOD_(BOOL, Uninit)() PURE;

	STDMETHOD_(LPCWSTR, GetHostName)() PURE;

	STDMETHOD_(LPCWSTR, GetLastError)() PURE;

	// IXArray<IPortInfoEntry>
	STDMETHOD_(BOOL, QueryPortInfo)(IXArray **pPortInfo, BOOL bUseInterfaceIDAsPortID = FALSE) PURE;

	// IXArray<IARPTableEntry>
	STDMETHOD_(BOOL, QueryArpTable)(IXArray **pArpTable) PURE;

	// IXArray<IMACTableEntry>
	STDMETHOD_(BOOL, QueryMACTable)(IXArray **pMACTable) PURE;

};