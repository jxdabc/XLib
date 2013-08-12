#include "stdafx.h"

#include "PortInfoEntryImpl.h"

SMC_IMPLEMENT_INTERFACEIMPL(CPortInfoEntry)

CPortInfoEntry::CPortInfoEntry()
	:m_nPortID(0),
	m_nInterfaceID(0)
{

}

BOOL CPortInfoEntry::SetPortID(UINT32 nPortID)
{
	m_nPortID = nPortID;

	return TRUE;
}

UINT32 CPortInfoEntry::GetPortID()
{
	return m_nPortID;
}


BOOL CPortInfoEntry::SetInterfaceID(UINT32 nInterfaceID)
{
	m_nInterfaceID = nInterfaceID;

	return TRUE;
}

UINT32 CPortInfoEntry::GetInterfaceID()
{
	return m_nInterfaceID;
}


BOOL CPortInfoEntry::SetPortDescription(LPCWSTR pPortDescription)
{
	if (!pPortDescription)
		return FALSE;

	m_strPortDescription = pPortDescription;
	return TRUE;
}

LPCWSTR CPortInfoEntry::GetPortDescription()
{
	return m_strPortDescription;
}