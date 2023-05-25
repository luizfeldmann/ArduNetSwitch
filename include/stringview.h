#ifndef _STRINGVIEW_H_
#define _STRINGVIEW_H_

#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

//! Compares a null-terminated C-string with a string-view
//! @param[in] szNullTerm A null-terminated string to be compared
//! @param[in] pStrView Pointer to the first character of the string-view
//! @param[in] uSvLen Size of the string-view
//! @return True if the strings are equal
bool stringview_cmp(const char* szNullTerm, const char* pStrView, size_t uSvLen);

//! Copies a string-view into the destination buffer as a null-terminated string
//! At most the specified capacity is copied, and the \p pDest is always null-terminated
//! @param[out] pDest Buffer to receive the null-terminated string
//! @param[in] uCapacity The sizeof(...) of \p pDest
//! @param[in] pStrView Pointer to the first character of the string-view
//! @param[in] uSvLen The length of the string-view
void stringview_copy(char* pDest, size_t uCapacity, const char* pStrView, size_t uSvLen);

#ifdef __cplusplus
}
#endif

#endif