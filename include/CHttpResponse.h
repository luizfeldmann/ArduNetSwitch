#ifndef _CHTTPRESPONSE_H_
#define _CHTTPRESPONSE_H_

#include <Print.h>
#include "HttpConstants.h"

//! HTTP Response 
class CHttpResponse
{
public:
    static void Send(Print& Out, enum EHttpStatusCodes eStatusCode,
        const char* sHost, 
        const char* sContentType, const char* pContentStart, size_t uContentLength);
};

#endif