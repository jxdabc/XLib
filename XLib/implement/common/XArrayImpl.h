#pragma once

#include "../../../inc/interface/common/XArray.h"

#include <vector>

class CXArray : public IXArray
{
	SMC_DECLARE_INTERFACEIMPL_BEGIN(CXArray, SMC_MUTIINSTANCE)
		SMC_IMPLEMENT_INTERFACE(CXArray, IXArray)
	SMC_DECLARE_INTERFACEIMPL_END(CXArray)

public:
	STDMETHOD_(ISMCInterface * &, operator[])(UINT nIndex);

	STDMETHOD_(UINT, size)();

	STDMETHOD_(UINT, insert)(UINT nWhere, ISMCInterface * const & pItem);
	STDMETHOD_(VOID, push_back)(ISMCInterface * const & pItem);

	STDMETHOD_(UINT, erase)(UINT nWhere);

public:
	~CXArray();

private:
	std::vector<ISMCInterface *> m_Vector;
};
