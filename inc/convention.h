/*
    Author   :  Jiao Xiaodong
    Create   :  2012-2-18
    Mail     :  milestonejxd@gmail.com

    Redistribution and use the source code, with or without modification,
    must retain the above copyright.
*/

#pragma once

#include <windows.h>
#include <basetyps.h>
#include <atlstr.h>

#include <map>

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//					Section 0.0 : Simple Thread Lock

// Simple thread lock by critical sections. 
class SMCLock
{
public:
	VOID Lock()
	{
		EnterCriticalSection(&m_CritialSection);
	}
	VOID Unlock()
	{
		LeaveCriticalSection(&m_CritialSection);
	}
public:
	SMCLock()
	{
		InitializeCriticalSection(&m_CritialSection);
	}
	~SMCLock()
	{
		DeleteCriticalSection(&m_CritialSection);
	}
private:
	CRITICAL_SECTION m_CritialSection;
};

// Lock guard. 
class SMCAutoLock
{
public:
	SMCAutoLock(SMCLock *pLock)
	{
		m_pLock = pLock;
		if (pLock)
			pLock->Lock();
	}
	~SMCAutoLock()
	{
		if (m_pLock)
			m_pLock->Unlock();
		m_pLock = NULL;
	}
private:
	SMCLock *m_pLock;
};

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//					Section 0.1 : Thread Safe Singleton

#define SMC_THREAD_SAFE_SINGLETON(type, funcname)										\
	type & funcname()																	\
	{																					\
		__SMC_THREAD_SAFE_SINGLETON_BODY(type)											\
	}																					\
	static type & __SMC_THREAD_SAFE_SINGLETON_INSTANCE##type##funcname					\
		= funcname();

#define SMC_THREAD_SAFE_SINGLETON_INLINE(type, funcname)								\
	inline type & funcname()															\
	{																					\
		__SMC_THREAD_SAFE_SINGLETON_BODY(type)											\
	}																					\
	__declspec(selectany) type & __SMC_THREAD_SAFE_SINGLETON_INSTANCE##type##funcname	\
		= funcname();

#define SMC_THREAD_SAFE_SINGLETON_STATIC(type, funcname)								\
	static type & funcname()															\
	{																					\
		__SMC_THREAD_SAFE_SINGLETON_BODY(type)											\
	}																					\
	static type & __SMC_THREAD_SAFE_SINGLETON_INSTANCE##type##funcname					\
		= funcname();
	
#define SMC_THREAD_SAFE_SINGLETON_CLASS_METHOD(type, classname, funcname)				\
	type & classname::funcname()														\
	{																					\
		__SMC_THREAD_SAFE_SINGLETON_BODY(type)											\
	}																					\
	static type & __SMC_THREAD_SAFE_SINGLETON_INSTANCE##type##funcname##classname		\
		= classname::funcname();
	

#define __SMC_THREAD_SAFE_SINGLETON_BODY(type)				\
		static type instance;								\
		return instance;

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//					Section 1 : Common Macros


#define SMC_NO_VTABLE						__declspec(novtable)
#define SMC_EXPORT_FUNCTION					STDMETHODCALLTYPE
#define SMC_CORE_INTERFACE					SMC_EXPORT_FUNCTION

#define SMC_DECLARE_INTERFACE(name, ID)		class SMC_NO_VTABLE __declspec(uuid(#ID)) name
#define SMC_IDOF(interface)					__uuidof(interface)

typedef GUID SMC_INTERFACE_ID;
#define SMC_IS_IID_EQUAL(a, b)				IsEqualGUID(a, b)


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//					Section 2 : Base Interface


class SMC_INTERFACE_RUNTIME_CLASS;
class SMC_NO_VTABLE ISMCInterface
{
public:
	STDMETHOD_(BOOL, QueryInterface)(SMC_INTERFACE_ID, ISMCInterface **) PURE;

public:
	STDMETHOD_(UINT, AddRef)() PURE;
	STDMETHOD_(VOID, Release)() PURE;
	STDMETHOD_(VOID, AllRelease)() PURE;
	STDMETHOD_(UINT, GetRefCount)() PURE;

public:
	virtual SMC_INTERFACE_RUNTIME_CLASS* GetInterfaceRuntimeInfo() PURE;

public:
	ISMCInterface();
	virtual ~ISMCInterface();
};


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//					Section 3 : Create Instance Implementation

typedef class _TYPESMCLoadedDLLMap 
	: public std::map<CString, HMODULE>
{
public:
	~_TYPESMCLoadedDLLMap()
	{
		iterator it = begin();
		for(;it != end(); it++)
		{
			::FreeLibrary(it->second);
		}
	}
} TYPESMCLoadedDLLMap;

SMC_THREAD_SAFE_SINGLETON_INLINE(SMCLock, __GET_CREATE_INSTANCE_GLOBAL_LOCK)
SMC_THREAD_SAFE_SINGLETON_INLINE(TYPESMCLoadedDLLMap, __GET_CREATE_INSTANCE_LOADED_DLL_MAP)



typedef BOOL (SMC_EXPORT_FUNCTION *TYPESMCCreateInstanceImpl)(SMC_INTERFACE_ID, ISMCInterface **);
typedef VOID (SMC_EXPORT_FUNCTION *TYPESMCReleaseDLLImpl)();

inline CStringW SMCRelativePathW(LPCWSTR szRelativePath, HMODULE hModule = NULL)
{
	WCHAR szPath[MAX_PATH] = {0};
	::GetModuleFileNameW(hModule, szPath, MAX_PATH);
	::PathRemoveFileSpecW(szPath);
	::PathAppendW(szPath, szRelativePath);

	return szPath;
}
inline CStringA SMCRelativePathA(LPCSTR szRelativePath, HMODULE hModule = NULL)
{
	CHAR szPath[MAX_PATH] = {0};
	::GetModuleFileNameA(hModule, szPath, MAX_PATH);
	::PathRemoveFileSpecA(szPath);
	::PathAppendA(szPath, szRelativePath);

	return szPath;
}
#ifdef _UNICODE
#define	SMCRelativePath SMCRelativePathW
#else
#define SMCRelativePath SMCRelativePathA
#endif

inline BOOL SMCCreateInstance(LPCTSTR szDLLPath, SMC_INTERFACE_ID id, ISMCInterface **p)
{
	SMCAutoLock lock(&__GET_CREATE_INSTANCE_GLOBAL_LOCK());

	CString strDllPath(szDLLPath);
	strDllPath.MakeLower();

	HMODULE hDLL = NULL;
	TYPESMCLoadedDLLMap::iterator it = __GET_CREATE_INSTANCE_LOADED_DLL_MAP().find(strDllPath);
	if (it == __GET_CREATE_INSTANCE_LOADED_DLL_MAP().end())
	{
		hDLL = ::LoadLibrary(strDllPath);
		if (hDLL)
			__GET_CREATE_INSTANCE_LOADED_DLL_MAP().insert(std::make_pair(strDllPath, hDLL));
	}
	else
		hDLL = it->second;

	if (!hDLL)
		return FALSE;

	TYPESMCCreateInstanceImpl funSMCCreateInstanceImpl = 
		(TYPESMCCreateInstanceImpl)::GetProcAddress(hDLL, "SMCCreateInstanceImpl");
	if (!funSMCCreateInstanceImpl)
		return FALSE;

	return funSMCCreateInstanceImpl(id, p);
}

inline VOID SMCReleaseDLL(LPCTSTR szDLLPath)
{
	SMCAutoLock lock(&__GET_CREATE_INSTANCE_GLOBAL_LOCK());

	CString strDllPath(szDLLPath);
	strDllPath.MakeLower();

	TYPESMCLoadedDLLMap::iterator it = __GET_CREATE_INSTANCE_LOADED_DLL_MAP().find(strDllPath);

	if (it == __GET_CREATE_INSTANCE_LOADED_DLL_MAP().end())
		return;

	TYPESMCReleaseDLLImpl funSMCReleaseDLLImpl =
		(TYPESMCReleaseDLLImpl)::GetProcAddress(it->second, "SMCReleaseDLLImpl");
	if (!funSMCReleaseDLLImpl)
		return;

	funSMCReleaseDLLImpl();

	FreeLibrary(it->second);
	__GET_CREATE_INSTANCE_LOADED_DLL_MAP().erase(it);
}

inline VOID SMCReleaseAllDLL()
{
	SMCAutoLock lock(&__GET_CREATE_INSTANCE_GLOBAL_LOCK());

	TYPESMCLoadedDLLMap::iterator it = __GET_CREATE_INSTANCE_LOADED_DLL_MAP().begin();

	while (it != __GET_CREATE_INSTANCE_LOADED_DLL_MAP().end())
	{
		TYPESMCReleaseDLLImpl funSMCReleaseDLLImpl =
			(TYPESMCReleaseDLLImpl)::GetProcAddress(it->second, "SMCReleaseDLLImpl");
		if (!funSMCReleaseDLLImpl)
			return;

		funSMCReleaseDLLImpl();

		FreeLibrary(it->second);

		it++;
	}
	
	__GET_CREATE_INSTANCE_LOADED_DLL_MAP().clear();
}

#include "SMCPtr.inl"