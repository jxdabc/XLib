#include "stdafx.h"
#include "MemMappedFileImpl.h"

SMC_IMPLEMENT_INTERFACEIMPL(CMemMappedFile)

BOOL CMemMappedFile::Open( LPCWSTR pFileName, BOOL bWriteable)
{
	Close();

	if (!pFileName || !PathFileExists(pFileName))
		return FALSE;

	m_strFileName = pFileName;
	m_bWriteable = bWriteable;

	return TRUE;
}

CMemMappedFile::CMemMappedFile()
	: m_bWriteable(FALSE),
	m_hFile(INVALID_HANDLE_VALUE),
	m_hFileMapping(NULL)
{

}

BYTE * CMemMappedFile::Map( DWORD dwOffsetLow, SIZE_T szSizeToMap, DWORD dwOffsetHigh )
{
	if (m_strFileName.IsEmpty())
		return NULL;

	if (m_hFile == INVALID_HANDLE_VALUE)
		m_hFile = ::CreateFileW(m_strFileName, GENERIC_READ | (m_bWriteable ? GENERIC_WRITE : 0), 0, NULL, OPEN_EXISTING, 0, NULL);

	if (m_hFile != INVALID_HANDLE_VALUE && !m_hFileMapping)
		m_hFileMapping = ::CreateFileMapping(m_hFile, NULL, m_bWriteable ? PAGE_READWRITE : PAGE_READONLY, 0, 0, NULL);

	if (!m_hFileMapping)
		return NULL;

	VOID *p = ::MapViewOfFile(m_hFileMapping, FILE_MAP_READ | (m_bWriteable ? FILE_MAP_WRITE : 0), dwOffsetHigh, dwOffsetLow, szSizeToMap);

	if (p)
		m_setMappedView.insert(p);

	return (BYTE *)p;
}

BOOL CMemMappedFile::Unmap( BYTE *p )
{
	if (!p)
		return FALSE;

	if (::UnmapViewOfFile(p))
	{
		m_setMappedView.erase(p);
		return TRUE;
	}

	return FALSE;
}

VOID CMemMappedFile::Close()
{
	for (std::set<VOID *>::const_iterator it = m_setMappedView.begin();
		it != m_setMappedView.end(); it++)
		::UnmapViewOfFile(*it);

	m_setMappedView.clear();

	if (m_hFileMapping)
	{
		CloseHandle(m_hFileMapping);
		m_hFileMapping = NULL;
	}
	if (m_hFile != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_hFile);
		m_hFile = INVALID_HANDLE_VALUE;
	}

	m_strFileName.Empty();
	m_bWriteable = FALSE;
}

CMemMappedFile::~CMemMappedFile()
{
	Close();
}

UINT64 CMemMappedFile::GetFileSize()
{
	if (m_strFileName.IsEmpty())
		return 0;

	if (m_hFile == INVALID_HANDLE_VALUE)
		m_hFile = ::CreateFileW(m_strFileName, GENERIC_READ | (m_bWriteable ? GENERIC_WRITE : 0), 0, NULL, OPEN_EXISTING, 0, NULL);

	if (m_hFile == INVALID_HANDLE_VALUE)
		return 0;

	LARGE_INTEGER l;
	if (::GetFileSizeEx(m_hFile, &l))
		return l.QuadPart;

	return 0;
}