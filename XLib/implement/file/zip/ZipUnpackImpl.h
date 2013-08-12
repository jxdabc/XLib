
#include "..\..\..\..\inc\interface\file\zip\ZipUnpack.h"
#include "..\..\..\..\Lib\include\zlib\minizip\unzip.h"

class IZipUnpackOutput;
class CZipUnpack 
	: public IZipUnpack
{
	SMC_DECLARE_INTERFACEIMPL_BEGIN(CZipUnpack, SMC_MUTIINSTANCE)
		SMC_IMPLEMENT_INTERFACE(CZipUnpack, IZipUnpack)
	SMC_DECLARE_INTERFACEIMPL_END(CZipUnpack)

public:
	STDMETHOD_(BOOL, Open)(LPCWSTR pFileName);

	STDMETHOD_(BOOL, IsFileExists)(LPCWSTR pFilePath);

	STDMETHOD_(UINT64, GetFileSize)(LPCWSTR pFilePath);

	/* Before the current listener destroyed, call SetProcessListener(NULL). */
	STDMETHOD_(BOOL, SetProcessListener)(IZipUnpackProcessListener *pListener);

	/* If bAnsy == TRUE, a new thread will run the unpack process. 
	Any method later called will wait until the new thread finish its job. */
	STDMETHOD_(BOOL, UnpackFile)(LPCWSTR pFilePath, BYTE *pBufferOut, UINT64 nBufferSize, BOOL bSync);

	STDMETHOD_(BOOL, UnpackFile)(LPCWSTR pFilePath, LPCWSTR pFilePathOut, BOOL bSync);

	STDMETHOD_(VOID, Close)();

public:
	CZipUnpack();
	~CZipUnpack();

private:
	BOOL CheckAnsyTask(BOOL bWait);

private:
	static DWORD WINAPI DoUnpackBridge(LPVOID lpParameter);
	BOOL DoUnpack(LPCWSTR pFilePath, IZipUnpackOutput *pOutput);

private:
	unzFile m_UnzipFile;
	IZipUnpackProcessListener * m_pListener;
	HANDLE m_hUnpackThread;
};