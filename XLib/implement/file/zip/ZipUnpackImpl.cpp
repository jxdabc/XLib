#include "stdafx.h"

#include "ZipUnpackImpl.h"

#include "..\..\..\..\inc\interfaceS\string\StringCode.h"

#define UNPACK_BUFFER_SIZE (1024)

SMC_IMPLEMENT_INTERFACEIMPL(CZipUnpack)

#define NOT_CASE_SENSITIVE (2)

BOOL CZipUnpack::Open( LPCWSTR pFileName )
{
	Close();

	if (!pFileName)
		return FALSE;

	m_UnzipFile = unzOpen64(XLibS::StringCode::ConvertWideStrToAnsiStr(pFileName));

	if (!m_UnzipFile)
		return FALSE;

	return TRUE;
}

CZipUnpack::CZipUnpack()
	: m_UnzipFile(NULL),
	m_pListener(NULL),
	m_hUnpackThread(NULL)
{
}

VOID CZipUnpack::Close()
{
	CheckAnsyTask(TRUE);

	if (m_UnzipFile)
	{
		unzClose(m_UnzipFile);
		m_UnzipFile = NULL;
	}

	m_pListener = NULL;
}

BOOL CZipUnpack::IsFileExists( LPCWSTR pFilePath )
{
	CheckAnsyTask(TRUE);

	if (!pFilePath)
		return FALSE;

	CStringA strFilePath(XLibS::StringCode::ConvertWideStrToAnsiStr(pFilePath));
	strFilePath.Replace("\\", "/");
	if (unzLocateFile(m_UnzipFile, strFilePath, NOT_CASE_SENSITIVE))
		return FALSE;

	return TRUE;
}


UINT64 CZipUnpack::GetFileSize( LPCWSTR pFilePath )
{
	CheckAnsyTask(TRUE);

	if (!pFilePath)
		return 0;

	CStringA strFilePath(XLibS::StringCode::ConvertWideStrToAnsiStr(pFilePath));
	strFilePath.Replace("\\", "/");
	if (unzLocateFile(m_UnzipFile, strFilePath, NOT_CASE_SENSITIVE))
		return 0;

	unz_file_info64 info;
	ZeroMemory(&info, sizeof(unz_file_info64));

	if (unzGetCurrentFileInfo64(m_UnzipFile, &info, NULL, 0, NULL, 0, NULL, 0))
		return 0;

	return info.uncompressed_size;
}

BOOL CZipUnpack::SetProcessListener( IZipUnpackProcessListener *pListener )
{
	CheckAnsyTask(TRUE);

	m_pListener = pListener;

	return TRUE;
}

class IZipUnpackOutput
{
public:
	virtual BYTE * GetOutputBuffer(SIZE_T size) = 0;
	virtual BOOL Put(BYTE *pData, SIZE_T size) = 0;
};



BOOL CZipUnpack::DoUnpack( LPCWSTR pFilePath, IZipUnpackOutput *pOutput )
{

	if (!pFilePath || !pOutput)
		return TRUE;

	CStringA strFilePath(XLibS::StringCode::ConvertWideStrToAnsiStr(pFilePath));
	strFilePath.Replace("\\", "/");
	if (unzLocateFile(m_UnzipFile, strFilePath, NOT_CASE_SENSITIVE))
		return FALSE;

	unz_file_info64 info;
	ZeroMemory(&info, sizeof(unz_file_info64));
	if (unzGetCurrentFileInfo64(m_UnzipFile, &info, NULL, 0, NULL, 0, NULL, 0))
		return 0;

	if (unzOpenCurrentFile(m_UnzipFile))
		return FALSE;

	BYTE *pBuffer = NULL;

	BOOL bRtn = FALSE;
	do 
	{
		UINT64 nByteProcessed = 0;

		BOOL bBreak = FALSE;
		while (nByteProcessed < info.uncompressed_size)
		{
			SIZE_T nByteToProcess = min(info.uncompressed_size - nByteProcessed, UNPACK_BUFFER_SIZE);

			BOOL bNeedPut = FALSE;

			BYTE *pDst = pOutput->GetOutputBuffer(nByteToProcess);
			if (!pDst)
			{
				if (!pBuffer) pBuffer = new BYTE [UNPACK_BUFFER_SIZE];
				pDst = pBuffer;
				bNeedPut = TRUE;
			}

			if (unzReadCurrentFile(m_UnzipFile, pDst, nByteToProcess) <= 0)
			{
				bBreak = TRUE;
				break;
			}

			if (bNeedPut && !pOutput->Put(pDst, nByteToProcess))
			{
				bBreak = TRUE;
				break;
			}

			nByteProcessed += nByteToProcess;

			if (m_pListener)
				if (!m_pListener->OnZipUnpackProcessNotify((double)nByteProcessed / info.uncompressed_size, this))
				{
					bBreak = TRUE;
					break;
				}
		}
		if (bBreak)
			break;

		bRtn = TRUE;

	} while (FALSE);

	if (pBuffer)
		delete [] pBuffer;

	unzCloseCurrentFile(m_UnzipFile);

	if (m_pListener)
		m_pListener->OnZipUnpackFinished(bRtn, this);

	return bRtn;
}

class CZipUnpackOutputBuffer
	: public IZipUnpackOutput
{
public:
	CZipUnpackOutputBuffer(BYTE *pBuffer, SIZE_T size);

public:
	virtual BYTE * GetOutputBuffer(SIZE_T size);
	virtual BOOL Put(BYTE *pData, SIZE_T size);

private:
	BYTE *m_pBuffer;
	SIZE_T m_szSize;
	SIZE_T m_szPos;
};

CZipUnpackOutputBuffer::CZipUnpackOutputBuffer(BYTE *pBuffer, SIZE_T size)
	: m_pBuffer(pBuffer),
	m_szSize(size),
	m_szPos(0)
{
}

BYTE * CZipUnpackOutputBuffer::GetOutputBuffer( SIZE_T size )
{
	if (!m_pBuffer)
		return NULL;

	if (m_szSize - m_szPos < size)
		return NULL;

	BYTE *pRtn = m_pBuffer + m_szPos;

	m_szPos += size;

	return pRtn;
}

BOOL CZipUnpackOutputBuffer::Put( BYTE *pData, SIZE_T size )
{
	return FALSE;
}

class CZipUnpackOutputFile
	: public IZipUnpackOutput
{
public:
	CZipUnpackOutputFile(LPCWSTR pFile);
	~CZipUnpackOutputFile();

public:
	virtual BYTE * GetOutputBuffer(SIZE_T size);
	virtual BOOL Put(BYTE *pData, SIZE_T size);

private:
	HANDLE m_hFile;
};

CZipUnpackOutputFile::CZipUnpackOutputFile( LPCWSTR pFile )
	: m_hFile(INVALID_HANDLE_VALUE) 
{
	if (!pFile)
		return;

	m_hFile = 
		::CreateFile(pFile, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);
}

BYTE * CZipUnpackOutputFile::GetOutputBuffer( SIZE_T size )
{
	return NULL;
}

BOOL CZipUnpackOutputFile::Put( BYTE *pData, SIZE_T size )
{
	if (size == 0)
		return TRUE;

	if (m_hFile == INVALID_HANDLE_VALUE)
		return FALSE;

	DWORD dwWritten = 0;
	if (!::WriteFile(m_hFile, pData, size, &dwWritten, NULL))
		return FALSE;

	if (dwWritten != size)
		return FALSE;

	return TRUE;
}

CZipUnpackOutputFile::~CZipUnpackOutputFile()
{
	if (m_hFile != INVALID_HANDLE_VALUE)
		CloseHandle(m_hFile);
}

struct UnpackParam {CZipUnpack *This; LPCWSTR pFilePath; IZipUnpackOutput *pOutput;};

BOOL CZipUnpack::UnpackFile( LPCWSTR pFilePath, BYTE *pBufferOut, UINT64 nBufferSize, BOOL bSync )
{
	static UnpackParam param;

	CheckAnsyTask(TRUE);

	if (!pFilePath)
		return FALSE;

	if (!pBufferOut || nBufferSize == 0)
		return FALSE;
	
	param.This = this;
	param.pFilePath = pFilePath;
	param.pOutput = new CZipUnpackOutputBuffer(pBufferOut, nBufferSize);

	if (bSync)
		return DoUnpackBridge(&param);
	else
		m_hUnpackThread = ::CreateThread(NULL, 0, &DoUnpackBridge, &param, 0, NULL);

	return TRUE;
}

BOOL CZipUnpack::UnpackFile( LPCWSTR pFilePath, LPCWSTR pFilePathOut, BOOL bSync )
{
	static UnpackParam param;

	CheckAnsyTask(TRUE);

	if (!pFilePath)
		return FALSE;

	if (!pFilePathOut)
		return FALSE;

	param.This = this;
	param.pFilePath = pFilePath;
	param.pOutput = new CZipUnpackOutputFile(pFilePathOut);

	if (bSync)
		return DoUnpackBridge(&param);
	else
		m_hUnpackThread = ::CreateThread(NULL, 0, &DoUnpackBridge, &param, 0, NULL);

	return TRUE;
}

DWORD WINAPI CZipUnpack::DoUnpackBridge( LPVOID lpParameter )
{
	if (!lpParameter)
		return FALSE;

	UnpackParam *pParam = (UnpackParam *)lpParameter;

	if (!pParam->pOutput)
		return FALSE;

	BOOL bRst = FALSE;

	if(pParam->This && pParam->pFilePath)
		bRst = pParam->This->DoUnpack(pParam->pFilePath, pParam->pOutput);

	delete pParam->pOutput;

	return bRst;
}

BOOL CZipUnpack::CheckAnsyTask( BOOL bWait )
{
	if (!m_hUnpackThread)
		return TRUE;

	if (::WaitForSingleObject(m_hUnpackThread, bWait ? INFINITE : 0) == WAIT_TIMEOUT)
		return FALSE;

	CloseHandle(m_hUnpackThread);
	m_hUnpackThread = NULL;

	return TRUE;
}

CZipUnpack::~CZipUnpack()
{
	Close();
}


