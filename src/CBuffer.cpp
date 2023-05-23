#include "CBuffer.h"
#include <string.h>

CBuffer::CBuffer(char* pData, size_t uCapacity)
    : m_pData(pData)
    , m_pEnd(pData)
    , m_uCapacity(uCapacity)
{
    
}

char* CBuffer::begin()
{
    return m_pData;
}

const char* CBuffer::cbegin() const
{
    return m_pData;
}

char* CBuffer::end()
{
    return m_pEnd;
}

const char* CBuffer::cend() const
{
    return m_pEnd;
}

size_t CBuffer::size() const
{
    return m_pEnd - m_pData;
}

size_t CBuffer::capacity() const
{
    return m_uCapacity;
}

void CBuffer::clear()
{
    m_pEnd = m_pData;
}

bool CBuffer::empty() const
{
    return (m_pEnd == m_pData);
}

bool CBuffer::push_back(char c)
{
    if (size() + 1 <= capacity())
    {
        *m_pEnd = c;
        ++m_pEnd;
        return true;
    }

    return false;
}

bool CBuffer::push_back(const char* pInsert, size_t uCount)
{
    if (size() + uCount <= capacity())
    {
        memcpy(m_pEnd, pInsert, uCount);
        m_pEnd += uCount;

        return true;
    }

    return false;
}

bool CBuffer::push_back(Client& Src)
{
    const int nAvail = Src.available();

    if (size() + nAvail <= capacity())
    {
        m_pEnd += Src.read((uint8_t*)m_pData, nAvail);

        return true;
    }

    return false;
}