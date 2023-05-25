#ifndef _CHTTPRESPONSE_H_
#define _CHTTPRESPONSE_H_

#include <Print.h>
#include "HttpConstants.h"

//! HTTP Response 
class CHttpResponse
{
public:
    //! Response code
    enum EHttpStatusCodes m_eStatusCode;

    //! Hostname of server
    const char* m_sHost;

    //! Auth challenge
    const char* m_sAuthenticate;

    //! Content type
    const char* m_sContentType;

    //! Pointer to content
    const char* m_pContentStart;

    //! Length of body
    size_t m_uContentLength;

    CHttpResponse();

    //! Sends this response to the provided stream
    void Send(Print& Out);
};

#endif