SMC_DECLARE_INTERFACE(ISmartProcess, {9E23E1A8-64E2-456a-83E1-F8CD5962B30A})
	: public ISMCInterface
{
public:
	STDMETHOD_(BOOL, SetPath)(LPCWSTR pExePath) PURE;
	STDMETHOD_(BOOL, SetCommandLine)(LPCWSTR pCommandLine) PURE;
	STDMETHOD_(BOOL, SetWorkingDirectory)(LPCWSTR pWorkingDirectory) PURE;
	STDMETHOD_(BOOL, SetShowWindow)(int nCmdShow) PURE;
	STDMETHOD_(BOOL, SetRedirectStandardIO)(BOOL bToThisProcessStandardIO) PURE;

	STDMETHOD_(BOOL, SetAutoDestroyOnProcessExit)() PURE;

	STDMETHOD_(BOOL, AnsyRun)() PURE;
	STDMETHOD_(BOOL, SyncRun)() PURE;

	STDMETHOD_(DWORD, Wait)(DWORD dwMilliSec) PURE;

	STDMETHOD_(BOOL, TerminateProcess)() PURE;

	STDMETHOD_(DWORD, GetExitCode)() PURE;

	STDMETHOD_(BOOL, WriteInput)(LPBYTE pBuffer, UINT uLenToWrite) PURE;
	STDMETHOD_(BOOL, ReadStdOutput)(LPBYTE pBuffer, UINT uLenToRead) PURE;
	STDMETHOD_(BOOL, ReadErrOutput)(LPBYTE pBuffer, UINT uLenToRead) PURE;
};