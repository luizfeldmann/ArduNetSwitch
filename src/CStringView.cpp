#include "CStringView.h"

CStringView::CStringView(const char* pFirst, size_t uSize)
    : m_pStart(pFirst)
    , m_uSize(uSize)
{

}

CStringView::CStringView(const char* pFirst, const char* pEnd)
    : m_pStart(pFirst)
    , m_uSize(pEnd - pFirst)
{

}

const char* CStringView::cbegin() const
{
    return m_pStart;
}

const char* CStringView::cend() const
{
    return m_pStart + m_uSize;
}

size_t CStringView::size() const
{
    return m_uSize;
}

bool CStringView::empty() const
{
    return (0 == m_uSize);
}