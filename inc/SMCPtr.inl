#pragma once

#include <assert.h>

inline __declspec(nothrow) ISMCInterface* __stdcall SMCPtrAssign(ISMCInterface** pp, ISMCInterface *lp)
{
	if (pp == NULL)
		return NULL;

	if (lp != NULL)
		lp->AddRef();
	if (*pp)
		(*pp)->Release();
	*pp = lp;
	return lp;
}

inline __declspec(nothrow) ISMCInterface* __stdcall SMCQIPtrAssign(ISMCInterface** pp, SMC_INTERFACE_ID id, ISMCInterface *lp)
{
	if (pp == NULL)
		return NULL;

	ISMCInterface* pTemp = *pp;
	*pp = NULL;
	if (lp != NULL)
		lp->QueryInterface(id, (ISMCInterface**)pp);
	if (pTemp)
		pTemp->Release();
	return *pp;
}

template <class T>
class _NoAddRefReleaseOnCSMCPtr : public T
{
private:
	STDMETHOD_(UINT, AddRef)() PURE;
	STDMETHOD_(VOID, Release)() PURE;
	STDMETHOD_(VOID, AllRelease)() PURE;
};

template<class T>
class CSMCPtrBase
{
protected:
	CSMCPtrBase() throw()
	{
		m_p	= NULL;
	}
	CSMCPtrBase(int nNull) throw()
	{
		assert(nNull == 0);
		(void)nNull;
		m_p = NULL;
	}
	CSMCPtrBase(T* lp) throw()
	{
		m_p = lp;
		if (lp != NULL)
			lp->AddRef();
	}

public:
	~CSMCPtrBase() throw()
	{
		if (m_p)
			m_p->Release();
	}
	operator T*() const throw()
	{ 
		return m_p; 
	}
	T& operator*() const
	{
		assert(m_p != NULL);
		return *m_p;
	}
	T** operator&()
	{
		assert(m_p == NULL);
		Release();
		m_p = NULL;
		return &m_p;
	}

	_NoAddRefReleaseOnCSMCPtr<T>* operator->() const throw()
	{ 
		assert(m_p != NULL);
		return (_NoAddRefReleaseOnCSMCPtr<T>*)m_p;
	}

	bool operator!() const throw()
	{
		return (m_p == NULL);
	}
	bool operator!=(T* pT) const
	{
		return !operator==(pT);
	}
	bool operator==(T* pT) const throw()
	{
		return m_p == pT;
	}
	void Release()
	{
		T* pTemp = m_p;
		if (pTemp)
		{
			m_p = NULL;
			pTemp->Release();
		}
	}
	void Attach(T* p) throw()
	{
		Release();
		m_p = p;
	}

	T* Detach() throw()
	{
		T* const old = m_p;
		m_p = NULL;
		return old;
	}
	bool CreateInstance(LPCTSTR szDLLPath)
	{
		assert(m_p == NULL);
		Release();

		return SMCCreateInstance(szDLLPath, SMC_IDOF(T), (ISMCInterface **)&m_p);
	}
	T* operator=(T* p)
	{
		Attach(p);
		if (m_p != NULL)
			m_p->AddRef();

		return *this;
	}

	T* m_p;
};

template <class T>
class CSMCPtr : public CSMCPtrBase<T>
{
public:
	CSMCPtr() throw()
	{
	}
	CSMCPtr(int nNull) throw()
		: CSMCPtrBase<T>(nNull)
	{
	}
	CSMCPtr(T* lp) throw()
		: CSMCPtrBase<T>(lp)
	{
	}
	CSMCPtr(const CSMCPtr<T>& lp) throw() 
		: CSMCPtrBase<T>(lp.m_p)
	{
	}
	T* operator=(T* lp) throw()
	{
		if (*this != lp)
		{
			return static_cast<T*>((ISMCInterface **)SMCPtrAssign(&m_p, lp));
		}
		return *this;
	}
	T* operator=(const CSMCPtr<T>& lp) throw()
	{
		if (*this != lp)
		{
			return static_cast<T*>(SMCPtrAssign((ISMCInterface **)&m_p, lp));
		}
		return *this;
	}
};

 template <class T>
 class CSMCQIPtr : public CSMCPtr<T>
 {
 public:
 	CSMCQIPtr() throw()
 	{
 	}
 	CSMCQIPtr(T* lp) throw() :
 	CSMCPtr<T>(lp)
 	{
 	}
 	CSMCQIPtr(const CSMCQIPtr<T>& lp) throw() :
 	CSMCPtr<T>(lp.m_p)
 	{
 	}
 	CSMCQIPtr(ISMCInterface* lp) throw()
 	{
 		if (lp != NULL)
			lp->QueryInterface(SMC_IDOF(T), (ISMCInterface **)&m_p);
 	}
 	T* operator=(T* lp) throw()
 	{
 		if(*this != lp)
 			return static_cast<T*>(SMCPtrAssign((ISMCInterface **)&m_p, lp));

 		return *this;
 	}
 	T* operator=(const CSMCQIPtr<T>& lp) throw()
 	{
 		if(*this != lp)
 			return static_cast<T*>(SMCPtrAssign((ISMCInterface **)&m_p, lp.m_p));

 		return *this;
 	}

 	T* operator=(ISMCInterface* lp) throw()
 	{
 		if( *this != lp )
 			return static_cast<T*>(SMCQIPtrAssign((ISMCInterface **)&m_p, SMC_IDOF(T), lp));

 		return *this;
 	}
 };