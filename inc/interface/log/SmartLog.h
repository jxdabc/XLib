#pragma once

#define SMART_LOG_TYPE_TRACE (0)
#define SMART_LOG_TYPE_EVENT (1)

#define SMART_LOG_DES_DEBUGSTR (0x1)
#define SMART_LOG_DES_FILE (0x2)


SMC_DECLARE_INTERFACE(ISmartLog, {61EC95F0-4E2A-4bd9-94D7-C070C7021796})
	: public ISMCInterface
{
public:
	STDMETHOD_(BOOL, SetLogDestination)(DWORD dwType, DWORD dwDestinations) PURE;

	STDMETHOD_(BOOL, SetLogTypeDescription)(DWORD dwType, LPCWSTR pDescription) PURE;

	STDMETHOD_(BOOL, SetLogParam)(LPCWSTR pKey, LPCWSTR pValue) PURE;

	STDMETHOD_(BOOL, Log)(DWORD dwType, LPCWSTR pLogStr) PURE;
};


inline CStringW __cdecl __SMART_LOG_MAKE_LOGSTR(LPCWSTR format, ...)
{
	if (!format)
		return _T("");

	CStringW strLogStr;
	va_list args;
	va_start(args, format);
	strLogStr.FormatV(format, args);
	va_end(args);
	return strLogStr;
}

#define SMART_LOG_MAKE_LOGSTR(format, ...) ( __SMART_LOG_MAKE_LOGSTR(format, __VA_ARGS__) )