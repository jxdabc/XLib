#pragma once

#define CONFIG_TYPE_INI (1)

#define INVALID_SECTION_INDEX ((UINT)-1)

SMC_DECLARE_INTERFACE(IConfig, {2DD52A86-001A-4878-BA5E-AE3ED67826D9})
	: public ISMCInterface
{
public:
	STDMETHOD_(BOOL, Init)(UINT nConfigType = CONFIG_TYPE_INI) PURE;

	STDMETHOD_(BOOL, SetParam)(LPCWSTR pKey, LPCWSTR pValue) PURE;

	STDMETHOD_(BOOL, Load)() PURE;

	STDMETHOD_(UINT, GetSectionCount)() PURE;

	STDMETHOD_(LPCWSTR, GetSectionName)(UINT nSectionIndex) PURE;

	// return INVALID_SECTION_INDEX if not found. 
	STDMETHOD_(UINT, GetSectionIndexByName)(LPCWSTR pName) PURE;

	STDMETHOD_(UINT, GetSectionKeyCount)(UINT nSectionIndex) PURE;

	STDMETHOD_(LPCWSTR, GetSectionKey)(UINT nSectionIndex, UINT nKeyIndex) PURE;

	STDMETHOD_(BOOL, IsSectionKeyExist)(UINT nSectionIndex, LPCWSTR pKey) PURE;

	STDMETHOD_(LONG, GetIntValue)(UINT nSectionIndex, LPCWSTR pKey) PURE;
	STDMETHOD_(DOUBLE, GetDoubleValue)(UINT nSectionIndex, LPCWSTR pKey) PURE;
	STDMETHOD_(LPCWSTR, GetStringValue)(UINT nSectionIndex, LPCWSTR pKey) PURE;
};
