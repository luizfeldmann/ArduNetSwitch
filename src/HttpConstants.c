#include "HttpConstants.h"

/* Header Fields */

const char c_strHttpHeaderContentType[] = "Content-Type";

const char c_strHttpHeaderContentLength[] = "Content-Length";

/* MIME Types */

const char c_strHttpTypeHtml[] = "text/html";

/* Reason Messages */

const char c_strHttpReasonOK[] = "OK";

const char* HttpReasonFromCode(enum EHttpStatusCodes eCode)
{
    static const char* const sEmpty = "";
    const char* sReason = sEmpty;

    switch (eCode)
    {
    case HTTP_OK:
        sReason = c_strHttpReasonOK;
        break;
    }

    return sReason;
}

