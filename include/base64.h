#ifndef _BASE64_H_
#define _BASE64_H_

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

//! Decodes a Base-64 buffer
//! @param[in] pEncInput Input encoded buffer
//! @param[in] uInputLen Input buffer size
//! @param[out] pDecOutput Output decoded buffer
//! @param[in,out] puDecOutLen Pointer to the capacity of the output buffer, receives the actual decoded size
//! @return 0 on success
int base64_decode(const char* pEncInput, size_t uInputLen, char* pDecOutput, size_t* puDecOutLen);

#ifdef __cplusplus
}
#endif

#endif