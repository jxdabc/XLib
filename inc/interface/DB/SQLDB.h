#pragma once

#define SQLDB_MYSQL (1)


SMC_DECLARE_INTERFACE(ISQLDB, {BD1342DB-2BE6-445a-B79F-95C718E36B26})
	: public ISMCInterface
{
public:
	STDMETHOD_(BOOL, Init)(LPCWSTR host, LPCWSTR username, LPCWSTR password, LPCWSTR db, UINT port = 0, UINT nDBType = SQLDB_MYSQL) PURE;

	STDMETHOD_(BOOL, ExecuteSQL)(LPCWSTR sql, BOOL bUseResult, BOOL *pIsEmptyResSet) PURE;

	STDMETHOD_(BOOL, GetNextRow)(BOOL *pIsEnd) PURE;

	STDMETHOD_(LONG, GetIntField)(UINT nFieldIndex) PURE;
	STDMETHOD_(DOUBLE, GetDoubleField)(UINT nFieldIndex) PURE;
	STDMETHOD_(LPCWSTR, GetStringField)(UINT nFieldIndex) PURE;
	STDMETHOD_(UINT, GetBinaryFieldLen)(UINT nFieldIndex) PURE;
	STDMETHOD_(LPBYTE, GetBinaryField)(UINT nFieldIndex) PURE;
	STDMETHOD_(BOOL, IsNULLField)(UINT nFieldIndex) PURE;

	STDMETHOD_(BOOL, FreeResultSet)() PURE;
	STDMETHOD_(BOOL, Close)() PURE;

	STDMETHOD_(LPCWSTR, GetLastError)() PURE;
};


inline CStringW __cdecl __SQLDB_MAKE_SQL(LPCWSTR format, ...)
{
	if (!format)
		return _T("");

	CStringW strSQL;
	va_list args;
	va_start(args, format);
	strSQL.FormatV(format, args);
	va_end(args);
	return strSQL;
}

#define SQLDB_MAKE_SQL(format, ...) ( __SQLDB_MAKE_SQL(format, __VA_ARGS__) )