#pragma once

#include "../../../inc/interface/file/MemMappedFile.h"

class CMemMappedFile 
	: public IMemMappedFile
{
	SMC_DECLARE_INTERFACEIMPL_BEGIN(CMemMappedFile, SMC_MUTIINSTANCE)
		SMC_IMPLEMENT_INTERFACE(CMemMappedFile, IMemMappedFile)
	SMC_DECLARE_INTERFACEIMPL_END(CMemMappedFile)

public:

	STDMETHOD_(BOOL, Open)(LPCWSTR pFileName, BOOL bWriteable);

	STDMETHOD_(UINT64, GetFileSize)();

	STDMETHOD_(BYTE *, Map)(DWORD dwOffsetLow, SIZE_T szSizeToMap, DWORD dwOffsetHigh);

	STDMETHOD_(BOOL, Unmap)(BYTE *p);

	STDMETHOD_(VOID, Close)();

public:
	CMemMappedFile();
	~CMemMappedFile();

private:
	CStringW m_strFileName;
	BOOL m_bWriteable;
	HANDLE m_hFile;
	HANDLE m_hFileMapping;
	std::set<VOID *> m_setMappedView;
};