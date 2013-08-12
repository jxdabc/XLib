// ConsoleTest.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"

#include <windows.h>
#include <tchar.h>
#include <wtypes.h>

#include "..\..\inc\convention.h"
#include "..\..\inc\interface\file\zip\ZipUnpack.h"
#include "..\..\inc\interfaceS\string\StringHelper.h"

class CUnpackProcess
	 : public IZipUnpackProcessListener
{
	virtual BOOL OnZipUnpackProcessNotify(DOUBLE bProcess, IZipUnpack *pUnpack)
	{
		return TRUE;
	}
	virtual VOID OnZipUnpackFinished(BOOL bSuccess, IZipUnpack *pUnpack)
	{
		return;
	}
};

int _tmain(int argc, _TCHAR* argv[])
{
	printf("ttt");
	system("PAUSE");

	CStringW strA(L"|dafwf|dafw|dwafdwa||||dawfw||||");
	CStringW strB(L"dasfasf|dafadf|");
	CStringW strC(L"|dafwf|dafw|dwafdwa||||dawfw");
	CStringW strD(L"dasfasf|dafadf");
	CStringW strE(L"dasfasf||dafadf");

	std::vector<CStringW> vRst;

	XLibS::StringHelper::SplitStringW(strA, _T("|"), &vRst);
	XLibS::StringHelper::SplitStringW(strB, _T("|"), &vRst);
	XLibS::StringHelper::SplitStringW(strC, _T("|"), &vRst);
	XLibS::StringHelper::SplitStringW(strD, _T("|"), &vRst);
	XLibS::StringHelper::SplitStringW(strE, _T("|"), &vRst);

	argc++;



// 	CSMCPtr<IZipUnpack> Unpack;
// 
// 	Unpack.CreateInstance(SMCRelativePath(_T("XLib.dll")));
// 
// 	Unpack->Open(L"D:\\software\\EA80858\\EA80858.zip");
// 
// 	CUnpackProcess listener;
// 	Unpack->SetProcessListener(&listener);
// 
// 	BYTE * pBuffer = new BYTE [Unpack->GetFileSize(L"说明\\SN.txt")];
// 
// 	Unpack->UnpackFile(L"说明\\SN.txt", pBuffer, Unpack->GetFileSize(L"说明\\SN.txt"));
// 
// 	delete [] pBuffer;
// 
// 	Unpack->UnpackFile(L"Enterprise Architect 8.0.858.exe", L"D:\\test.ext", FALSE);
// 
// 	Unpack.Detach()->Release();
// 
//  	SMCReleaseAllDLL();

//	LONG a = _tcstol(_T(""), NULL, 10);

//	printf(typeid(0.0).name());

// 	CSMCPtr<IARPTableEntry> spARPTableEntry;
// 	spARPTableEntry.CreateInstance(SMCRelativeDLLPath(_T("XLib.dll")));
// 
// 	spARPTableEntry->SetIPAddr(_T("10.7.1.1"));
// 	UINT32 ip = spARPTableEntry->GetIPAddr();
// 
// 	spARPTableEntry->SetIPAddr(ip);
// 	spARPTableEntry->GetIPAddr();
// 
// 	spARPTableEntry->SetMACAddr(_T("8C-9-A5-3C-2B-D1"));
// 	UINT64 mac = spARPTableEntry->GetMACAddr();
// 	spARPTableEntry->SetMACAddr(mac);

// 
// 	CSMCPtr<ISNMP> spSNMP;
// 	spSNMP.CreateInstance(SMCRelativeDLLPath(_T("XLib.dll")));
// 
// 	spSNMP->Init(_T("111.11.85.3"), _T("public"), 5000, 1);
// 
//  CXArrayPtr<IMACTableEntry *> spMACTable;
//  spSNMP->QueryMACTable(&spMACTable);
//  
//  CXArrayPtr<IARPTableEntry *> spArpTable;
//  spSNMP->QueryArpTable(&spArpTable);
// 
// 	CXArrayPtr<IPortInfoEntry *> spPortInfo;
// 	spSNMP->QueryPortInfo(&spPortInfo);

	// CXArrayPtr<IPortInfoEntry *> spPortInfo;
	// LPCWSTR hostName = spSNMP->GetHostName();





// 	HMODULE hMod = ::LoadLibrary(L"E:\\MY\\XLib\\Debug\\dependence\\libmysql.dll");
// 	void *p = (void *)::GetProcAddress(hMod, "mysql_init");
// 	

// 	CSMCPtr<ISQLDB> spSQLDB;
// 	spSQLDB.CreateInstance(SMCRelativeDLLPath(_T("XLib.dll")));
// 
// 	spSQLDB->Init(_T("172.23.54.102"), _T("root"), _T("test"), _T("test") );
// 	spSQLDB->ExecuteSQL(_T("INSERT into TEST values(2, 2.2, 'abc', 'def blob')"), FALSE, NULL);
// 	spSQLDB->ExecuteSQL(_T("INSERT into TEST values(3, 2.2, 'abc', 'def blob')"), FALSE, NULL);
// 	spSQLDB->ExecuteSQL(_T("INSERT into TEST values(4, 2.2, 'abc', 'def blob')"), FALSE, NULL);
// 	spSQLDB->ExecuteSQL(_T("INSERT into TEST values(5, 2.2, 'abc', 'def blob')"), FALSE, NULL);
// 	spSQLDB->ExecuteSQL(_T("INSERT into TEST values(6, 2.2, 'abc', 'def blob')"), FALSE, NULL);
// 	spSQLDB->ExecuteSQL(_T("INSERT into TEST values(7, 2.2, 'abc', 'def blob')"), FALSE, NULL);
// 	spSQLDB->ExecuteSQL(_T("INSERT into TEST values(8, 2.2, 'abc', 'def blob')"), FALSE, NULL);
// 	spSQLDB->ExecuteSQL(_T("INSERT into TEST values(9, 2.2, 'abc', 'def blob')"), FALSE, NULL);

// 	BOOL bIsEmptySet = TRUE;
// 	spSQLDB->ExecuteSQL(_T("SELECT * FROM test;"), TRUE, &bIsEmptySet);
// 
// 	
// 
// 	if (!bIsEmptySet)
// 	{
// 		BOOL bEnd = FALSE;
// 		while(!bEnd)
// 		{
// 			DWORD a = spSQLDB->GetIntField(0);
// 			DOUBLE b = spSQLDB->GetDoubleField(1);
// 			LPCWSTR c = spSQLDB->GetStringField(2);
// 			UINT nLen = spSQLDB->GetBinaryFieldLen(3);
// 			LPBYTE p = spSQLDB->GetBinaryField(3);
// 
// 			CStringA t((char *)p, nLen);
// 			t.GetLength();
// 
// 			spSQLDB->GetNextRow(&bEnd);
// 		}
// 	}
// 
// 
// 	spSQLDB.Detach()->Release();

// 	CString str = _T("abcd");
// 	LPTSTR zz = str.GetBuffer(10);
// 
// 
// 	CSMCPtr<IConfig> spCofig;
// 	spCofig.CreateInstance(SMCRelativeDLLPath(_T("XLib.dll")));
// 
// 	spCofig->Init();
// 	spCofig->SetParam(_T("filepath"), _T("D:\\wamp\\bin\\mysql\\mysql5.5.20\\my.ini"));
// 	spCofig->Load();
// 
// 	for (UINT i = 0; i < spCofig->GetSectionCount(); i++)
// 	{
// 		CString strSectionName = spCofig->GetSectionName(i);
// 		UINT nIndex = spCofig->GetSectionIndexByName(strSectionName);
// 
// 		for (UINT j = 0; j < spCofig->GetSectionKeyCount(i); j++)
// 		{
// 			CString strKey = spCofig->GetSectionKey(i, j);
// 
// 			CString strValue = spCofig->GetStringValue(i, strKey);
// 			LONG nValue = spCofig->GetIntValue(i, strKey);
// 			DOUBLE dwValue = spCofig->GetDoubleValue(i, strKey);
// 		}
// 
// 	}
// 
// 	spCofig.Detach()->Release();
// 
// 	SMCReleaseDLL(SMCRelativeDLLPath(_T("XLib.dll")));


	return 0;
}
 
