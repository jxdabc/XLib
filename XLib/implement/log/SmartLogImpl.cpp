#include "stdafx.h"
#include "SmartLogImpl.h"

//////////////////////////////////////////////////////////////////////////
class ISmartLogDestionation
{
public:
	virtual VOID OnLog(DWORD dwDestionationFlag, LPCTSTR logstr) PURE;
	virtual VOID OnParam(LPCTSTR key, LPCTSTR value) PURE;
};
//////////////////////////////////////////////////////////////////////////
class CSmartLogFileDest
	: public ISmartLogDestionation
{
public:
	virtual VOID OnLog(DWORD dwDestionationFlag, LPCTSTR logstr);
	virtual VOID OnParam(LPCTSTR key, LPCTSTR value);

public:
	CSmartLogFileDest();
	~CSmartLogFileDest();

private:
	HANDLE m_hFile;
};
VOID CSmartLogFileDest::OnParam( LPCTSTR key, LPCTSTR value )
{
	if (!key || !value)
		return;

	if (!_tcsicmp(key, _T("FILEPATH")))
	{
		CloseHandle(m_hFile);

		LPCTSTR pFilePath = value;
		m_hFile = 
			::CreateFile(pFilePath, GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
		SetFilePointer(m_hFile, 0, NULL, FILE_END); 
	}
	/*else if (...)*/
}
VOID CSmartLogFileDest::OnLog( DWORD dwDestionationFlag, LPCTSTR logstr )
{
	if (!logstr)
		return;

	if ((dwDestionationFlag & SMART_LOG_DES_FILE) == 0)
		return;

	if (!m_hFile || m_hFile == INVALID_HANDLE_VALUE)
		return;

	DWORD dwWritten = 0;
	::WriteFile(m_hFile, logstr, _tcslen(logstr) * sizeof(TCHAR), &dwWritten, NULL);
	::FlushFileBuffers(m_hFile);
}
CSmartLogFileDest::CSmartLogFileDest()
	: m_hFile(NULL)
{

}
CSmartLogFileDest::~CSmartLogFileDest()
{
	CloseHandle(m_hFile);
	m_hFile = NULL;
}
//////////////////////////////////////////////////////////////////////////
class CSmartLogDebugStrDest
	: public ISmartLogDestionation
{
public:
	virtual VOID OnLog(DWORD dwDestionationFlag, LPCTSTR logstr);
	virtual VOID OnParam(LPCTSTR key, LPCTSTR value);
};
VOID CSmartLogDebugStrDest::OnLog( DWORD dwDestionationFlag, LPCTSTR logstr )
{
	if ((dwDestionationFlag & SMART_LOG_DES_DEBUGSTR) == 0)
		return;

	::OutputDebugString(logstr);
}
VOID CSmartLogDebugStrDest::OnParam( LPCTSTR key, LPCTSTR value )
{
}
//////////////////////////////////////////////////////////////////////////




//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

SMC_IMPLEMENT_INTERFACEIMPL(CSmartLog)

BOOL CSmartLog::SetLogDestination(DWORD dwType, DWORD dwDestinations)
{
	SMCAutoLock lock(&m_Lock);

	std::map<DWORD, std::pair<DWORD, CString>>::iterator itFind =
		m_mapTypes.find(dwType);
	if (itFind == m_mapTypes.end())
	{
		if (dwDestinations == 0)
			return TRUE;

		m_mapTypes.insert(std::make_pair(dwType, std::make_pair(dwDestinations, _T(""))));
	}
	else
	{
		if (dwDestinations == 0 && itFind->second.second.IsEmpty())
			m_mapTypes.erase(itFind);
		else
			itFind->second.first = dwDestinations;
	}

	return TRUE;
}

BOOL CSmartLog::SetLogTypeDescription(DWORD dwType, LPCWSTR pDescription)
{
	SMCAutoLock lock(&m_Lock);

	if (!pDescription)
		return FALSE;

	std::map<DWORD, std::pair<DWORD, CString>>::iterator itFind =
		m_mapTypes.find(dwType);
	if (itFind == m_mapTypes.end())
		m_mapTypes.insert(std::make_pair(dwType, std::make_pair(0, pDescription)));
	else
		itFind->second.second = pDescription;

	return TRUE;
}

BOOL CSmartLog::SetLogParam(LPCWSTR pKey, LPCWSTR pValue)
{
	SMCAutoLock lock(&m_Lock);

	if (!pKey || !pValue)
		return FALSE;

	std::vector<ISmartLogDestionation *>::const_iterator it =
		m_vDestionations.begin();
	for ( ; it != m_vDestionations.end(); it++)
		if (*it)
			(*it)->OnParam(pKey, pValue);

	return TRUE;
}



BOOL CSmartLog::Log(DWORD dwType, LPCWSTR pLogStr)
{
	SMCAutoLock lock(&m_Lock);

	if (!pLogStr)
		return FALSE;

	std::map<DWORD, std::pair<DWORD, CString>>::const_iterator itFind =
		m_mapTypes.find(dwType);
	if (itFind == m_mapTypes.end())
		return FALSE;

	DWORD dwDestionationFlag = itFind->second.first;
	const CString & strDescription = itFind->second.second;

	if (dwDestionationFlag == 0)
		return TRUE;

	SYSTEMTIME time; 
	GetLocalTime( &time ); 

	CString strLog;
	strLog.Format(_T("## %s ## ( %d/%02d/%02d %02d:%02d:%02d.%04d ) : %s \r\n"),
		(LPCTSTR)strDescription,
		time.wYear,time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond, time.wMilliseconds, // time
		pLogStr);

	std::vector<ISmartLogDestionation *>::const_iterator it =
		m_vDestionations.begin();
	for ( ; it != m_vDestionations.end(); it++)
		if (*it)
			(*it)->OnLog(dwDestionationFlag, strLog);
		
	return TRUE;
}

CSmartLog::CSmartLog()
{
	m_vDestionations.push_back(new CSmartLogFileDest());
	m_vDestionations.push_back(new CSmartLogDebugStrDest());
}

CSmartLog::~CSmartLog()
{
	std::vector<ISmartLogDestionation *>::const_iterator it =
		m_vDestionations.begin();
	for ( ; it != m_vDestionations.end(); it++)
		if (*it)
			delete *it;

	m_vDestionations.clear();
}
