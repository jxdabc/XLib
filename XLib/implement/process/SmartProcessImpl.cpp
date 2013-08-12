#include "stdafx.h"
#include "SmartProcessImpl.h"

SMC_IMPLEMENT_INTERFACEIMPL(CSmartProcess)

CSmartProcess::CSmartProcess()
	: m_bAutoDestroy(FALSE),
	m_hInputRead(NULL),
	m_hInputWrite(NULL),
	m_hStdOutRead(NULL),
	m_hStdOutWrite(NULL),
	m_hErrOutRead(NULL),
	m_hErrOutWrite(NULL),
	m_hProcess(NULL),
	m_bIsAnsyRun(FALSE)
{
	ZeroMemory(&m_StartUpInfo, sizeof(m_StartUpInfo));
	m_StartUpInfo.cb = sizeof(m_StartUpInfo);
	GetStartupInfo(&m_StartUpInfo);
}

CSmartProcess::~CSmartProcess()
{
	CloseHandle(m_hInputRead);
	m_hInputRead = NULL;
	CloseHandle(m_hInputWrite);
	m_hInputWrite = NULL;
	CloseHandle(m_hStdOutRead);
	m_hInputRead = NULL;
	CloseHandle(m_hStdOutWrite);
	m_hInputWrite = NULL;
	CloseHandle(m_hErrOutRead);
	m_hInputRead = NULL;
	CloseHandle(m_hErrOutWrite);
	m_hInputWrite = NULL;

	CloseHandle(m_hProcess);
	m_hProcess = NULL;
}

BOOL CSmartProcess::SetPath(LPCWSTR pExePath)
{
	if (!pExePath)
		return FALSE;

	m_strExePath = pExePath;
	
	return TRUE;
}

BOOL CSmartProcess::SetCommandLine(LPCWSTR pCommandLine)
{
	if (!pCommandLine)
		return FALSE;

	m_strCommandLine = pCommandLine;

	return TRUE;
}

BOOL CSmartProcess::SetWorkingDirectory(LPCWSTR pWorkingDirectory)
{
	if (!pWorkingDirectory)
		return FALSE;

	m_strWorkingDirectory = pWorkingDirectory;

	return TRUE;
}

BOOL CSmartProcess::SetShowWindow(int nCmdShow)
{
	m_StartUpInfo.dwFlags |= STARTF_USESHOWWINDOW;
	m_StartUpInfo.wShowWindow = nCmdShow;

	return TRUE;
}

BOOL CSmartProcess::SetRedirectStandardIO(BOOL bToThisProcessStandardIO)
{
	m_StartUpInfo.dwFlags |= STARTF_USESTDHANDLES;

	if (bToThisProcessStandardIO)
	{
		m_StartUpInfo.hStdInput = ::GetStdHandle(STD_INPUT_HANDLE);
		m_StartUpInfo.hStdOutput = ::GetStdHandle(STD_OUTPUT_HANDLE);
		m_StartUpInfo.hStdError = ::GetStdHandle(STD_ERROR_HANDLE);
	}
	else
	{
		SECURITY_ATTRIBUTES security;
		security.nLength = sizeof(security);
		security.lpSecurityDescriptor = NULL;
		security.bInheritHandle = TRUE;

		::CreatePipe(&m_hInputRead, &m_hInputWrite, &security, 0);
		::CreatePipe(&m_hStdOutRead, &m_hStdOutWrite, &security, 0);
		::CreatePipe(&m_hErrOutRead, &m_hErrOutWrite, &security, 0);

		m_StartUpInfo.hStdInput = m_hInputRead;
		m_StartUpInfo.hStdOutput = m_hStdOutWrite;
		m_StartUpInfo.hStdError = m_hErrOutWrite;
	}


	return TRUE;
}

BOOL CSmartProcess::SetAutoDestroyOnProcessExit()
{
	m_bAutoDestroy = TRUE;
	return TRUE;
}


DWORD WINAPI CSmartProcess::ThreadProc_AutoDestroy( LPVOID lpThreadParameter )
{
	if (!lpThreadParameter)
		return EXIT_FAILURE;
	
	CSmartProcess *pThis = (CSmartProcess *)lpThreadParameter;

	WaitForSingleObject(pThis->m_hProcess, INFINITE);

	pThis->AllRelease();

	return EXIT_SUCCESS;
}

BOOL CSmartProcess::AnsyRun()
{
	// Every instance can be run only once. 
	assert(m_hProcess == NULL);
	if (m_hProcess)
		return FALSE;

	m_bIsAnsyRun = TRUE;

	HANDLE hProcess = StartProcess();
	if (!hProcess)
		return FALSE;

	m_hProcess = hProcess;

	if (m_bAutoDestroy)
	{
		HANDLE hAutoDestoryCreateThread = CreateThread(NULL, 0, &ThreadProc_AutoDestroy, this, 0, NULL);
		CloseHandle(hAutoDestoryCreateThread);
	}

	return TRUE;
}

BOOL CSmartProcess::SyncRun()
{
	// Every instance can be run only once. 
	assert(m_hProcess == NULL);
	if (m_hProcess)
		return FALSE;

	HANDLE hProcess = StartProcess();
	if (!hProcess)
		return FALSE;

	m_hProcess = hProcess;

	WaitForSingleObject(hProcess, INFINITE);

	if (m_bAutoDestroy)
		AllRelease();

	return TRUE;
}

HANDLE CSmartProcess::StartProcess()
{
	if (m_strExePath.IsEmpty())
		return NULL;

	if (m_strWorkingDirectory.IsEmpty())
	{
		m_strWorkingDirectory = m_strExePath;
		PathRemoveFileSpec(m_strWorkingDirectory.GetBuffer());
		m_strWorkingDirectory.ReleaseBuffer();
	}

	PROCESS_INFORMATION info;
	BOOL bRes = ::CreateProcess(m_strExePath, m_strCommandLine.GetBuffer(), NULL, NULL, TRUE, 0, NULL, 
		m_strWorkingDirectory, &m_StartUpInfo, &info);
	m_strCommandLine.ReleaseBuffer();

	if (!bRes)
		return NULL;

	CloseHandle(info.hThread);

	CloseHandle(m_hInputRead);
	CloseHandle(m_hStdOutWrite);
	CloseHandle(m_hErrOutWrite);

	return info.hProcess;
}

BOOL CSmartProcess::TerminateProcess()
{
	if (!m_hProcess)
		return FALSE;

	::TerminateProcess(m_hProcess, EXIT_FAILURE);

	if (m_bAutoDestroy && !m_bIsAnsyRun)
		AllRelease();

	return TRUE;
}

DWORD CSmartProcess::GetExitCode()
{
	DWORD dwExitCode = 0;
	::GetExitCodeProcess(m_hProcess, &dwExitCode);
	return dwExitCode;
}

BOOL CSmartProcess::WriteInput(LPBYTE pBuffer, UINT uLenToWrite)
{
	if (!pBuffer)
		return FALSE;

	if (!m_hInputWrite)
		return FALSE;

	DWORD dwWritten = 0;
	BOOL bRes = ::WriteFile(m_hInputWrite, pBuffer, uLenToWrite, &dwWritten, NULL);
	return bRes && uLenToWrite == dwWritten;
}

BOOL CSmartProcess::ReadStdOutput(LPBYTE pBuffer, UINT uLenToRead)
{
	if (!pBuffer)
		return FALSE;

	DWORD dwRead = 0;
	BOOL bRes = ::ReadFile(m_hStdOutRead, pBuffer, uLenToRead, &dwRead, NULL);
	return bRes && uLenToRead == dwRead;
}

BOOL CSmartProcess::ReadErrOutput(LPBYTE pBuffer, UINT uLenToRead)
{
	if (!pBuffer)
		return FALSE;

	DWORD dwRead = 0;
	BOOL bRes = ::ReadFile(m_hErrOutRead, pBuffer, uLenToRead, &dwRead, NULL);
	return bRes && uLenToRead == dwRead;
}

DWORD CSmartProcess::Wait(DWORD dwMilliSec)
{
	if (!m_hProcess)
		return WAIT_FAILED;

	return ::WaitForSingleObject(m_hProcess, dwMilliSec);
}
