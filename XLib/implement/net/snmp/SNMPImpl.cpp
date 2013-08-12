#include "stdafx.h"

#include "SNMPImpl.h" 

#include "..\..\..\..\inc\interfaceS\string\StringCode.h"
#include "..\..\..\..\inc\interface\net\snmp\ARPTableEntry.h"
#include "..\..\..\..\inc\interface\net\snmp\MACTableEntry.h"
#include "..\..\..\..\inc\interface\net\snmp\PortInfoEntry.h"

#pragma comment(lib, "Mgmtapi.lib")
#pragma comment(lib, "Snmpapi.lib")


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
///								Helpers

static VOID ClearQueryTableResult(std::vector<std::pair<CString, std::vector<AsnAny>>> & result)
{
	for (std::vector<std::pair<CString, std::vector<AsnAny>>>::size_type i = 0; 
		i < result.size(); i++)
	{
		for (std::vector<AsnAny>::size_type j = 0; j < result[i].second.size(); j++ )
		{
			SnmpUtilAsnAnyFree(&result[i].second[j]);
		}
	}
}

static CStringA ThreadSafeSnmpUtilOidToA(AsnObjectIdentifier *Oid)
{
	if (!Oid)
		return "";
	else
	{
		CStringA strResult;

		for (UINT i = 0; i < Oid->idLength; i++)
			strResult.AppendFormat(".%u", Oid->ids[i]);

		return strResult;
	}
}


//////////////////////////////////////////////////////////////////////////

SMC_IMPLEMENT_INTERFACEIMPL(CSNMP)

CSNMP::CSNMP()
	:m_SnmpSession(NULL)
{

}

CSNMP::~CSNMP()
{
	Uninit();
}

BOOL CSNMP::Init(LPCWSTR pAddress, LPCWSTR pCommunity, INT nTimeoutMilliSec, INT nRetry)
{
	if (!pAddress)
		return FALSE;

	if (!pCommunity)
		return FALSE;

	CStringA strAddress = XLibS::StringCode::ConvertWideStrToAnsiStr(pAddress);
	CStringA strCommunity = XLibS::StringCode::ConvertWideStrToAnsiStr(pCommunity);

	m_SnmpSession = 
		::SnmpMgrOpen(strAddress.GetBuffer(), strCommunity.GetBuffer(), nTimeoutMilliSec, nRetry);
	strAddress.ReleaseBuffer();
	strCommunity.ReleaseBuffer();

	return m_SnmpSession != NULL;
}

BOOL CSNMP::Uninit()
{
	if (m_SnmpSession)
		::SnmpMgrClose(m_SnmpSession);

	return FALSE;
}

// IXArray<IPortInfoEntry>
BOOL CSNMP::QueryPortInfo(IXArray **pPortInfo, BOOL bUseInterfaceIDAsPortID)
{
	if (!pPortInfo)
	{
		m_strLastError = L"[QueryPortInfo] : Verify the arguments failed. ";
		return FALSE;
	}

	std::map<UINT, CString>	/* interface ID -> description */ mapInterfaceDescription;
	std::vector<std::pair<UINT, UINT>> /* port ID -> interface ID */ vPortToInterfaceList;

	if (!GetInterfaceDescription(&mapInterfaceDescription))
	{
		m_strLastError = L"Get the interface descriptions failed. ( " + m_strLastError + L" )";
		return FALSE;
	}

	if (!bUseInterfaceIDAsPortID)
		if (!GetPortToInterfaceList(&vPortToInterfaceList))
		{
			m_strLastError = L"Get the port-to-interface list failed. ( " + m_strLastError + L" )";
			return FALSE;
		}

	IXArray *pArray = NULL;

	BOOL bRes = FALSE;
	do 
	{
		if (!SMCCreateLocalInterfaceInstance(&pArray))
		{
			m_strLastError = L"[QueryPortInfo] : Create IXArray instance failed. ";
			break;
		}

		if (bUseInterfaceIDAsPortID)
		{
			std::map<UINT, CString>::const_iterator it =
				mapInterfaceDescription.begin();
			for (; it != mapInterfaceDescription.end(); it++)
			{
				IPortInfoEntry *pEntry = NULL;
				if (!SMCCreateLocalInterfaceInstance(&pEntry))
				{
					m_strLastError = L"[QueryPortInfo] : Create IPortInfoEntry instance failed. ";
					break;
				}

				pEntry->SetPortID(it->first);
				pEntry->SetInterfaceID(it->first);
				pEntry->SetPortDescription(it->second);

				pArray->push_back(pEntry);

				pEntry->Release();
			}
			if (it != mapInterfaceDescription.end())
				break;

		}
		else
		{
			std::vector<std::pair<UINT, UINT>>::size_type i = 0;
			for ( ; i < vPortToInterfaceList.size(); i++)
			{
				std::map<UINT, CString>::const_iterator itFind =
					mapInterfaceDescription.find(vPortToInterfaceList[i].second);

				if (itFind == mapInterfaceDescription.end())
				{
					assert(NULL);
					continue;
				}

				IPortInfoEntry *pEntry = NULL;
				if (!SMCCreateLocalInterfaceInstance(&pEntry))
				{
					m_strLastError = L"[QueryPortInfo] : Create IPortInfoEntry instance failed. ";
					break;
				}

				pEntry->SetPortID(vPortToInterfaceList[i].first);
				pEntry->SetInterfaceID(itFind->first);
				pEntry->SetPortDescription(itFind->second);

				pArray->push_back(pEntry);

				pEntry->Release();
			}
			if (i < vPortToInterfaceList.size())
				break;
		}

		pArray->AddRef();
		*pPortInfo = pArray;

		bRes = TRUE;
	} while (FALSE);


	if (pArray)
		pArray->Release();

	return bRes;
}



// IXArray<IARPTableEntry>
BOOL CSNMP::QueryArpTable(IXArray **pArpTable)
{
	if (!pArpTable)
	{
		m_strLastError = L"[QueryArpTable] : Verify the arguments failed. ";
		return FALSE;
	}

	std::vector<std::pair<CString, std::vector<AsnAny>>> rows;
	IXArray *pArray = NULL;

	BOOL bRes = FALSE;
	do 
	{
		LPCTSTR apColumnOidList[] = {_T(".1.3.6.1.2.1.4.22.1.2"),		// MAC
			_T(".1.3.6.1.2.1.4.22.1.3")};								// IP

		if (!SnmpQueryTable(apColumnOidList, ARRAYSIZE(apColumnOidList), &rows))
			break;

		if (!SMCCreateLocalInterfaceInstance(&pArray))
		{
			m_strLastError = L"[QueryArpTable] : Create IXArray instance failed. ";
			break;
		}

		std::vector<std::pair<CString, std::vector<AsnAny>>>::size_type i = 0;
		for (; i < rows.size(); i++)
		{
			if (rows[i].second.size() != 2)
			{
				assert(NULL);
				continue;
			}

			if (rows[i].second[0].asnType != ASN_OCTETSTRING || rows[i].second[0].asnValue.string.length != 6)
			{
				assert(NULL);
				continue;
			}
			if (rows[i].second[1].asnType != ASN_IPADDRESS || rows[i].second[0].asnValue.address.length != 6)
			{
				assert(NULL);
				continue;
			}

			IARPTableEntry *pEntry = NULL;
			if (!SMCCreateLocalInterfaceInstance(&pEntry))
			{
				m_strLastError = L"[QueryArpTable] : Create IARPTableEntry instance failed. ";
				break;
			}

			UINT64 mac = 0;
			((BYTE *)&mac)[0] = rows[i].second[0].asnValue.string.stream[5];
			((BYTE *)&mac)[1] = rows[i].second[0].asnValue.string.stream[4];
			((BYTE *)&mac)[2] = rows[i].second[0].asnValue.string.stream[3];
			((BYTE *)&mac)[3] = rows[i].second[0].asnValue.string.stream[2];
			((BYTE *)&mac)[4] = rows[i].second[0].asnValue.string.stream[1];
			((BYTE *)&mac)[5] = rows[i].second[0].asnValue.string.stream[0];

			UINT32 ip = 0;
			((BYTE *)&ip)[0] = rows[i].second[1].asnValue.address.stream[3];
			((BYTE *)&ip)[1] = rows[i].second[1].asnValue.address.stream[2];
			((BYTE *)&ip)[2] = rows[i].second[1].asnValue.address.stream[1];
			((BYTE *)&ip)[3] = rows[i].second[1].asnValue.address.stream[0];

			pEntry->SetMACAddr(mac);
			pEntry->SetIPAddr(ip);

			pArray->push_back(pEntry);

			pEntry->Release();
		}
		if (i < rows.size())
			break;

		pArray->AddRef();
		*pArpTable = pArray;

		bRes = TRUE;

	} while (FALSE);


	if (pArray)
		pArray->Release();

	ClearQueryTableResult(rows);
	rows.clear();

	return bRes;
}

// IXArray<IMACTableEntry>
BOOL CSNMP::QueryMACTable(IXArray **pMACTable)
{
	if (!pMACTable)
	{
		m_strLastError = L"[QueryMACTable] : Verify the arguments failed. ";
		return FALSE;
	}

	const AsnInteger32 MAC_TYPE_LEARNED = 3;

	std::vector<std::pair<CString, std::vector<AsnAny>>> rows;
	IXArray *pArray = NULL;

	BOOL bRes = FALSE;
	do 
	{
		LPCTSTR apColumnOidList[] = {_T(".1.3.6.1.2.1.17.4.3.1.1"),		// MAC
			_T(".1.3.6.1.2.1.17.4.3.1.2"),								// PORT
			_T(".1.3.6.1.2.1.17.4.3.1.3")};								// TYPE

		if (!SnmpQueryTable(apColumnOidList, ARRAYSIZE(apColumnOidList), &rows))
			break;

		if (!SMCCreateLocalInterfaceInstance(&pArray))
		{
			m_strLastError = L"[QueryMACTable] : Create IXArray instance failed. ";
			break;
		}

		std::vector<std::pair<CString, std::vector<AsnAny>>>::size_type i = 0;
		for (; i < rows.size(); i++)
		{
			if (rows[i].second.size() != 3)
			{
				assert(NULL);
				continue;
			}

			if (rows[i].second[0].asnType != ASN_OCTETSTRING || rows[i].second[0].asnValue.string.length != 6)
			{
				assert(NULL);
				continue;
			}
			if (rows[i].second[1].asnType != ASN_INTEGER)
			{
				assert(NULL);
				continue;
			}
			if (rows[i].second[2].asnType != ASN_INTEGER)
			{
				assert(NULL);
				continue;
			}

			// Not dynamically learned. 
			if (rows[i].second[2].asnValue.number != MAC_TYPE_LEARNED)
				continue;

			IMACTableEntry *pEntry = NULL;
			if (!SMCCreateLocalInterfaceInstance(&pEntry))
			{
				m_strLastError = L"[QueryMACTable] : Create IMACTableEntry instance failed. ";
				break;
			}

			UINT64 mac = 0;
			((BYTE *)&mac)[0] = rows[i].second[0].asnValue.string.stream[5];
			((BYTE *)&mac)[1] = rows[i].second[0].asnValue.string.stream[4];
			((BYTE *)&mac)[2] = rows[i].second[0].asnValue.string.stream[3];
			((BYTE *)&mac)[3] = rows[i].second[0].asnValue.string.stream[2];
			((BYTE *)&mac)[4] = rows[i].second[0].asnValue.string.stream[1];
			((BYTE *)&mac)[5] = rows[i].second[0].asnValue.string.stream[0];

			pEntry->SetMACAddr(mac);
			pEntry->SetPortID(rows[i].second[1].asnValue.number);

			pArray->push_back(pEntry);

			pEntry->Release();
		}
		if (i < rows.size())
			break;

		pArray->AddRef();
		*pMACTable = pArray;

		bRes = TRUE;

	} while (FALSE);


	if (pArray)
		pArray->Release();

	ClearQueryTableResult(rows);
	rows.clear();

	return bRes;
}

BOOL CSNMP::SnmpQueryTable( LPCTSTR apColumnOidList[], UINT nListLen, std::vector<std::pair<CString, std::vector<AsnAny>>> *pTable )
{
	if (!apColumnOidList || !pTable)
	{
		m_strLastError = L"[SnmpQueryTable] : Verify the arguments failed. ";
		return FALSE;
	}

	pTable->clear();

	if (!nListLen)
		return TRUE;

	typedef std::map<CString, AsnAny> RELATIVE_OID_TO_VALUE_MAP;

	std::vector<RELATIVE_OID_TO_VALUE_MAP> vColumnRelativeOidToValue(nListLen);

	SnmpVarBindList CurrentVarBindList;
	CurrentVarBindList.len = 0;
	CurrentVarBindList.list = NULL;

	AsnObjectIdentifier TargetObjectIdentifier;
	TargetObjectIdentifier.idLength = 0;
	TargetObjectIdentifier.ids = NULL;

	BOOL bRes = FALSE;
	do 
	{
		UINT i = 0;
		for (; i < nListLen; i++)
		{
			SnmpUtilVarBindListFree(&CurrentVarBindList);

			CurrentVarBindList.len = 1;
			CurrentVarBindList.list = (SnmpVarBind *)SnmpUtilMemAlloc(sizeof(SnmpVarBind));
			CurrentVarBindList.list->name.idLength = 0;
			CurrentVarBindList.list->name.ids = NULL;
			CurrentVarBindList.list->value.asnType = ASN_NULL;

#ifdef _UNICODE
			CStringA strOIDA = XLibS::StringCode::ConvertWideStrToAnsiStr(apColumnOidList[i]); 
#else
			CStringA strOIDA = apColumnOidList[i];
#endif

			if(!::SnmpMgrStrToOid(strOIDA.GetBuffer(), &CurrentVarBindList.list->name) || !CurrentVarBindList.list->name.ids || !CurrentVarBindList.list->name.idLength)
			{

#ifdef _UNICODE
				CStringW strOIDW = apColumnOidList[i];
#else
				CStringW strOIDW = XLibS::StringCode::ConvertAnsiStrToWideStr(apColumnOidList[i]);
#endif
				m_strLastError.Format(L"[SnmpQueryTable] : Bad OID string \"%s\". ", (LPCWSTR)strOIDW);

				strOIDA.ReleaseBuffer();

				break;
			}
			strOIDA.ReleaseBuffer();

			SnmpUtilOidFree(&TargetObjectIdentifier);
			TargetObjectIdentifier.idLength = 0;
			TargetObjectIdentifier.ids = NULL;

			if (!SnmpUtilOidCpy(&TargetObjectIdentifier, &CurrentVarBindList.list->name) || !TargetObjectIdentifier.ids || !TargetObjectIdentifier.idLength)
			{
				m_strLastError.Format(L"[SnmpQueryTable] : Copy OID failed. ");
				break;
			}

			BOOL bFailFlag = FALSE;
			while(1)
			{
				AsnInteger nSNMPErrorCode = 0;
				AsnInteger nSNMPErrorIndex = 0;
				if(!::SnmpMgrRequest(m_SnmpSession, SNMP_PDU_GETNEXT, &CurrentVarBindList, &nSNMPErrorCode, &nSNMPErrorIndex))
				{
					DWORD dwLastWinError = ::GetLastError();
					assert(nSNMPErrorIndex == 0);

					const WCHAR * SNMP_ERROR_MESSAGE[] = { L"The agent reports that no errors occurred during transmission. ", 
						L"The agent could not place the results of the requested operation into a single SNMP message. ", 
						L"The requested operation identified an unknown variable. ", 
						L"The requested operation tried to change a variable but it specified either a syntax or value error. ", 
						L"The requested operation tried to change a variable that was not allowed to change according to the community profile of the variable. "
					};

					const WCHAR * WIN_ERROR_MESSAGE[] = {L"The request timed-out.",
						L"Unexpected error file descriptors indicated by the Windows Sockets select function."
					};

					const WCHAR *pSNMPError = NULL;
					if (nSNMPErrorIndex < ARRAYSIZE(SNMP_ERROR_MESSAGE))
						pSNMPError = SNMP_ERROR_MESSAGE[nSNMPErrorIndex];
					else
						pSNMPError = L"Unknown error. ";

					const WCHAR *pWinError = NULL;
					if (dwLastWinError - 40 < ARRAYSIZE(WIN_ERROR_MESSAGE))
						pWinError = WIN_ERROR_MESSAGE[dwLastWinError - 40];
					else
						pWinError = L"Unknown error. ";

					m_strLastError.Format(L"SNMP requesting failed. SNMP ERROR : %s (%ld). WIN ERROR : %s (%lu). ", 
						pSNMPError, nSNMPErrorIndex, pWinError, dwLastWinError);
	
//					LPCWSTR pSysError = NULL;
//					m_strLastError.Format(L"SNMP requesting failed. SNMP ERROR : %s (%ld). ", pError, nErrorIndex);
//					if (::FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, 
//						NULL, dwLastError, 0, (LPTSTR)&pSysError, 0, NULL) && pSysError)
//					{
//						m_strLastError.AppendFormat(L"Windows ERROR: %s (%lu). ", pSysError, dwLastError);
//
//						::LocalFree((HLOCAL)pSysError);
//						pSysError = NULL;
//					}

					
					bFailFlag = TRUE;
					break;
				}

				assert(SNMP_ERRORSTATUS_NOERROR == nSNMPErrorCode);
				
				if (!CurrentVarBindList.list || (!CurrentVarBindList.list->name.ids && CurrentVarBindList.list->name.idLength))
				{
#ifdef _UNICODE
					CStringW strOIDW = apColumnOidList[i];
#else
					CStringW strOIDW = XLibS::StringCode::ConvertAnsiStrToWideStr(apColumnOidList[i]);
#endif
					m_strLastError.Format(L"[SnmpQueryTable] : GetNext returned a bad OID during the request for %s. ", (LPCWSTR)strOIDW);

					bFailFlag = TRUE;
					break;
				}

				if (CurrentVarBindList.list->name.idLength < TargetObjectIdentifier.idLength)
					break;

				if (SnmpUtilOidNCmp(&CurrentVarBindList.list->name, &TargetObjectIdentifier, TargetObjectIdentifier.idLength))
					break;

				CStringA strOIDTarget = ThreadSafeSnmpUtilOidToA(&TargetObjectIdentifier);
				CStringA strOIDCurrent = ThreadSafeSnmpUtilOidToA(&CurrentVarBindList.list->name);

				assert(strOIDCurrent.Find(strOIDTarget) == 0);



				AsnAny value;
				SnmpUtilAsnAnyCpy(&value, &CurrentVarBindList.list->value);

#ifdef _UNICODE
				CString strRelativeOID = 
					XLibS::StringCode::ConvertAnsiStrToWideStr(strOIDCurrent.Mid(strOIDTarget.GetLength()));
#else
				CString strRelativeOID = strOIDCurrent.Mid(strOIDTarget.GetLength());
#endif

				vColumnRelativeOidToValue[i].insert(std::make_pair(strRelativeOID, value));
			}
			if (bFailFlag)
				break;

		}

		if (i < nListLen)
			break;

		const RELATIVE_OID_TO_VALUE_MAP & FirstCol = vColumnRelativeOidToValue[0];
		for (RELATIVE_OID_TO_VALUE_MAP::const_iterator itFirstCol = FirstCol.begin();
			itFirstCol != FirstCol.end(); itFirstCol++)
		{
			std::vector<RELATIVE_OID_TO_VALUE_MAP::const_iterator> vItFind;

			UINT i = 1;
			for (; i < nListLen; i++)
			{
				const RELATIVE_OID_TO_VALUE_MAP & CurrentCol = 
					vColumnRelativeOidToValue[i];

				RELATIVE_OID_TO_VALUE_MAP::const_iterator itFind =
					 CurrentCol.find(itFirstCol->first);

				if (itFind == CurrentCol.end())
					break;

				vItFind.push_back(itFind);
			}

			if (i < nListLen)
				continue;

			pTable->push_back(make_pair(itFirstCol->first, std::vector<AsnAny>(1, itFirstCol->second)));

			for (std::vector<RELATIVE_OID_TO_VALUE_MAP::const_iterator>::size_type i = 0;
				i < vItFind.size(); i++)
			{
				pTable->back().second.push_back(vItFind[i]->second);
			}
		}

		bRes = TRUE;

	} while (FALSE);

	SnmpUtilOidFree(&TargetObjectIdentifier);
	SnmpUtilVarBindListFree(&CurrentVarBindList);

	if (!bRes)
	{
		ClearQueryTableResult(*pTable);
		pTable->clear();
	}

	return bRes;
}

LPCWSTR CSNMP::GetLastError()
{
	return m_strLastError;
}

LPCWSTR CSNMP::GetHostName()
{
	m_strLastError.Empty();

	if (!m_strHostName.IsEmpty())
		return m_strHostName;

	std::vector<std::pair<CString, std::vector<AsnAny>>> rows;
	do 
	{
		LPCTSTR apColumnOidList[] = {_T(".1.3.6.1.2.1.1.5"),			// System name
			};

		if (!SnmpQueryTable(apColumnOidList, ARRAYSIZE(apColumnOidList), &rows))
			break;

		if (!rows.size())
		{
			m_strLastError = L"No host name returned. ";
			break;
		}

		if (!rows[0].second.size() || rows[0].second[0].asnType != ASN_OCTETSTRING)
		{
			m_strLastError = L"[GetHostName] : SnmpQueryTable returned a bad result. ";
			break;
		}

		if (rows[0].second[0].asnValue.string.length)
		{
			CStringA strHostName((CHAR *)rows[0].second[0].asnValue.string.stream, rows[0].second[0].asnValue.string.length);
			m_strHostName = XLibS::StringCode::ConvertAnsiStrToWideStr(strHostName);
		}

	} while (FALSE);


	ClearQueryTableResult(rows);
	rows.clear();

	return m_strHostName;
}

BOOL CSNMP::GetPortToInterfaceList( std::vector<std::pair<UINT32, UINT32>>/* port ID -> interface ID*/ *pList )
{
	if (!pList)
		return FALSE;

	std::vector<std::pair<CString, std::vector<AsnAny>>> rows;

	BOOL bRes = FALSE;
	do 
	{
		LPCTSTR apColumnOidList[] = {_T(".1.3.6.1.2.1.17.1.4.1.1"),		// Port ID
			_T(".1.3.6.1.2.1.17.1.4.1.2")};								// Interface ID

		if (!SnmpQueryTable(apColumnOidList, ARRAYSIZE(apColumnOidList), &rows))
			break;

		std::vector<std::pair<CString, std::vector<AsnAny>>>::size_type i = 0;
		for (; i < rows.size(); i++)
		{
			if (rows[i].second.size() != 2)
			{
				assert(NULL);
				continue;
			}

			if (rows[i].second[0].asnType != ASN_INTEGER)
			{
				assert(NULL);
				continue;
			}
			if (rows[i].second[1].asnType != ASN_INTEGER)
			{
				assert(NULL);
				continue;
			}

			pList->push_back(std::make_pair(rows[i].second[0].asnValue.number, rows[i].second[1].asnValue.number));

		}
		if (i < rows.size())
			break;

		bRes = TRUE;

	} while (FALSE);

	ClearQueryTableResult(rows);
	rows.clear();

	if (!bRes)
		pList->clear();

	return bRes;
}

BOOL CSNMP::GetInterfaceDescription( std::map<UINT32, CString>/* interface ID -> description */ *pMap )
{
	if (!pMap)
		return FALSE;


	std::vector<std::pair<CString, std::vector<AsnAny>>> rows;

	BOOL bRes = FALSE;
	do 
	{
		LPCTSTR apColumnOidList[] = {_T(".1.3.6.1.2.1.2.2.1.1"),		// interface ID
			_T(".1.3.6.1.2.1.2.2.1.2")};								// description

		if (!SnmpQueryTable(apColumnOidList, ARRAYSIZE(apColumnOidList), &rows))
			break;

		std::vector<std::pair<CString, std::vector<AsnAny>>>::size_type i = 0;
		for (; i < rows.size(); i++)
		{
			if (rows[i].second.size() != 2)
			{
				assert(NULL);
				break;
			}

			if (rows[i].second[0].asnType != ASN_INTEGER)
			{
				assert(NULL);
				break;
			}
			if (rows[i].second[1].asnType != ASN_OCTETSTRING)
			{
				assert(NULL);
				break;
			}

			if (rows[i].second[1].asnValue.address.length)
			{
				CStringA strDesciption((CHAR *)rows[i].second[1].asnValue.string.stream, rows[i].second[1].asnValue.string.length);

#ifdef _UNICODE
				pMap->insert(std::make_pair(rows[i].second[0].asnValue.number, XLibS::StringCode::ConvertAnsiStrToWideStr(strDesciption)));
#else
				pMap->insert(std::make_pair(rows[i].second[0].asnValue.number, strDesciption));
#endif
			}
			else
				pMap->insert(std::make_pair(rows[i].second[0].asnValue.number, CString()));

		}
		if (i < rows.size())
			break;

		bRes = TRUE;

	} while (FALSE);



	ClearQueryTableResult(rows);
	rows.clear();

	if (!bRes)
		pMap->clear();

	return bRes;
}
