#include "stdafx.h"

#include "SQLDBImpl.h"

#include "mysql/include/mysql.h"
#include "../../../inc/interfaceS/string/StringCode.h"


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
///					Specific DB implements

class IDBImplement
{
public:
	virtual BOOL Init(LPCTSTR host, LPCTSTR username, LPCTSTR password, LPCTSTR db, UINT port) = 0;

	virtual BOOL ExecuteSQL(LPCTSTR sql, BOOL bUseResult, BOOL *pIsEmptyResSet) = 0;

	virtual BOOL GetNextRow(BOOL *pIsEnd) = 0;

	virtual LONG GetIntField(UINT nFieldIndex) = 0;
	virtual DOUBLE GetDoubleField(UINT nFieldIndex) = 0;
	virtual LPCTSTR GetStringField(UINT nFieldIndex) = 0;
	virtual UINT GetBinaryFieldLen(UINT nFieldIndex) = 0;
	virtual LPBYTE GetBinaryField(UINT nFieldIndex) = 0;
	virtual BOOL IsNULLField(UINT nFieldIndex) = 0;


	virtual BOOL FreeResultSet() = 0;
	virtual BOOL Close() = 0;

	virtual CString GetLastError() = 0;
};



//////////////////////////////////////////////////////////////////////////
///								MySQL
class CMySQLDBImplement : public IDBImplement
{
public:
	virtual BOOL Init(LPCTSTR host, LPCTSTR username, LPCTSTR password, LPCTSTR db, UINT port);

	virtual BOOL ExecuteSQL(LPCTSTR sql, BOOL bUseResult, BOOL *pIsEmptyResSet);

	virtual BOOL GetNextRow(BOOL *pIsEnd);

	virtual LONG GetIntField(UINT nFieldIndex);
	virtual DOUBLE GetDoubleField(UINT nFieldIndex);
	virtual LPCTSTR GetStringField(UINT nFieldIndex);
	virtual UINT GetBinaryFieldLen(UINT nFieldIndex);
	virtual LPBYTE GetBinaryField(UINT nFieldIndex);
	virtual BOOL IsNULLField(UINT nFieldIndex);

	virtual BOOL FreeResultSet();
	virtual BOOL Close();

	virtual CString GetLastError();

public:
	CMySQLDBImplement();
	~CMySQLDBImplement();

private:
	MYSQL *m_pMySQL;
	MYSQL_RES *m_pRes;
	MYSQL_ROW m_pCurrentRow;
	CString m_strLastError;

private:
	HMODULE	m_hDLL;
	LPVOID GetProc(LPCSTR pProcName);

private:
	CStringW m_strGetStringFieldResultBuffer;
};


BOOL CMySQLDBImplement::Init( LPCTSTR host, LPCTSTR username, LPCTSTR password, LPCTSTR db, UINT port )
{
	if (m_pMySQL)
	{
		m_strLastError = _T("Already initialized. ");
		return FALSE;
	}

	if (!host || !username || !password || !db)
	{
		m_strLastError = _T("Verity the arguments failed. ");
		return FALSE;
	}

	typedef MYSQL *	(STDCALL *mysql_init_def)(MYSQL *mysql);
	typedef int (STDCALL *mysql_options_def)(MYSQL *mysql, enum mysql_option option, const void *arg);
	typedef MYSQL *	(STDCALL *mysql_real_connect_def)(MYSQL *mysql, const char *host,
		const char *user, const char *passwd, const char *db, unsigned int port, const char *unix_socket, unsigned long clientflag);
	typedef const char * (STDCALL *mysql_error_def)(MYSQL *mysql);

	mysql_init_def mysql_init_proc = (mysql_init_def)GetProc("mysql_init");
	mysql_options_def mysql_options_proc = (mysql_options_def)GetProc("mysql_options");
	mysql_real_connect_def mysql_real_connect_proc = (mysql_real_connect_def)GetProc("mysql_real_connect");
	mysql_error_def mysql_error_proc = (mysql_error_def)GetProc("mysql_error");

	if (!mysql_init_proc || !mysql_options_proc || !mysql_real_connect_proc || !mysql_error_proc)
	{
		m_strLastError = _T("Export functions from DLL failed. ");
		return FALSE;
	}

	m_pMySQL = mysql_init_proc(NULL);
	mysql_options_proc(m_pMySQL, MYSQL_SET_CHARSET_NAME, "gbk");


#ifdef _UNICODE
	CStringA strHost = XLibS::StringCode::ConvertWideStrToAnsiStr(host);
	CStringA strUsername = XLibS::StringCode::ConvertWideStrToAnsiStr(username);
	CStringA strPassword = XLibS::StringCode::ConvertWideStrToAnsiStr(password);
	CStringA strDB	= XLibS::StringCode::ConvertWideStrToAnsiStr(db);
	MYSQL* bConnectRes = mysql_real_connect_proc(m_pMySQL, strHost, strUsername, strPassword, strDB, port, NULL, 0);
#else
	MYSQL* bConnectRes = mysql_real_connect_proc(m_pMySQL, host, username, password, db, port, NULL, 0);
#endif

	if (!bConnectRes)
	{
		LPCSTR pMySQLError = mysql_error_proc(m_pMySQL);
		if (!pMySQLError) pMySQLError = "";
#ifdef _UNICODE
		CStringW strMySQLError = XLibS::StringCode::ConvertAnsiStrToWideStr(pMySQLError);
		m_strLastError.Format(L"connect to DB server failed. ( %s )", (LPCWSTR)strMySQLError);
#else
		m_strLastError.Format("connect to DB server failed. ( %s )", pMySQLError);
#endif

		// Dealloc space. 
		if (!Close())
			assert(NULL);

		return FALSE;
	}

	return TRUE;
}

LPVOID CMySQLDBImplement::GetProc(LPCSTR pProcName)
{
	if (!pProcName)
		return NULL;

	if (!m_hDLL)
	{
		TCHAR szDLLPath[MAX_PATH] = {};
		::GetModuleFileName(THIS_MODULE, szDLLPath, MAX_PATH);
		::PathRemoveFileSpec(szDLLPath);
		::PathAppend(szDLLPath, _T("dependence\\libmysql.dll"));

		m_hDLL = ::LoadLibrary(szDLLPath);
	}

	if (m_hDLL)
		return ::GetProcAddress(m_hDLL, pProcName);

	return NULL;
}

CMySQLDBImplement::CMySQLDBImplement()
	: m_hDLL(NULL),
	m_pMySQL(NULL),
	m_pRes(NULL)
{
}

CMySQLDBImplement::~CMySQLDBImplement()
{
	Close();

	if (m_hDLL)
	{
		::FreeLibrary(m_hDLL);
		m_hDLL = NULL;
	}
}

BOOL CMySQLDBImplement::Close()
{
	typedef void (STDCALL *mysql_close_def)(MYSQL *sock);
	mysql_close_def mysql_close_proc = (mysql_close_def)GetProc("mysql_close");
	if (!mysql_close_proc)
	{
		m_strLastError = _T("Export functions from DLL failed. ");
		return FALSE;
	}

	if (!FreeResultSet())
		assert(NULL);

	if (m_pMySQL)
	{
		mysql_close_proc(m_pMySQL);
		m_pMySQL = NULL;
	}

	return TRUE;
}

BOOL CMySQLDBImplement::ExecuteSQL(LPCTSTR sql, BOOL bUseResult, BOOL *pIsEmptyResSet)
{
	if (!sql)
	{
		m_strLastError = _T("Verity the arguments failed. ");
		return FALSE;
	}

	if (!m_pMySQL)
	{
		m_strLastError = _T("Not initialized. ");
		return FALSE;
	}

	typedef int	(STDCALL *mysql_real_query_def)(MYSQL *mysql, const char *q, unsigned long length);
	typedef const char * (STDCALL *mysql_error_def)(MYSQL *mysql);

	mysql_real_query_def mysql_real_query_proc = (mysql_real_query_def)GetProc("mysql_real_query");
	mysql_error_def mysql_error_proc = (mysql_error_def)GetProc("mysql_error");

	if (!mysql_real_query_proc || !mysql_error_proc)
	{
		m_strLastError = _T("Export functions from DLL failed. ");
		return FALSE;
	}

	if (!FreeResultSet())
		assert(NULL);

#ifdef _UNICODE
	CStringA strSQL = XLibS::StringCode::ConvertWideStrToAnsiStr(sql);
	BOOL bRes = mysql_real_query_proc(m_pMySQL, strSQL, strSQL.GetLength());
#else
	BOOL bRes = mysql_real_query_proc(m_pMySQL, sql, wcslen(sql));
#endif

	if (bRes)
	{
		LPCSTR pMySQLError = mysql_error_proc(m_pMySQL);
		if (!pMySQLError) pMySQLError = "";
#ifdef _UNICODE
		CStringW strMySQLError = XLibS::StringCode::ConvertAnsiStrToWideStr(pMySQLError);
		m_strLastError.Format(L"Execute sql failed. ( %s )", (LPCWSTR)strMySQLError);
#else
		m_strLastError.Format("Execute sql failed. ( %s )", pMySQLError);
#endif
		
		return FALSE;
	}

	if (!bUseResult)
		return TRUE;

	if (!pIsEmptyResSet)
	{
		m_strLastError = _T("No pIsEmptyResSet supplied when specify using the result. ");
		return FALSE;
	}

	return GetNextRow(pIsEmptyResSet); // If any error occur, m_strLastError will be set properly. 

}

BOOL CMySQLDBImplement::GetNextRow( BOOL *pIsEnd )
{
	if (!pIsEnd)
	{
		m_strLastError = _T("Verity the arguments failed. ");
		return FALSE;
	}

	if (!m_pMySQL)
	{
		m_strLastError = _T("Not initialized. ");
		return FALSE;
	}

	typedef MYSQL_RES * (STDCALL *mysql_use_result_def)(MYSQL *mysql);
	typedef MYSQL_ROW (STDCALL *mysql_fetch_row_def)(MYSQL_RES *result);
	typedef const char * (STDCALL *mysql_error_def)(MYSQL *mysql);
	typedef unsigned int (STDCALL *mysql_errno_def)(MYSQL *mysql);

	mysql_use_result_def mysql_use_result_proc = (mysql_use_result_def)GetProc("mysql_use_result");
	mysql_fetch_row_def mysql_fetch_row_proc = (mysql_fetch_row_def)GetProc("mysql_fetch_row");
	mysql_error_def mysql_error_proc = (mysql_error_def)GetProc("mysql_error");
	mysql_errno_def mysql_errno_proc = (mysql_errno_def)GetProc("mysql_errno");

	if (!mysql_use_result_proc || !mysql_fetch_row_proc || !mysql_error_proc || !mysql_errno_proc)
	{
		m_strLastError = _T("Export functions from DLL failed. ");
		return FALSE;
	}

	if (!pIsEnd)
	{
		m_strLastError = _T("Verity the arguments failed. ");
		return FALSE;
	}

	if (!m_pRes)
	{
		m_pRes = mysql_use_result_proc(m_pMySQL);
		if (!m_pRes)
		{
			LPCSTR pMySQLError = mysql_error_proc(m_pMySQL);
			if (!pMySQLError) pMySQLError = "";
#ifdef _UNICODE
			CStringW strMySQLError = XLibS::StringCode::ConvertAnsiStrToWideStr(pMySQLError);
			m_strLastError.Format(L"Get the current result row failed. ( %s )", (LPCWSTR)strMySQLError);
#else
			m_strLastError.Format("Get the current result row failed. ( %s )", pMySQLError);
#endif
			return FALSE;
		}
	}

	m_pCurrentRow = mysql_fetch_row_proc(m_pRes);
	if (!m_pCurrentRow && mysql_errno_proc(m_pMySQL) )
	{
		LPCSTR pMySQLError = mysql_error_proc(m_pMySQL);
		if (!pMySQLError) pMySQLError = "";
#ifdef _UNICODE
		CStringW strMySQLError = XLibS::StringCode::ConvertAnsiStrToWideStr(pMySQLError);
		m_strLastError.Format(L"Get the current row failed. ( %s )", (LPCWSTR)strMySQLError);
#else
		m_strLastError.Format("Get the current row failed. ( %s )", pMySQLError);
#endif
		return FALSE;
	}
	
	*pIsEnd = (m_pCurrentRow == NULL);

	return TRUE;
}

BOOL CMySQLDBImplement::FreeResultSet()
{
	typedef void (STDCALL *mysql_free_result_def)(MYSQL_RES *result);

	mysql_free_result_def mysql_free_result_proc = (mysql_free_result_def)GetProc("mysql_free_result"); 

	if (!mysql_free_result_proc)
	{
		m_strLastError = _T("Export functions from DLL failed. ");
		return FALSE;
	}


	if (m_pRes)
	{
		mysql_free_result_proc(m_pRes);
		m_pRes = NULL;
	}

	return TRUE;
}

LONG CMySQLDBImplement::GetIntField( UINT nFieldIndex )
{
	m_strLastError = _T("");

	if (!m_pMySQL)
	{
		m_strLastError = _T("Not initialized. ");
		return 0L;
	}

	if (!m_pCurrentRow)
	{
		m_strLastError = _T("No row data. ");
		return 0L;
	}

	typedef unsigned int (STDCALL *mysql_field_count_def)(MYSQL *mysql);
	mysql_field_count_def mysql_field_count_proc = (mysql_field_count_def)GetProc("mysql_field_count");
	if (!mysql_field_count_proc)
	{
		m_strLastError = _T("Export functions from DLL failed. ");
		return 0L;
	}

	UINT nFieldCount =  mysql_field_count_proc(m_pMySQL);

	if (nFieldIndex >= nFieldCount)
	{
		m_strLastError = _T("Illegal index supplied. ");
		return 0L;
	}

	return strtol(m_pCurrentRow[nFieldIndex], NULL, 10);
}

DOUBLE CMySQLDBImplement::GetDoubleField( UINT nFieldIndex )
{
	m_strLastError = _T("");

	if (!m_pMySQL)
	{
		m_strLastError = _T("Not initialized. ");
		return 0.0;
	}

	if (!m_pCurrentRow)
	{
		m_strLastError = _T("No row data. ");
		return 0.0;
	}


	typedef unsigned int (STDCALL *mysql_field_count_def)(MYSQL *mysql);
	mysql_field_count_def mysql_field_count_proc = (mysql_field_count_def)GetProc("mysql_field_count");
	if (!mysql_field_count_proc)
	{
		m_strLastError = _T("Export functions from DLL failed. ");
		return 0.0;
	}

	UINT nFieldCount =  mysql_field_count_proc(m_pMySQL);

	if (nFieldIndex >= nFieldCount)
	{
		m_strLastError = _T("Illegal index supplied. ");
		return 0.0;
	}

	return strtod(m_pCurrentRow[nFieldIndex], NULL);
}

BOOL CMySQLDBImplement::IsNULLField( UINT nFieldIndex )
{
	m_strLastError = _T("");

	if (!m_pMySQL)
	{
		m_strLastError = _T("Not initialized. ");
		return TRUE;
	}

	if (!m_pCurrentRow)
	{
		m_strLastError = _T("No row data. ");
		return 0;
	}

	typedef unsigned int (STDCALL *mysql_field_count_def)(MYSQL *mysql);
	mysql_field_count_def mysql_field_count_proc = (mysql_field_count_def)GetProc("mysql_field_count");
	if (!mysql_field_count_proc)
	{
		m_strLastError = _T("Export functions from DLL failed. ");
		return TRUE;
	}

	UINT nFieldCount =  mysql_field_count_proc(m_pMySQL);

	if (nFieldIndex >= nFieldCount)
	{
		m_strLastError = _T("Illegal index supplied. ");
		return TRUE;
	}

	return m_pCurrentRow[nFieldIndex] == NULL;
}


LPCTSTR CMySQLDBImplement::GetStringField( UINT nFieldIndex )
{
	m_strLastError = _T("");

	if (!m_pMySQL)
	{
		m_strLastError = _T("Not initialized. ");
		return _T("");
	}

	if (!m_pCurrentRow)
	{
		m_strLastError = _T("No row data. ");
		return 0;
	}


	typedef unsigned int (STDCALL *mysql_field_count_def)(MYSQL *mysql);
	mysql_field_count_def mysql_field_count_proc = (mysql_field_count_def)GetProc("mysql_field_count");
	if (!mysql_field_count_proc)
	{
		m_strLastError = _T("Export functions from DLL failed. ");
		return _T("");
	}

	UINT nFieldCount =  mysql_field_count_proc(m_pMySQL);

	if (nFieldIndex >= nFieldCount)
	{
		m_strLastError = _T("Illegal index supplied. ");
		return _T("");
	}

#ifdef _UNICODE
	m_strGetStringFieldResultBuffer = XLibS::StringCode::ConvertAnsiStrToWideStr(m_pCurrentRow[nFieldIndex]);
	return m_strGetStringFieldResultBuffer;
#else
	return m_pCurrentRow[nFieldIndex];
#endif

}

UINT CMySQLDBImplement::GetBinaryFieldLen( UINT nFieldIndex )
{
	m_strLastError = _T("");

	if (!m_pMySQL)
	{
		m_strLastError = _T("Not initialized. ");
		return 0;
	}

	if (!m_pCurrentRow)
	{
		m_strLastError = _T("No row data. ");
		return 0;
	}


	typedef unsigned int (STDCALL *mysql_field_count_def)(MYSQL *mysql);
	typedef unsigned long * (STDCALL *mysql_fetch_lengths_def)(MYSQL_RES *result);
	typedef const char * (STDCALL *mysql_error_def)(MYSQL *mysql);

	mysql_field_count_def mysql_field_count_proc = (mysql_field_count_def)GetProc("mysql_field_count");
	mysql_fetch_lengths_def mysql_fetch_lengths_proc = (mysql_fetch_lengths_def)GetProc("mysql_fetch_lengths");
	mysql_error_def mysql_error_proc = (mysql_error_def)GetProc("mysql_error");

	if (!mysql_field_count_proc || !mysql_fetch_lengths_proc || !mysql_error_proc)
	{
		m_strLastError = _T("Export functions from DLL failed. ");
		return 0;
	}

	UINT nFieldCount =  mysql_field_count_proc(m_pMySQL);

	if (nFieldIndex >= nFieldCount)
	{
		m_strLastError = _T("Illegal index supplied. ");
		return 0;
	}

	ULONG *pLens = mysql_fetch_lengths_proc(m_pRes);
	if (!pLens)
	{
		LPCSTR pMySQLError = mysql_error_proc(m_pMySQL);
		if (!pMySQLError) pMySQLError = "";
#ifdef _UNICODE
		CStringW strMySQLError = XLibS::StringCode::ConvertAnsiStrToWideStr(pMySQLError);
		m_strLastError.Format(L"Get field lengths failed. ( %s )", (LPCWSTR)strMySQLError);
#else
		m_strLastError.Format("Get field lengths failed. ( %s )", pMySQLError);
#endif
		return FALSE;
	}

	return pLens[nFieldIndex];

}

LPBYTE CMySQLDBImplement::GetBinaryField( UINT nFieldIndex )
{
	m_strLastError = _T("");

	if (!m_pMySQL)
	{
		m_strLastError = _T("Not initialized. ");
		return 0;
	}

	if (!m_pCurrentRow)
	{
		m_strLastError = _T("No row data. ");
		return 0;
	}


	typedef unsigned int (STDCALL *mysql_field_count_def)(MYSQL *mysql);
	typedef unsigned long * (STDCALL *mysql_fetch_lengths_def)(MYSQL_RES *result);
	typedef const char * (STDCALL *mysql_error_def)(MYSQL *mysql);

	mysql_field_count_def mysql_field_count_proc = (mysql_field_count_def)GetProc("mysql_field_count");
	mysql_fetch_lengths_def mysql_fetch_lengths_proc = (mysql_fetch_lengths_def)GetProc("mysql_fetch_lengths");
	mysql_error_def mysql_error_proc = (mysql_error_def)GetProc("mysql_error");

	if (!mysql_field_count_proc || !mysql_fetch_lengths_proc || !mysql_error_proc)
	{
		m_strLastError = _T("Export functions from DLL failed. ");
		return 0;
	}

	UINT nFieldCount =  mysql_field_count_proc(m_pMySQL);

	if (nFieldIndex >= nFieldCount)
	{
		m_strLastError = _T("Illegal index supplied. ");
		return 0;
	}

	return (LPBYTE)m_pCurrentRow[nFieldIndex];
}

CString CMySQLDBImplement::GetLastError()
{
	return m_strLastError;
}


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

SMC_IMPLEMENT_INTERFACEIMPL(CSQLDB)

BOOL CSQLDB::Init( LPCWSTR host, LPCWSTR username, LPCWSTR password, LPCWSTR db, UINT port /*= 0*/, UINT nDBType /*= SQLDB_MYSQL*/ )
{
	if (!host || !username || !password || !db)
	{
		m_strLastError = L"Verity the arguments failed. ";
		return FALSE;
	}

	if (!m_pImplement)
	{
		if (nDBType == SQLDB_MYSQL)
		{
			m_pImplement = new CMySQLDBImplement();
		}
		/*else if (nDBType == )*/
	}

	if (m_pImplement)
	{
#ifdef _UNICODE
		BOOL bRes = m_pImplement->Init(host, username, password, db, port);
#else
		CStringA strHost = XLibS::StringCode::ConvertWideStrToAnsiStr(host);
		CStringA strUsername = XLibS::StringCode::ConvertWideStrToAnsiStr(username);
		CStringA strPassword = XLibS::StringCode::ConvertWideStrToAnsiStr(password);
		CStringA strDB	= XLibS::StringCode::ConvertWideStrToAnsiStr(db);
		BOOL bRes = m_pImplement->Init(strHost, strUsername, strPassword, strDB, port);
#endif
		if (bRes)
			return TRUE;

#ifdef _UNICODE
		m_strLastError = m_pImplement->GetLastError();
#else
		m_strLastError = XLibS::StringCode::ConvertAnsiStrToWideStr(m_pImplement->GetLastError());
#endif
		return FALSE;
	}
	else
	{
		m_strLastError = L"This DB type is not implemented. ";
		return FALSE;
	}


	assert(!_T("NEVER REACH. "));
	return FALSE;
}

BOOL CSQLDB::ExecuteSQL(LPCWSTR sql, BOOL bUseResult, BOOL *pIsEmptyResSet)
{
	if (!sql)
	{
		m_strLastError = L"Verity the arguments failed. ";
		return FALSE;
	}

	if (!m_pImplement)
	{
		m_strLastError = L"This DB type is not implemented. ";
		return FALSE;
	}

#ifdef _UNICODE
	if (m_pImplement->ExecuteSQL(sql, bUseResult, pIsEmptyResSet))
		return TRUE;
#else
	CStringA strSQL = XLibS::StringCode::ConvertWideStrToAnsiStr(sql);
	if (m_pImplement->ExecuteSQL(strSQL, bUseResult, pIsEmptyResSet))
		return TRUE;
#endif


	

#ifdef _UNICODE
	m_strLastError = m_pImplement->GetLastError();
#else
	m_strLastError = XLibS::StringCode::ConvertAnsiStrToWideStr(m_pImplement->GetLastError());
#endif

	return FALSE;

}

BOOL CSQLDB::GetNextRow(BOOL *pIsEnd)
{
	if (!pIsEnd)
	{
		m_strLastError = L"Verity the arguments failed. ";
		return FALSE;
	}

	if (!m_pImplement)
	{
		m_strLastError = L"This DB type is not implemented. ";
		return FALSE;
	}

	if (m_pImplement->GetNextRow(pIsEnd))
		return TRUE;

#ifdef _UNICODE
	m_strLastError = m_pImplement->GetLastError();
#else
	m_strLastError = XLibS::StringCode::ConvertAnsiStrToWideStr(m_pImplement->GetLastError());
#endif

	return FALSE;
}

LONG CSQLDB::GetIntField( UINT nFieldIndex )
{
	if (!m_pImplement)
	{
		m_strLastError = L"This DB type is not implemented. ";
		return 0L;
	}

	DWORD dwRtn = m_pImplement->GetIntField(nFieldIndex);

#ifdef _UNICODE
	m_strLastError = m_pImplement->GetLastError();
#else
	m_strLastError = XLibS::StringCode::ConvertAnsiStrToWideStr(m_pImplement->GetLastError());
#endif

	return dwRtn;
}

DOUBLE CSQLDB::GetDoubleField( UINT nFieldIndex )
{
	if (!m_pImplement)
	{
		m_strLastError = L"This DB type is not implemented. ";
		return 0.0;
	}

	DOUBLE dRtn = m_pImplement->GetDoubleField(nFieldIndex);

#ifdef _UNICODE
	m_strLastError = m_pImplement->GetLastError();
#else
	m_strLastError = XLibS::StringCode::ConvertAnsiStrToWideStr(m_pImplement->GetLastError());
#endif

	return dRtn;
}


LPCWSTR CSQLDB::GetStringField(UINT nFieldIndex)
{
	if (!m_pImplement)
	{
		m_strLastError = L"This DB type is not implemented. ";
		return FALSE;
	}

#ifdef _UNICODE
	LPCWSTR pRtn = m_pImplement->GetStringField(nFieldIndex);
#else
	m_strGetStringFieldResultBuffer = XLibS::StringCode::ConvertAnsiStrToWideStr(m_pImplement->GetStringFiled(nFieldIndex));
	LPCWSTR *pRtn = m_strGetStringFieldResultBuffer;
#endif

#ifdef _UNICODE
	m_strLastError = m_pImplement->GetLastError();
#else
	m_strLastError = XLibS::StringCode::ConvertAnsiStrToWideStr(m_pImplement->GetLastError());
#endif

	return pRtn;
	
}

LPBYTE CSQLDB::GetBinaryField(UINT nFieldIndex)
{
	if (!m_pImplement)
	{
		m_strLastError = L"This DB type is not implemented. ";
		return FALSE;
	}

	LPBYTE pRtn = m_pImplement->GetBinaryField(nFieldIndex);

#ifdef _UNICODE
	m_strLastError = m_pImplement->GetLastError();
#else
	m_strLastError = XLibS::StringCode::ConvertAnsiStrToWideStr(m_pImplement->GetLastError());
#endif

	return pRtn;
}

BOOL CSQLDB::IsNULLField(UINT nFieldIndex)
{
	if (!m_pImplement)
	{
		m_strLastError = L"This DB type is not implemented. ";
		return FALSE;
	}

	return m_pImplement->IsNULLField(nFieldIndex);
}

UINT CSQLDB::GetBinaryFieldLen(UINT nFieldIndex)
{
	if (!m_pImplement)
	{
		m_strLastError = L"This DB type is not implemented. ";
		return FALSE;
	}

	UINT nRtn = m_pImplement->GetBinaryFieldLen(nFieldIndex);

#ifdef _UNICODE
	m_strLastError = m_pImplement->GetLastError();
#else
	m_strLastError = XLibS::StringCode::ConvertAnsiStrToWideStr(m_pImplement->GetLastError());
#endif

	return nRtn;
}

BOOL CSQLDB::FreeResultSet()
{
	if (!m_pImplement)
	{
		m_strLastError = L"This DB type is not implemented. ";
		return FALSE;
	}

	if (m_pImplement->FreeResultSet())
		return TRUE;

#ifdef _UNICODE
	m_strLastError = m_pImplement->GetLastError();
#else
	m_strLastError = XLibS::StringCode::ConvertAnsiStrToWideStr(m_pImplement->GetLastError());
#endif

	return FALSE;
}


BOOL CSQLDB::Close()
{
	if (!m_pImplement)
	{
		m_strLastError = L"This DB type is not implemented. ";
		return FALSE;
	}

	if (m_pImplement->Close())
		return TRUE;

#ifdef _UNICODE
	m_strLastError = m_pImplement->GetLastError();
#else
	m_strLastError = XLibS::StringCode::ConvertAnsiStrToWideStr(m_pImplement->GetLastError());
#endif

	return FALSE;
}

LPCWSTR CSQLDB::GetLastError()
{
	return m_strLastError;
}

CSQLDB::CSQLDB()
	: m_pImplement(NULL)
{
}

CSQLDB::~CSQLDB()
{
	if (m_pImplement)
	{
		delete m_pImplement;
		m_pImplement = NULL;
	}
}