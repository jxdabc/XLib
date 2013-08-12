/*
    Author   :  Jiao Xiaodong
    Create   :  2012-2-18
    Mail     :  milestonejxd@gmail.com

    Redistribution and use the source code, with or without modification,
    must retain the above copyright.
*/

#pragma once

#include "../inc/convention.h"
#include <tchar.h>
#include <list>
#include <map>
#include <set>


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//					Section 1 : Instance Type Supported


enum SMC_INSTANCE_TYPE
{
	SMC_MUTIINSTANCE,
	SMC_SINGLETON,
};

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//					Section 2 : Interface Manager

class SMC_INTERFACE_RUNTIME_CLASS;
typedef std::list<SMC_INTERFACE_RUNTIME_CLASS*> TYPE_INTERFACELIST;
typedef std::map<SMC_INTERFACE_RUNTIME_CLASS*, ISMCInterface*> TYPE_INSTANCEMAP;

class SMCInterfaceMgr
{
public:
	VOID REGISTER_INTERFACE(SMC_INTERFACE_RUNTIME_CLASS*);
	BOOL GET_INTERFACE_INSTANCE(SMC_INTERFACE_ID, ISMCInterface **);
	VOID ON_INSTANCE_DESTROYED(SMC_INTERFACE_RUNTIME_CLASS*);
	VOID FINAL_DESTROY();

public:
	static SMCInterfaceMgr & GET_INSTANCE();

private:
	TYPE_INTERFACELIST		INTERFACELIST;
	TYPE_INSTANCEMAP		INSTANCEMAP;

private:
	SMCLock	M_LOCK;
};

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//					Section 3 : Debug Leak Detector

typedef std::set<ISMCInterface*> InstanceCountCheckSet;
class INSTANCE_COUNT_CHECK
{
public:
	VOID OnInterfaceCreate(VOID *);
	VOID OnInterfaceDestroy(VOID *);
	BOOL Check(InstanceCountCheckSet *);

public:
	static INSTANCE_COUNT_CHECK & GetInstance();

private:
	InstanceCountCheckSet m_setCountCheck;
	SMCLock	m_lock;
};


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//					Section 4 : Interface Implementaion Macros

class SMC_INTERFACE_RUNTIME_CLASS
{
public:
	virtual BOOL Query(SMC_INTERFACE_ID, ISMCInterface **) PURE;
	virtual SMC_INSTANCE_TYPE GetInstanceType() PURE;
	virtual TCHAR* GetImplementClassName() PURE;
};

#define __SMC_INTERFACE_RUNTIMEINFO_BEGIN(classname, instancetype)							\
	public:																					\
		class INTERFACE_RUNTIME_CLASS_##classname											\
			: public SMC_INTERFACE_RUNTIME_CLASS											\
		{																					\
		public:																				\
			INTERFACE_RUNTIME_CLASS_##classname()											\
				: m_szClassName(_T(#classname))												\
				, m_eInstanceType(SMC_MUTIINSTANCE)											\
			{																				\
				m_eInstanceType = instancetype;												\
				SMCInterfaceMgr::GET_INSTANCE().REGISTER_INTERFACE(this);					\
			}																				\
		public:																				\
			virtual SMC_INSTANCE_TYPE GetInstanceType()										\
			{																				\
				return m_eInstanceType;														\
			}																				\
			virtual TCHAR* GetImplementClassName()											\
			{																				\
				return m_szClassName;														\
			}																				\
		private:																			\
			TCHAR *m_szClassName;															\
			SMC_INSTANCE_TYPE m_eInstanceType;												\
		public:																				\
			virtual BOOL Query(SMC_INTERFACE_ID id, ISMCInterface **p)						\
			{																				\
				if (!p)																		\
					return FALSE;															\
				if (FALSE) {}


#define __SMC_INTERFACE_RUNTIMEINFO_END(classname)											\
				return FALSE;																\
			}																				\
		};																					\
		static INTERFACE_RUNTIME_CLASS_##classname INTERFACE_RUNTIME_INFO;					\
	public:																					\
		virtual SMC_INTERFACE_RUNTIME_CLASS* GetInterfaceRuntimeInfo()						\
		{																					\
			return &classname::INTERFACE_RUNTIME_INFO;										\
		}

#define __SMC_INTERFACE_REFCOUNT(classname)													\
	public:																					\
		STDMETHOD_(UINT, GetRefCount)()														\
		{																					\
			SMCAutoLock lock(&m_InstanceLock);												\
			return m_nRefCount;																\
		}																					\
		STDMETHOD_(UINT, AddRef)()															\
		{																					\
			SMCAutoLock lock(&m_InstanceLock);												\
			return ++m_nRefCount;															\
		}																					\
		STDMETHOD_(VOID, Release)()															\
		{																					\
			m_InstanceLock.Lock();															\
			UINT nRefCount = --m_nRefCount;													\
			m_InstanceLock.Unlock();														\
			if (!nRefCount)																	\
			{																				\
				SMC_INTERFACE_RUNTIME_CLASS *pRuntimeInfo									\
					= this->GetInterfaceRuntimeInfo();										\
				if (pRuntimeInfo)															\
					SMCInterfaceMgr::GET_INSTANCE().ON_INSTANCE_DESTROYED(pRuntimeInfo);	\
				else																		\
					assert(NULL);															\
				delete this;																\
			}																				\
		}																					\
		STDMETHOD_(VOID, AllRelease)()														\
		{																					\
			while(GetRefCount() > 1)														\
				Release();																	\
			Release();																		\
		}																					\
private:																					\
		UINT m_nRefCount;																	\
		SMCLock m_InstanceLock;																\
public:																						\
		void* operator new(size_t size)														\
		{																			     	\
			classname *pThis = (classname *)::operator new(size);							\
			pThis->m_nRefCount = 1;															\
			INSTANCE_COUNT_CHECK::GetInstance().OnInterfaceCreate(pThis);					\
			return pThis;																	\
		}																					\
		void operator delete(void *pThis)													\
		{																			     	\
			INSTANCE_COUNT_CHECK::GetInstance().OnInterfaceDestroy(pThis);					\
			::operator delete(pThis);														\
		}

#define __SMC_INTERFACE_QUERYINTERFACE()													\
		STDMETHOD_(BOOL, QueryInterface)(SMC_INTERFACE_ID id, ISMCInterface **p)			\
		{																					\
			SMC_INTERFACE_RUNTIME_CLASS *pRuntimeInfo =										\
				this->GetInterfaceRuntimeInfo();											\
			if (!pRuntimeInfo)																\
				return FALSE;																\
			ISMCInterface *pInstance = (ISMCInterface *)(void *)this;						\
			if (pRuntimeInfo->Query(id, &pInstance))										\
			{																				\
				pInstance->AddRef();														\
				*p = pInstance;																\
				return TRUE;																\
			}																				\
			return FALSE;																	\
		}


#define SMC_DECLARE_INTERFACEIMPL_BEGIN(classname, instancetype)							\
	__SMC_INTERFACE_REFCOUNT(classname)														\
	__SMC_INTERFACE_RUNTIMEINFO_BEGIN(classname, instancetype)

#define SMC_DECLARE_INTERFACEIMPL_BEGIN_NO_REFCOUNT_IMPLEMENT(classname, instancetype)		\
	__SMC_INTERFACE_RUNTIMEINFO_BEGIN(classname, instancetype)

#define SMC_DECLARE_INTERFACEIMPL_END(classname)											\
	__SMC_INTERFACE_RUNTIMEINFO_END(classname)												\
	__SMC_INTERFACE_QUERYINTERFACE()

#define SMC_IMPLEMENT_INTERFACE(classname, intf)											\
				else if(SMC_IS_IID_EQUAL(SMC_IDOF(intf), id))								\
				{																			\
					classname *pInstance = (classname *)(void *)*p;							\
					if (!pInstance)															\
						pInstance = new classname();										\
					*p = (intf *)pInstance;													\
					return TRUE;															\
				}


#define SMC_IMPLEMENT_INTERFACEIMPL(classname)												\
	classname::INTERFACE_RUNTIME_CLASS_##classname classname::INTERFACE_RUNTIME_INFO;

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//					Section 4 : Local Interface Helper

template <class T>
BOOL SMCCreateLocalInterfaceInstance(T **p)
{
	if (!p)
		return FALSE;

	return SMCInterfaceMgr::GET_INSTANCE().GET_INTERFACE_INSTANCE(SMC_IDOF(T), (ISMCInterface **)p);
}