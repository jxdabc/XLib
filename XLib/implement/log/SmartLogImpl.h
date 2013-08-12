#pragma once

#include "../../../inc/interface/log/SmartLog.h"
#include <vector>


class ISmartLogDestionation;
class CSmartLog : public ISmartLog
{
	SMC_DECLARE_INTERFACEIMPL_BEGIN(CSmartLog, SMC_MUTIINSTANCE)
		SMC_IMPLEMENT_INTERFACE(CSmartLog, ISmartLog)
	SMC_DECLARE_INTERFACEIMPL_END(CSmartLog)

public:
	STDMETHOD_(BOOL, SetLogDestination)(DWORD dwType, DWORD dwDestinations);

	STDMETHOD_(BOOL, SetLogTypeDescription)(DWORD dwType, LPCWSTR pDescription);

	STDMETHOD_(BOOL, SetLogParam)(LPCWSTR pKey, LPCWSTR pValue);

	STDMETHOD_(BOOL, Log)(DWORD dwType, LPCWSTR pLogStr);

public:
	CSmartLog();
	~CSmartLog();

private:
	// ( log type --> (destination, description) )
	std::map<DWORD, std::pair<DWORD, CString>> m_mapTypes;
	std::vector<ISmartLogDestionation *> m_vDestionations;
	SMCLock m_Lock;
};
