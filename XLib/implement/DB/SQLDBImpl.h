#pragma once

#include "../../../inc/interface/DB/SQLDB.h"

class IDBImplement;
class CSQLDB : public ISQLDB
{
	SMC_DECLARE_INTERFACEIMPL_BEGIN(CSQLDB, SMC_MUTIINSTANCE)
		SMC_IMPLEMENT_INTERFACE(CSQLDB, ISQLDB)
	SMC_DECLARE_INTERFACEIMPL_END(CSQLDB)

public:
	STDMETHOD_(BOOL, Init)(LPCWSTR host, LPCWSTR username, LPCWSTR password, LPCWSTR db, UINT port = 0, UINT nDBType = SQLDB_MYSQL);

	STDMETHOD_(BOOL, ExecuteSQL)(LPCWSTR sql, BOOL bUseResult, BOOL *pIsEmptyResSet);

	STDMETHOD_(BOOL, GetNextRow)(BOOL *pIsEnd);

	STDMETHOD_(LONG, GetIntField)(UINT nFieldIndex);
	STDMETHOD_(DOUBLE, GetDoubleField)(UINT nFieldIndex);
	STDMETHOD_(LPCWSTR, GetStringField)(UINT nFieldIndex);
	STDMETHOD_(UINT, GetBinaryFieldLen)(UINT nFieldIndex);
	STDMETHOD_(LPBYTE, GetBinaryField)(UINT nFieldIndex);
	STDMETHOD_(BOOL, IsNULLField)(UINT nFieldIndex);

	STDMETHOD_(BOOL, FreeResultSet)();
	STDMETHOD_(BOOL, Close)();

	STDMETHOD_(LPCWSTR, GetLastError)();

public:
	CSQLDB();
	~CSQLDB();

private:
	IDBImplement *m_pImplement;
	CStringW	m_strLastError;

private:
	CStringW m_strGetStringFieldResultBuffer;
};
