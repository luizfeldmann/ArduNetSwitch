#include "HttpConstants.h"

/* General Constants */

const char c_strHttpBasic[] = "Basic";

/* Header Fields */

const char c_strHttpHeaderHost[] = "Host";

const char c_strHttpHeaderContentType[] = "Content-Type";

const char c_strHttpHeaderContentLength[] = "Content-Length";

const char c_strHttpHeaderWWWAuthenticate[] = "WWW-Authenticate";

const char c_strHttpHeaderAuthorization[] = "Authorization";

/* MIME Types */

const char c_strHttpTypeHtml[] = "text/html";

const char c_strHttpTypeWWWForm[] = "application/x-www-form-urlencoded";

/* Reason Messages */

const char c_strHttpReasonOK[] = "OK";

const char c_strHttpReasonUnauthorized[] = "Unauthorized";

const char c_strHttpReasonForbidden[] = "Forbidden";

const char c_strHttpReasonNotFound[] = "Not Found";

const char* HttpReasonFromCode(enum EHttpStatusCodes eCode)
{
    static const char* const sEmpty = "";
    const char* sReason = sEmpty;

    switch (eCode)
    {
    case HTTP_OK:
        sReason = c_strHttpReasonOK;
        break;

    case HTTP_UNAUTHORIZED:
        sReason = c_strHttpReasonUnauthorized;
        break;

    case HTTP_FORBIDDEN:
        sReason = c_strHttpReasonForbidden;
        break;

    case HTTP_NOTFOUND:
        sReason = c_strHttpReasonNotFound;
        break;
    }

    return sReason;
}

