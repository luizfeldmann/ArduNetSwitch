#ifndef _CSTRINGVIEW_H_
#define _CSTRINGVIEW_H_

#include <stddef.h>

//! Describes a range for 'view' of a string stored in a larger buffer
class CStringView
{
private:
    //! Pointer to first character
    const char* m_pStart;

    //! Number of bytes
    const size_t m_uSize;
public:

    CStringView(const char* pFirst, size_t uSize);
    CStringView(const char* pFirst, const char* pEnd);

    //! Iterator to begin
    const char* cbegin() const;

    //! Iterator to end
    const char* cend() const;
    
    //! Reads the size of the string
    size_t size() const;
    
    //! Checks if empty
    bool empty() const;
};

#endif