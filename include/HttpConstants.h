#ifndef _HTTPCONSTANTS_H_
#define _HTTPCONSTANTS_H_

#ifdef __cplusplus
extern "C" {
#endif

//! HTTP status codes
enum EHttpStatusCodes
{
    HTTP_OK = 200,
};

const char* HttpReasonFromCode(enum EHttpStatusCodes);

/* Reason Messages */

//! 200 OK
extern const char c_strHttpReasonOK[];

/* Header Fields */

//! Content-Type
extern const char c_strHttpHeaderContentType[];

//! Content-Length
extern const char c_strHttpHeaderContentLength[];

/* MIME Types */

//! text/html
extern const char c_strHttpTypeHtml[];

#ifdef __cplusplus
}
#endif

#endif