#include "stdafx.h"

#include "XArrayImpl.h"

SMC_IMPLEMENT_INTERFACEIMPL(CXArray)

ISMCInterface * & CXArray::operator[](UINT nIndex)
{
	return m_Vector[nIndex];
}

UINT CXArray::size()
{
	return m_Vector.size();
}

UINT CXArray::insert(UINT nWhere, ISMCInterface * const & pItem)
{
	if (pItem)
		pItem->AddRef();

	return m_Vector.insert(m_Vector.begin() + nWhere, pItem) - m_Vector.begin();
}

VOID CXArray::push_back(ISMCInterface * const & pItem)
{
	if (pItem)
		pItem->AddRef();

	m_Vector.push_back(pItem);
}

UINT CXArray::erase(UINT nWhere)
{
	if (nWhere < m_Vector.size())
	{
		ISMCInterface * const & pItem =
			m_Vector[nWhere];

		if (pItem)
			pItem->Release();
	}

	return m_Vector.erase(m_Vector.begin() + nWhere) - m_Vector.begin();
}

CXArray::~CXArray()
{
	for (std::vector<ISMCInterface *>::size_type i = 0; i < m_Vector.size(); i++)
	{
		ISMCInterface * const & pItem = m_Vector[i];
		if (pItem)
			pItem->Release();
	}

	m_Vector.clear();
}
