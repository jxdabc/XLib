#include "../../libcommon.h"
#include "../../../inc/interfaceS/persistance/FileJsonArchive.h"

#include <shlobj.h>

#include "../../../Lib/include/json/writer.h"
#include "../../../Lib/include/json/reader.h"


XLibS::CFileJsonArchive::CFileJsonArchive( LPCWSTR szFileName )
	: m_strFileName(szFileName)
{
}

BOOL XLibS::CFileJsonArchive::SaveToFile( const Json::Value & jvJsonValue )
{
	Json::FastWriter writer;
	std::string strBuffer = writer.write(jvJsonValue);

	// 如果目录不存在，创建目录
	CString strBufferDirectory(m_strFileName);
	PathRemoveFileSpec(strBufferDirectory.GetBuffer(MAX_PATH));
	strBufferDirectory.ReleaseBuffer();
	if (!PathFileExists(strBufferDirectory))
		SHCreateDirectory(NULL, strBufferDirectory);

	HANDLE hFile = ::CreateFile(m_strFileName, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, 0, NULL);
	DWORD dwWritten = 0;
	BOOL bRes = ::WriteFile(hFile, strBuffer.c_str(), strBuffer.length(), &dwWritten, NULL);
	CloseHandle(hFile);

	return (bRes && dwWritten == strBuffer.length());
}

BOOL XLibS::CFileJsonArchive::LoadFromFile( Json::Value *pJsonValue )
{
	if (!pJsonValue)
		return FALSE;

	HANDLE hFile = ::CreateFile(m_strFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
	if (hFile == INVALID_HANDLE_VALUE || !hFile)
		return FALSE;

	LARGE_INTEGER FileSize;
	if (!GetFileSizeEx(hFile, &FileSize) || FileSize.HighPart != 0)
		return FALSE;

	BOOL bRes = FALSE;

	BYTE *pBuffer = new BYTE[FileSize.LowPart];
	ZeroMemory(pBuffer, FileSize.LowPart);
	do 
	{
		DWORD dwRead = 0;
		BOOL bRes = ReadFile(hFile, pBuffer, FileSize.LowPart, &dwRead, NULL);
		if (!bRes || dwRead != FileSize.LowPart)
			break;

		std::string strBuffer((char *)pBuffer, FileSize.LowPart);

		Json::Reader reader;

		Json::Value &jvJsonValue = *pJsonValue;
		jvJsonValue = Json::Value(Json::nullValue);

		bRes = reader.parse(strBuffer, jvJsonValue, false);
	} while (FALSE);
	delete [] pBuffer;


	return bRes;
}

