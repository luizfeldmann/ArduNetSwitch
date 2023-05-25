#include "CHttpResponse.h"

static const char c_szCRLF[] = { '\r', '\n' };
static const char c_szHSep[] = { ':', ' ' };

/* static */
void
CHttpResponse::Send(Print& Out, enum EHttpStatusCodes eStatusCode, const char* sHost, const char* sContentType, const char* pContentStart, size_t uContentLength)
{
    // Status line
    static const char szHttpVers[] = "HTTP/1.1 ";
    Out.write(szHttpVers);
    {
        char szStatusCode[8];
        int nLenCode = snprintf(szStatusCode, sizeof(szStatusCode), "%" PRIu16 " ", eStatusCode);
        Out.write(szStatusCode, nLenCode);
    }
    const char* sReason = HttpReasonFromCode(eStatusCode);
    Out.write(sReason);
    Out.write(c_szCRLF, sizeof(c_szCRLF));

    // Host
    if (sHost)
    {
        Out.write(c_strHttpHeaderHost);
        Out.write(c_szHSep, sizeof(c_szHSep));
        Out.write(sHost);
        Out.write(c_szCRLF, sizeof(c_szCRLF));
    }

    // Type
    if (sContentType)
    {
        Out.write(c_strHttpHeaderContentType);
        Out.write(c_szHSep, sizeof(c_szHSep));
        Out.write(sContentType);
        Out.write(c_szCRLF, sizeof(c_szCRLF));
    }

    // Length
    Out.write(c_strHttpHeaderContentLength);
    Out.write(c_szHSep, sizeof(c_szHSep));
    {
        char szLength[16];
        utoa(uContentLength, szLength, 10);
        Out.write(szLength);
    }
    Out.write(c_szCRLF, sizeof(c_szCRLF));

    // Final CRLF
    Out.write(c_szCRLF, sizeof(c_szCRLF));

    // Body
    if (pContentStart && uContentLength)
    {
        Out.write(pContentStart, uContentLength);
    }

    Out.flush();
}