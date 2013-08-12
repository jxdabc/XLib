#pragma once

SMC_DECLARE_INTERFACE(IMemMappedFile, {B46DA50B-D641-407a-8ABB-B0997FDEE317})
	: public ISMCInterface
{
public:
	STDMETHOD_(BOOL, Open)(LPCWSTR pFileName, BOOL bWriteable = FALSE) PURE;

	STDMETHOD_(UINT64, GetFileSize)() PURE;

	STDMETHOD_(BYTE *, Map)(DWORD dwOffsetLow = 0, SIZE_T szSizeToMap = 0, DWORD dwOffsetHigh = 0) PURE;

	STDMETHOD_(BOOL, Unmap)(BYTE *p) PURE;

	STDMETHOD_(VOID, Close)() PURE;
};
