#ifndef _HTTPCONSTANTS_H_
#define _HTTPCONSTANTS_H_

#ifdef __cplusplus
extern "C" {
#endif

//! HTTP status codes
enum EHttpStatusCodes
{
    HTTP_OK             = 200,
    HTTP_FOUND          = 302,
    HTTP_BADREQUEST     = 400,
    HTTP_UNAUTHORIZED   = 401,
    HTTP_FORBIDDEN      = 403,
    HTTP_NOTFOUND       = 404,
};

const char* HttpReasonFromCode(enum EHttpStatusCodes);

/* General Constants */

extern const char c_strHttpBasic[];

/* Reason Messages */

//! 200 OK
extern const char c_strHttpReasonOK[];

//! 302 Found
extern const char c_strHttpReasonFound[];

//! 400 Bad Request
extern const char c_strHttpReasonBadRequest[];

//! 401 Unauthorized
extern const char c_strHttpReasonUnauthorized[];

//! 403 Forbidden
extern const char c_strHttpReasonForbidden[];

//! 404 Not Found
extern const char c_strHttpReasonNotFound[];

/* Header Fields */

//! Host
extern const char c_strHttpHeaderHost[];

//! Content-Type
extern const char c_strHttpHeaderContentType[];

//! Content-Length
extern const char c_strHttpHeaderContentLength[];

//! WWW-Authenticate
extern const char c_strHttpHeaderWWWAuthenticate[];

//! Authorization
extern const char c_strHttpHeaderAuthorization[];

//! Location
extern const char c_strHttpHeaderLocation[];

/* MIME Types */

//! text/html
extern const char c_strHttpTypeHtml[];

//! application/x-www-form-urlencoded
extern const char c_strHttpTypeWWWForm[];

#ifdef __cplusplus
}
#endif

#endif