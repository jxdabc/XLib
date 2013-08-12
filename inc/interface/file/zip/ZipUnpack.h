#pragma once

class IZipUnpack;
class IZipUnpackProcessListener
{
public:
	/* return TRUE to continue. */
	virtual BOOL OnZipUnpackProcessNotify(DOUBLE bProcess, IZipUnpack *pUnpack) = 0;
	virtual VOID OnZipUnpackFinished(BOOL bSuccess, IZipUnpack *pUnpack) = 0;
};


SMC_DECLARE_INTERFACE(IZipUnpack, {9E457D8C-D03D-4021-9B0B-5677F3612657})
	: public ISMCInterface
{
public:
	STDMETHOD_(BOOL, Open)(LPCWSTR pFileName) PURE;

	STDMETHOD_(BOOL, IsFileExists)(LPCWSTR pFilePath) PURE;

	STDMETHOD_(UINT64, GetFileSize)(LPCWSTR pFilePath) PURE;

	/* Before the current listener destroyed, call SetProcessListener(NULL). */
	STDMETHOD_(BOOL, SetProcessListener)(IZipUnpackProcessListener *pListener) PURE;

	STDMETHOD_(BOOL, UnpackFile)(LPCWSTR pFilePath, BYTE *pBufferOut, UINT64 nBufferSize, BOOL bSync = TRUE) PURE;

	STDMETHOD_(BOOL, UnpackFile)(LPCWSTR pFilePath, LPCWSTR pFilePathOut, BOOL bSync = TRUE) PURE;

	STDMETHOD_(VOID, Close)() PURE;
};