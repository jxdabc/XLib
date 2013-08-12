#pragma once

#include "../../../inc/interface/config/Config.h"
#include <vector>

class IConfigImplement;
class CConfig : public IConfig
{
	SMC_DECLARE_INTERFACEIMPL_BEGIN(CConfig, SMC_MUTIINSTANCE)
		SMC_IMPLEMENT_INTERFACE(CConfig, IConfig)
	SMC_DECLARE_INTERFACEIMPL_END(CConfig)

public:
	STDMETHOD_(BOOL, Init)(UINT nConfigType = CONFIG_TYPE_INI);

	STDMETHOD_(BOOL, SetParam)(LPCWSTR pKey, LPCWSTR pValue);

	STDMETHOD_(BOOL, Load)();

	STDMETHOD_(UINT, GetSectionCount)();

	STDMETHOD_(LPCWSTR, GetSectionName)(UINT nSectionIndex);

	// return INVALID_SECTION_INDEX if not found. 
	STDMETHOD_(UINT, GetSectionIndexByName)(LPCWSTR pName);

	STDMETHOD_(UINT, GetSectionKeyCount)(UINT nSectionIndex);

	STDMETHOD_(LPCWSTR, GetSectionKey)(UINT nSectionIndex, UINT nKeyIndex);

	STDMETHOD_(BOOL, IsSectionKeyExist)(UINT nSectionIndex, LPCWSTR pKey);

	STDMETHOD_(LONG, GetIntValue)(UINT nSectionIndex, LPCWSTR pKey);
	STDMETHOD_(DOUBLE, GetDoubleValue)(UINT nSectionIndex, LPCWSTR pKey);
	STDMETHOD_(LPCWSTR, GetStringValue)(UINT nSectionIndex, LPCWSTR pKey);

public:
	CConfig();
	~CConfig();

private:
	IConfigImplement *m_pImplement;

private:
	CStringW m_strGetSectionNameSectionNameBuffer;
	CStringW m_strGetSectionKeySectionKeyBuffer;
	CStringW m_strGetStringValueValueBuffer;
};
