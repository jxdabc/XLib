#pragma once

#include "../../../../inc/interface/net/snmp/SNMP.h"

#include <mgmtapi.h>
#include <vector>

class CSNMP : public ISNMP
{
	SMC_DECLARE_INTERFACEIMPL_BEGIN(CSNMP, SMC_MUTIINSTANCE)
		SMC_IMPLEMENT_INTERFACE(CSNMP, ISNMP)
	SMC_DECLARE_INTERFACEIMPL_END(CSNMP)

public:
	STDMETHOD_(BOOL, Init)(LPCWSTR pAddress, LPCWSTR pCommunity, INT nTimeoutMilliSec, INT nRetry);

	STDMETHOD_(BOOL, Uninit)();

	STDMETHOD_(LPCWSTR, GetHostName)();

	STDMETHOD_(LPCWSTR, GetLastError)();

	// IXArray<IPortInfoEntry>
	STDMETHOD_(BOOL, QueryPortInfo)(IXArray **pPortInfo, BOOL bUseInterfaceIDAsPortID = FALSE);

	// IXArray<IARPTableEntry>
	STDMETHOD_(BOOL, QueryArpTable)(IXArray **pArpTable);

	// IXArray<IMACTableEntry>
	STDMETHOD_(BOOL, QueryMACTable)(IXArray **pMACTable);

public:
	CSNMP();
	~CSNMP();

private:
	BOOL GetPortToInterfaceList( std::vector<std::pair<UINT32, UINT32>>/* port ID -> interface ID */ *pList );
	BOOL GetInterfaceDescription( std::map<UINT32, CString>/* interface ID -> description */ *pMap );

private:
	// Fetch a snmp table.
	// vector<pair<CStringW, vector<AsnAny>>> -> rows< ( relative oid, value ) >
	// It will set m_strLastError properly.  
	BOOL SnmpQueryTable(LPCTSTR apColumnOidList[], UINT nListLen, std::vector<std::pair<CString, std::vector<AsnAny>>> *pTable);

private:
	CStringW m_strHostName;
	CStringW m_strLastError;
	LPSNMP_MGR_SESSION m_SnmpSession;
};