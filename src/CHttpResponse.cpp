#include "CHttpResponse.h"

/* Constants */

static const char c_szCRLF[] = { '\r', '\n' };
static const char c_szHSep[] = { ':', ' ' };

/* Utils */

static inline void WriteHeaderField(Print& Out, const char* sKey, const char* sValue)
{
    Out.write(sKey);
    Out.write(c_szHSep, sizeof(c_szHSep));
    Out.write(sValue);
    Out.write(c_szCRLF, sizeof(c_szCRLF));
}

/* CHttpResponse */

CHttpResponse::CHttpResponse()
    : m_eStatusCode(EHttpStatusCodes::HTTP_NOTFOUND)
    , m_sHost(nullptr)
    , m_sAuthenticate(nullptr)
    , m_sContentType(nullptr)
    , m_pContentStart(nullptr)
    , m_uContentLength(0)
{

}

void
CHttpResponse::Send(Print& Out)
{
    // Status line
    static const char szHttpVers[] = "HTTP/1.1 ";
    Out.write(szHttpVers);
    {
        char szStatusCode[8];
        int nLenCode = snprintf(szStatusCode, sizeof(szStatusCode), "%" PRIu16 " ", m_eStatusCode);
        Out.write(szStatusCode, nLenCode);
    }
    const char* sReason = HttpReasonFromCode(m_eStatusCode);
    Out.write(sReason);
    Out.write(c_szCRLF, sizeof(c_szCRLF));

    // Host
    if (m_sHost)
        WriteHeaderField(Out, c_strHttpHeaderHost, m_sHost);

    // Authenticate
    if (m_sAuthenticate)
         WriteHeaderField(Out, c_strHttpHeaderWWWAuthenticate, m_sAuthenticate);

    // Type
    if (m_sContentType)
        WriteHeaderField(Out, c_strHttpHeaderContentType, m_sContentType);

    // Length
    {
        char szLength[16];
        utoa(m_uContentLength, szLength, 10);

        WriteHeaderField(Out, c_strHttpHeaderContentLength, szLength);
    }

    // Final CRLF
    Out.write(c_szCRLF, sizeof(c_szCRLF));

    // Body
    if (m_pContentStart && m_uContentLength)
    {
        Out.write(m_pContentStart, m_uContentLength);
    }

    Out.flush();
}