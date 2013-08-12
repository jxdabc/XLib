#include "../../../inc/interface/process/SmartProcess.h"

class CSmartProcess : public ISmartProcess
{
	SMC_DECLARE_INTERFACEIMPL_BEGIN(CSmartProcess, SMC_MUTIINSTANCE)
		SMC_IMPLEMENT_INTERFACE(CSmartProcess, ISmartProcess)
	SMC_DECLARE_INTERFACEIMPL_END(CSmartProcess)

public:
	STDMETHOD_(BOOL, SetPath)(LPCWSTR pExePath);
	STDMETHOD_(BOOL, SetCommandLine)(LPCWSTR pCommandLine);
	STDMETHOD_(BOOL, SetWorkingDirectory)(LPCWSTR pWorkingDirectory);
	STDMETHOD_(BOOL, SetShowWindow)(int nCmdShow);
	STDMETHOD_(BOOL, SetRedirectStandardIO)(BOOL bToThisProcessStandardIO);

	STDMETHOD_(BOOL, SetAutoDestroyOnProcessExit)();

	STDMETHOD_(BOOL, AnsyRun)();
	STDMETHOD_(BOOL, SyncRun)();

	STDMETHOD_(DWORD, Wait)(DWORD dwMilliSec);

	STDMETHOD_(BOOL, TerminateProcess)();

	STDMETHOD_(DWORD, GetExitCode)();

	STDMETHOD_(BOOL, WriteInput)(LPBYTE pBuffer, UINT uLenToWrite);
	STDMETHOD_(BOOL, ReadStdOutput)(LPBYTE pBuffer, UINT uLenToRead);
	STDMETHOD_(BOOL, ReadErrOutput)(LPBYTE pBuffer, UINT uLenToRead);

public:
	CSmartProcess();
	~CSmartProcess();

private:
	HANDLE StartProcess();

private:
	static DWORD WINAPI ThreadProc_AutoDestroy( LPVOID lpThreadParameter );

private:
	CString m_strExePath;
	CString m_strCommandLine;
	CString m_strWorkingDirectory;

	BOOL m_bAutoDestroy;

	STARTUPINFO m_StartUpInfo;

	HANDLE m_hInputRead;
	HANDLE m_hInputWrite;
	HANDLE m_hStdOutRead;
	HANDLE m_hStdOutWrite;
	HANDLE m_hErrOutRead;
	HANDLE m_hErrOutWrite;

	HANDLE m_hProcess;

	BOOL m_bIsAnsyRun;
};
