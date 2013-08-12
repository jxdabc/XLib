#include "stdafx.h"
#include <assert.h>
#include <objbase.h>
#include <set>

#include "interface.h"

VOID INSTANCE_COUNT_CHECK::OnInterfaceCreate(VOID *pInstance)
{
#ifdef _DEBUG
	SMCAutoLock lock(&m_lock);
	m_setCountCheck.insert((ISMCInterface *)pInstance);
#endif
}
VOID INSTANCE_COUNT_CHECK::OnInterfaceDestroy(VOID *pInstance)
{
#ifdef _DEBUG
	SMCAutoLock lock(&m_lock);
	m_setCountCheck.erase((ISMCInterface *)pInstance);
#endif
}
BOOL INSTANCE_COUNT_CHECK::Check(InstanceCountCheckSet *pSet)
{
#ifdef _DEBUG
	SMCAutoLock lock(&m_lock);
	if (m_setCountCheck.empty())
		return TRUE;

	if (pSet)
		*pSet = m_setCountCheck;
	return FALSE;
#else
	return TRUE;
#endif
}

SMC_THREAD_SAFE_SINGLETON_CLASS_METHOD(INSTANCE_COUNT_CHECK, INSTANCE_COUNT_CHECK, GetInstance)

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

VOID SMCInterfaceMgr::REGISTER_INTERFACE(SMC_INTERFACE_RUNTIME_CLASS* pRuntimeInfo)
{
	SMCAutoLock lock(&M_LOCK);

	if (!pRuntimeInfo)
		return;

	INTERFACELIST.push_back(pRuntimeInfo);
}

VOID SMCInterfaceMgr::ON_INSTANCE_DESTROYED(SMC_INTERFACE_RUNTIME_CLASS* pRuntimeInfo)
{
	SMCAutoLock lock(&M_LOCK);

	INSTANCEMAP.erase(pRuntimeInfo);
}

BOOL SMCInterfaceMgr::GET_INTERFACE_INSTANCE(SMC_INTERFACE_ID id, ISMCInterface **p)
{
	SMCAutoLock lock(&M_LOCK);

	if (!p)
		return FALSE;

	*p = NULL;

	TYPE_INTERFACELIST::iterator itRuntimeInfo = 
		INTERFACELIST.begin();
	for (;itRuntimeInfo != INTERFACELIST.end(); itRuntimeInfo++)
	{
		if ((*itRuntimeInfo) == NULL)
		{
			assert(NULL);
			continue;
		}

		if ((*itRuntimeInfo)->GetInstanceType() == SMC_SINGLETON)
		{
			TYPE_INSTANCEMAP::iterator itInstance =
				INSTANCEMAP.find(*itRuntimeInfo);
			if (itInstance != INSTANCEMAP.end())
			{
				if (itInstance->second && itInstance->second->QueryInterface(id, p))
					return TRUE;
			}
			else
			{
				if ((*itRuntimeInfo)->Query(id, p))
				{
					(*p)->AddRef(); // 防止抖动
					INSTANCEMAP.insert(std::make_pair(*itRuntimeInfo, *p));
					return TRUE;
				}
			}
		}
		else if ((*itRuntimeInfo)->GetInstanceType() == SMC_MUTIINSTANCE)
		{
			if ((*itRuntimeInfo)->Query(id, p))
				return TRUE;
		}
	}

	return FALSE;
}

VOID SMCInterfaceMgr::FINAL_DESTROY()
{
	SMCAutoLock lock(&M_LOCK);

	TYPE_INSTANCEMAP::iterator itInstance;
	while(!INSTANCEMAP.empty())
	{
		itInstance = INSTANCEMAP.begin();
		if (itInstance->second->GetRefCount() != 1)
			assert(!_T("销毁时还有没有释放的接口实例！"));
		itInstance->second->AllRelease();
	}

#ifdef _DEBUG
	InstanceCountCheckSet setUnreleasedInstance;
	if (!INSTANCE_COUNT_CHECK::GetInstance().Check(&setUnreleasedInstance))
	{
		InstanceCountCheckSet::iterator it =
			setUnreleasedInstance.begin();
		for (;it != setUnreleasedInstance.end();)
		{
			if (!*it)
			{
				assert(NULL);
				continue;
			}

			SMC_INTERFACE_RUNTIME_CLASS *pRuntimeInfo = (*it)->GetInterfaceRuntimeInfo();
			if (pRuntimeInfo)
			{
				TCHAR szAssertMsg[1024] = {0};
#ifdef _UNICODE
				swprintf(szAssertMsg, _T("没有释放的接口实例：%s, 计数：%i, 地址：%X"),
						 pRuntimeInfo->GetImplementClassName(), (*it)->GetRefCount(), *it);
#else
				sprintf(szAssertMsg, _T("没有释放的接口实例: %s, 计数: %i, 地址： %X"),
					pRuntimeInfo->GetImplementClassName(), (*it)->GetRefCount(), *it);
#endif
				
				assert(!szAssertMsg);
			}
			else
			{
				assert(NULL);
			}
			(*it)->AllRelease();
			it = setUnreleasedInstance.erase(it);
		}
	}
#endif
}

SMC_THREAD_SAFE_SINGLETON_CLASS_METHOD(SMCInterfaceMgr, SMCInterfaceMgr, GET_INSTANCE)

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

ISMCInterface::ISMCInterface()
{
}

ISMCInterface::~ISMCInterface()
{
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

BOOL SMC_EXPORT_FUNCTION SMCCreateInstanceImpl(SMC_INTERFACE_ID id, ISMCInterface **p)
{
	return SMCInterfaceMgr::GET_INSTANCE().GET_INTERFACE_INSTANCE(id, p);
}

VOID SMC_EXPORT_FUNCTION SMCReleaseDLLImpl()
{
	SMCInterfaceMgr::GET_INSTANCE().FINAL_DESTROY();
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////