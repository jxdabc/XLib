#pragma once

SMC_DECLARE_INTERFACE(IXArray, {C4431464-2312-497b-A8EC-3FA49F3125A6})
	: public ISMCInterface
{
public:
	STDMETHOD_(ISMCInterface * &, operator[])(UINT nIndex) PURE;
	
	STDMETHOD_(UINT, size)() PURE;
	
	STDMETHOD_(UINT, insert)(UINT nWhere, ISMCInterface * const & pItem) PURE;
	STDMETHOD_(VOID, push_back)(ISMCInterface * const & pItem) PURE;
	
	STDMETHOD_(UINT, erase)(UINT nWhere) PURE;
};


template <class T>
class CXArrayPtr :
	public CSMCPtr<IXArray>
{
public:
	CXArrayPtr() throw()
	{
	}
	CXArrayPtr(int nNull) throw()
		: CSMCPtr<IXArray>(nNull)
	{
	}
	CXArrayPtr(T* lp) throw()
		: CSMCPtr<IXArray>(lp)
	{
	}
	T & operator[] (UINT nIndex)
	{
		return (T &) (*this)->operator[](nIndex);
	}
};

