#include "stringview.h"

bool stringview_cmp(const char* szNullTerm, const char* pStrView, size_t uSvLen)
{
    const char* pEnd = pStrView + uSvLen;

    for (; *szNullTerm; ++szNullTerm, ++pStrView)
    {
        // The string-view is shorter than the null-terminated string
        if (pEnd == pStrView)
            return false;

        //! The characters are different
        if (*szNullTerm != *pStrView)
            return false;
    }

    // szNullTerm was \0 and pStrView was finished as well
    if (pStrView == pEnd)
        return true;

    // The null-terminated is shorter than the string-view
    return false;
}

void stringview_copy(char* pDest, size_t uCapacity, const char* pStrView, size_t uSvLen)
{
    for (size_t i = 0; (i < uSvLen) && (i < uCapacity - 1); ++pDest, ++pStrView, ++i)
        *pDest = *pStrView;

    *pDest = '\0';
}