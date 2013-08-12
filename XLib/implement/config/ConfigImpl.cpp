#include "stdafx.h"

#include "ConfigImpl.h"
#include "..\..\..\inc\interfaceS\string\StringCode.h"
#include <map>
#include <vector>

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
///					Specific Config implements

class IConfigImplement
{
public:
	virtual BOOL Init() = 0;

	virtual BOOL SetParam(LPCTSTR pKey, LPCTSTR pValue) = 0;

	virtual BOOL Load() = 0;

	virtual UINT GetSectionCount() = 0;

	virtual LPCTSTR GetSectionName(UINT nSectionIndex) = 0;

	// return INVALID_SECTION_INDEX if not found. 
	virtual UINT GetSectionIndexByName(LPCTSTR pName) = 0;

	virtual UINT GetSectionKeyCount(UINT nSectionIndex) = 0;

	virtual LPCTSTR GetSectionKey(UINT nSectionIndex, UINT nKeyIndex) = 0;

	virtual BOOL IsSectionKeyExist(UINT nSectionIndex, LPCTSTR pKey) = 0;

	virtual LONG GetIntValue(UINT nSectionIndex, LPCTSTR pKey) = 0;
	virtual DOUBLE GetDoubleValue(UINT nSectionIndex, LPCTSTR pKey) = 0;
	virtual LPCTSTR GetStringValue(UINT nSectionIndex, LPCTSTR pKey) = 0;
};



//////////////////////////////////////////////////////////////////////////
///								INI

#define MAX_TOTAL_SECTIONS_LEN (MAX_PATH * 100)
#define MAX_TOTAL_KEYS_LEN (MAX_PATH * 100)
#define MAX_VALUE_LEN (MAX_PATH * 5)

class CINIConfigImplement :
	public IConfigImplement
{
public:
	virtual BOOL Init();

	virtual BOOL SetParam(LPCTSTR pKey, LPCTSTR pValue);

	virtual BOOL Load();

	virtual UINT GetSectionCount();

	virtual LPCTSTR GetSectionName(UINT nSectionIndex);

	// return INVALID_SECTION_INDEX if not found. 
	virtual UINT GetSectionIndexByName(LPCTSTR pName);

	virtual UINT GetSectionKeyCount(UINT nSectionIndex);

	virtual LPCTSTR GetSectionKey(UINT nSectionIndex, UINT nKeyIndex);

	virtual BOOL IsSectionKeyExist(UINT nSectionIndex, LPCTSTR pKey);

	virtual LONG GetIntValue(UINT nSectionIndex, LPCTSTR pKey);
	virtual DOUBLE GetDoubleValue(UINT nSectionIndex, LPCTSTR pKey);
	virtual LPCTSTR GetStringValue(UINT nSectionIndex, LPCTSTR pKey);

public:
	CINIConfigImplement();
	~CINIConfigImplement();

private:
	CString m_strFileName;
	/* (section , (key -> value)) */
	std::vector<std::pair<CString, std::map<CString, CString>>> m_vConfig;
	BOOL m_bLoaded;

private:
	/* section index -> [keys] */
	std::map<UINT, std::vector<CString>> m_mapGetSectionKeyCache;
	/* section name -> section index */
	std::map<CString, UINT> m_mapGetSectionIndexByNameCache;
};

BOOL CINIConfigImplement::Init()
{
	return TRUE;
}

BOOL CINIConfigImplement::SetParam( LPCTSTR pKey, LPCTSTR pValue )
{
	if (!pKey || !pValue)
		return FALSE;

	if (!_tcsicmp(pKey, _T("filepath")))
	{
		m_strFileName = pValue;
		return TRUE;
	}
	/*else if (!_tcsicmp(pKey, _T("")))*/

	return FALSE;
}

BOOL CINIConfigImplement::Load()
{
	if (m_bLoaded)
		return FALSE;

	m_bLoaded = TRUE;

	if (!PathFileExists(m_strFileName))
		return FALSE;

	TCHAR szSectionNames[MAX_TOTAL_SECTIONS_LEN] = {};
	TCHAR szKeys[MAX_TOTAL_KEYS_LEN] = {};
	TCHAR szValue[MAX_VALUE_LEN] = {};

	DWORD dwSectionsLen = 
		::GetPrivateProfileSectionNames(szSectionNames, MAX_TOTAL_SECTIONS_LEN, m_strFileName);
	if (dwSectionsLen >= MAX_TOTAL_SECTIONS_LEN - 2)
		return FALSE;

	LPCTSTR pSection = szSectionNames;
	while(pSection - szSectionNames + 1 < dwSectionsLen)
	{
		m_vConfig.push_back(std::make_pair(pSection, std::map<CString, CString>()));

		DWORD dwKeysLen = 
			::GetPrivateProfileSection(pSection, szKeys, MAX_TOTAL_KEYS_LEN, m_strFileName);

		if (dwKeysLen >= MAX_TOTAL_KEYS_LEN - 2)
			return FALSE;

		LPCTSTR pKeyValue = szKeys;
		while (pKeyValue - szKeys + 1 < dwKeysLen)
		{
			CString strKey;
			CString strValue;

			LPCWSTR pSeparator = StrStr(pKeyValue, _T("="));
			if (pSeparator == NULL)
			{
				strKey = pKeyValue;
			}
			else
			{
				UINT nKeyLen = pSeparator - pKeyValue;
				LPTSTR pKeyBuffer = strKey.GetBuffer(nKeyLen + 1);
				_tcsncpy(pKeyBuffer, pKeyValue, nKeyLen);
				pKeyBuffer[nKeyLen] = _T('\0');
				strKey.ReleaseBuffer();

				strValue = pSeparator + 1;
			}

			strKey.Trim();
			strValue.Trim();

			std::pair<std::map<CString, CString>::iterator, bool> pairKeyIt = 
				m_vConfig.back().second.insert(std::make_pair(strKey, CString()));

			pairKeyIt.first->second = strValue;

			pKeyValue += _tcslen(pKeyValue) + 1;
		}

		pSection += _tcslen(pSection) + 1;
	}

	return TRUE;
}

CINIConfigImplement::CINIConfigImplement()
	:m_bLoaded(FALSE)
{
}

UINT CINIConfigImplement::GetSectionCount()
{
	return m_vConfig.size();
}

LPCTSTR CINIConfigImplement::GetSectionName( UINT nSectionIndex )
{
	if (nSectionIndex >= GetSectionCount())
		return _T("");

	return m_vConfig[nSectionIndex].first;
}

UINT CINIConfigImplement::GetSectionIndexByName( LPCTSTR pName )
{
	if (!pName)
		return INVALID_SECTION_INDEX;
	
	if (m_mapGetSectionIndexByNameCache.empty())
	{
		for (std::vector<std::pair<CString, std::map<CString, CString>>>::size_type i = 0; 
			i < m_vConfig.size(); i++)
		{
			m_mapGetSectionIndexByNameCache.insert(std::make_pair(m_vConfig[i].first, i));
		}
	}

	std::map<CString, UINT>::const_iterator itFind = m_mapGetSectionIndexByNameCache.find(pName);
	if (itFind == m_mapGetSectionIndexByNameCache.end())
		return INVALID_SECTION_INDEX;

	return itFind->second;
}

UINT CINIConfigImplement::GetSectionKeyCount( UINT nSectionIndex )
{
	if (nSectionIndex >= GetSectionCount())
		return 0;

	return m_vConfig[nSectionIndex].second.size();
}

LPCTSTR CINIConfigImplement::GetSectionKey( UINT nSectionIndex, UINT nKeyIndex )
{
	if (nSectionIndex >= GetSectionCount())
		return _T("");

	std::map<UINT, std::vector<CString>>::iterator itFind = m_mapGetSectionKeyCache.find(nSectionIndex);
	if (itFind == m_mapGetSectionKeyCache.end())
	{
		std::pair<std::map<UINT, std::vector<CString>>::iterator, bool> pairCacheInsertIt =
			m_mapGetSectionKeyCache.insert(std::make_pair(nSectionIndex, std::vector<CString>()));

		itFind = pairCacheInsertIt.first;
	}

	if (!itFind->second.size())
	{
		const std::pair<CString, std::map<CString, CString>> & pairSection =
			m_vConfig[nSectionIndex];

		for (std::map<CString, CString>::const_iterator itSectionKVIt = pairSection.second.begin();
			itSectionKVIt != pairSection.second.end();
			itSectionKVIt++)
		{
			itFind->second.push_back(itSectionKVIt->first);
		}
	}

	if (nKeyIndex >= itFind->second.size())
		return _T("");


	return itFind->second[nKeyIndex];
}

BOOL CINIConfigImplement::IsSectionKeyExist( UINT nSectionIndex, LPCTSTR pKey )
{
	if (nSectionIndex >= GetSectionCount())
		return FALSE;

	std::map<CString, CString> & mapKV =
		m_vConfig[nSectionIndex].second;

	std::map<CString, CString>::const_iterator itFind =
		mapKV.find(pKey);

	if (itFind == mapKV.end())
		return FALSE;

	return TRUE;
}

LONG CINIConfigImplement::GetIntValue( UINT nSectionIndex, LPCTSTR pKey )
{
	if (nSectionIndex >= GetSectionCount())
		return 0;

	std::map<CString, CString> & mapKV =
		m_vConfig[nSectionIndex].second;

	std::map<CString, CString>::const_iterator itFind =
		mapKV.find(pKey);

	if (itFind == mapKV.end())
		return 0;

	return _tcstol(itFind->second, NULL, 10);
}

DOUBLE CINIConfigImplement::GetDoubleValue( UINT nSectionIndex, LPCTSTR pKey )
{
	if (nSectionIndex >= GetSectionCount())
		return 0.0;

	std::map<CString, CString> & mapKV =
		m_vConfig[nSectionIndex].second;

	std::map<CString, CString>::const_iterator itFind =
		mapKV.find(pKey);

	if (itFind == mapKV.end())
		return 0.0;

	return _tcstod(itFind->second, NULL);
}

LPCTSTR CINIConfigImplement::GetStringValue( UINT nSectionIndex, LPCTSTR pKey )
{
	if (nSectionIndex >= GetSectionCount())
		return _T("");

	std::map<CString, CString> & mapKV =
		m_vConfig[nSectionIndex].second;

	std::map<CString, CString>::const_iterator itFind =
		mapKV.find(pKey);

	if (itFind == mapKV.end())
		return _T("");

	return itFind->second;
}


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

SMC_IMPLEMENT_INTERFACEIMPL(CConfig)

BOOL CConfig::Init(UINT nConfigType /*= CONFIG_TYPE_INI*/)
{
	if (m_pImplement)
		return TRUE;

	if (nConfigType == CONFIG_TYPE_INI)
	{
		m_pImplement = new CINIConfigImplement();
	}
	/*else if (nConfigType == )*/

	if (!m_pImplement)
		return FALSE;

	return m_pImplement->Init();
}

BOOL CConfig::SetParam(LPCWSTR pKey, LPCWSTR pValue)
{
	if (!m_pImplement)
		return FALSE;

	if (!pKey || !pValue)
		return FALSE;

#ifdef _UNICODE
	return m_pImplement->SetParam(pKey, pValue);
#else
	CStringA strKey = XLibS::StringCode::ConvertWideStrToAnsiStr(pKey);
	CStringA strValue = XLibS::StringCode::ConvertWideStrToAnsiStr(pValue);
	return m_pImplement->SetParam(strKey, strValue);
#endif
}

BOOL CConfig::Load()
{
	if (!m_pImplement)
		return FALSE;

	return m_pImplement->Load();
}

UINT CConfig::GetSectionCount()
{
	if (!m_pImplement)
		return FALSE;

	return m_pImplement->GetSectionCount();
}

LPCWSTR CConfig::GetSectionName(UINT nSectionIndex)
{
	if (!m_pImplement)
		return FALSE;

#ifdef _UNICODE
	return m_pImplement->GetSectionName(nSectionIndex);
#else
	LPCSTR pSectionName = m_pImplement->GetSectionName(nSectionIndex);
	if (!pSectionName)
		return L"";
	m_strGetSectionNameSectionNameBuffer = XLibS::StringCode::ConvertAnsiStrToWideStr(pSectionName);
	return m_strGetSectionNameSectionNameBuffer;
#endif
}

// return INVALID_SECTION_INDEX if not found. 
UINT CConfig::GetSectionIndexByName(LPCWSTR pName)
{
	if (!m_pImplement)
		return INVALID_SECTION_INDEX;

	if (!pName)
		return INVALID_SECTION_INDEX;

#ifdef _UNICODE
	return m_pImplement->GetSectionIndexByName(pName);
#else
	CStringA strName = XLibS::StringCode::ConvertWideStrToAnsiStr(pName);
	return m_pImplement->GetSectionIndexByName(pName);
#endif
}

UINT CConfig::GetSectionKeyCount(UINT nSectionIndex)
{
	if (!m_pImplement)
		return 0;

	return m_pImplement->GetSectionKeyCount(nSectionIndex);
}

LPCWSTR CConfig::GetSectionKey(UINT nSectionIndex, UINT nKeyIndex)
{
	if (!m_pImplement)
		return FALSE;

#ifdef _UNICODE
	return m_pImplement->GetSectionKey(nSectionIndex, nKeyIndex);
#else
	LPCSTR pSectionKey = m_pImplement->GetSectionKey(nSectionIndex, nKeyIndex);
	if (!pSectionKey)
		return L"";
	m_strGetSectionKeySectionKeyBuffer = XLibS::StringCode::ConvertAnsiStrToWideStr(pSectionKey);
	return m_strGetSectionKeySectionKeyBuffer;
#endif
}

BOOL CConfig::IsSectionKeyExist(UINT nSectionIndex, LPCWSTR pKey)
{
	if (!m_pImplement)
		return FALSE;

	if (!pKey)
		return FALSE;

#ifdef _UNICODE
	return m_pImplement->IsSectionKeyExist(nSectionIndex, pKey);
#else
	CStringA strKey = XLibS::StringCode::ConvertWideStrToAnsiStr(pKey);
	return m_pImplement->IsSectionKeyExist(nSectionIndex, strKey);
#endif
}

LONG CConfig::GetIntValue(UINT nSectionIndex, LPCWSTR pKey)
{
	if (!m_pImplement)
		return 0L;

	if (!pKey)
		return 0L;

#ifdef _UNICODE
	return m_pImplement->GetIntValue(nSectionIndex, pKey);
#else
	CStringA strKey = XLibS::StringCode::ConvertWideStrToAnsiStr(pKey);
	return m_pImplement->GetIntValue(nSectionIndex, strKey);
#endif
}

DOUBLE CConfig::GetDoubleValue(UINT nSectionIndex, LPCWSTR pKey)
{
	if (!m_pImplement)
		return 0.0;

	if (!pKey)
		return 0.0;

#ifdef _UNICODE
	return m_pImplement->GetDoubleValue(nSectionIndex, pKey);
#else
	CStringA strKey = XLibS::StringCode::ConvertWideStrToAnsiStr(pKey);
	return m_pImplement->GetDoubleValue(nSectionIndex, strKey);
#endif
}

LPCWSTR CConfig::GetStringValue(UINT nSectionIndex, LPCWSTR pKey)
{
	if (!m_pImplement)
		return L"";

	if (!pKey)
		return L"";

#ifdef _UNICODE
	return m_pImplement->GetStringValue(nSectionIndex, pKey);
#else
	CStringA strKey = XLibS::StringCode::ConvertWideStrToAnsiStr(pKey);
	LPCSTR pRtn = m_pImplement->GetStringValue(nSectionIndex, strKey);
	if (!pRtn)
		return L"";
	m_strGetStringValueValueBuffer = XLibS::StringCode::ConvertAnsiStrToWideStr(pRtn);
	return m_strGetStringValueValueBuffer;
#endif
}

CConfig::CConfig()
	: m_pImplement(NULL)
{
}

CConfig::~CConfig()
{
	if (m_pImplement)
		delete m_pImplement;
}
