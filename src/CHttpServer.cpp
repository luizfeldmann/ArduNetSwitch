#include "CHttpServer.h"
#include "CHttpResponse.h"
#include "webpages.h"
#include "stringview.h"
#include "base64.h"
#include "persistent.h"
#include <Arduino.h>

/* CHttpServer */

CHttpServer::CHttpServer(EthernetClient& ethCli)
    : m_ethCli(ethCli)
    , m_eState(State::StartLine)
    , m_eAuth(Auth::None)
    , m_szMethod{0}
    , m_szTarget{0}
    , m_szQuery{0}
    , m_uContentLength(0)
    , m_szContentType{0}
{

}

/* static */ const char* CHttpServer::GetHostName()
{
    static char g_szHost[20] = {0};

    union {
	    uint8_t localip_bytes[4];
	    uint32_t localip_raw;
    };

    localip_raw = Ethernet.localIP();

    sprintf(g_szHost, "%" PRIu8 ".%" PRIu8 ".%" PRIu8 ".%" PRIu8,
        localip_bytes[0], localip_bytes[1], localip_bytes[2], localip_bytes[3]);

    return g_szHost;
}


void CHttpServer::EndOfRequest()
{
    CHttpResponse Response;

    if (0 == strcmp(m_szTarget, "/") ||
        0 == strcmp(m_szTarget, "/index.html"))
    {
        Response.m_eStatusCode = EHttpStatusCodes::HTTP_OK;
        Response.m_sContentType = c_strHttpTypeHtml;
        Response.m_pContentStart = _binary_index_html_start;
        Response.m_uContentLength = _binary_index_html_end - Response.m_pContentStart;
    }
    else if (0 == strcmp(m_szTarget, "/admin.html"))
    {
        if (Auth::None == m_eAuth)
        {
            Response.m_eStatusCode = EHttpStatusCodes::HTTP_UNAUTHORIZED;
            Response.m_sAuthenticate = c_strHttpBasic;
        }
        else if (Auth::Bad == m_eAuth)
        {
            Response.m_eStatusCode = EHttpStatusCodes::HTTP_FORBIDDEN;
            Response.m_sAuthenticate = c_strHttpBasic;
            Response.m_pContentStart = _binary_forbidden_html_start;
            Response.m_uContentLength = _binary_forbidden_html_end - Response.m_pContentStart;
        }
        else
        {
            Response.m_eStatusCode = EHttpStatusCodes::HTTP_OK;
            Response.m_sContentType = c_strHttpTypeHtml;
            Response.m_pContentStart = _binary_admin_html_start;
            Response.m_uContentLength = _binary_admin_html_end - Response.m_pContentStart;
        }
    }
    else
    {
        Response.m_eStatusCode = EHttpStatusCodes::HTTP_NOTFOUND;
        Response.m_sContentType = c_strHttpTypeHtml;
        Response.m_pContentStart = _binary_notfound_html_start;
        Response.m_uContentLength = _binary_notfound_html_end - Response.m_pContentStart;
    }
    
    // Send response
    Response.m_sHost = GetHostName();
    Response.Send(m_ethCli);

    // Reset server state variables
    Reset();
}

void CHttpServer::Reset()
{
    m_eState = State::StartLine;
    m_eAuth = Auth::None;
    m_uContentLength = 0;
    memset(m_szMethod, 0, sizeof(m_szMethod));
    memset(m_szTarget, 0, sizeof(m_szTarget));
    memset(m_szQuery, 0, sizeof(m_szQuery));
    memset(m_szContentType, 0, sizeof(m_szContentType));
}

void CHttpServer::HandleFormData(const char* pName, size_t uNameLen, const char* pValue, size_t uValueLen)
{

}

void CHttpServer::HandleBody(const char* pBody)
{
    if (0 == strcmp(m_szContentType, c_strHttpTypeWWWForm))
    {
        // Received form data: key=value(&...)
        const char* const pEnd = pBody + m_uContentLength;

        const char* pNameStart = pBody;
        size_t pNameLen = 0;

        const char* pValueStart = nullptr;
        size_t pValueLen = 0;

        for (;; ++pBody)
        {
            if (pBody >= pEnd)
            {
                if (!pNameLen || !pValueStart)
                    return; // Unexpected EOF
                
                // End of form - handle last field
                pValueLen = pBody - pValueStart;
                HandleFormData(pNameStart, pNameLen, pValueStart, pValueLen);

                break;
            }

            if (*pBody == '=')
            {
                pNameLen = pBody - pNameStart;
                pValueStart = pBody + 1;
            }
            else if (*pBody == '&')
            {
                // End of value - handle it
                pValueLen = pBody - pValueStart;
                HandleFormData(pNameStart, pNameLen, pValueStart, pValueLen);

                // Reset for next pair
                pNameStart = pBody + 1;
                pNameLen = 0;
                pValueStart = nullptr;
                pValueLen = 0;
            }
        }
    }
    else
    {
        // Unknown type of content
    }
}

void CHttpServer::CheckAuthorization(const char* pUser, size_t uUserLen, const char* pPass, size_t uPassLen)
{
    m_eAuth = (Persist_CheckUserName(pUser, uUserLen) && Persist_CheckPassword(pPass, uPassLen))
        ? Auth::Good
        : Auth::Bad;
}

void CHttpServer::ParseAuthorization(const char* pData, size_t uSize)
{
    const char* pFirst = pData;
    const char* pEnd = pData + uSize;

    // Read scheme
    for (;;++pData)
    {
        if (pData >= pEnd)
            return; // Unexpected EOL

        if (*pData == ' ')
            break;
    }

    if (!stringview_cmp(c_strHttpBasic, pFirst, pData - pFirst))
        return; // Violation of Auth scheme

    ++pData; // Consume the SP

    // Decode base64 digest
    char arrDecodeBuf[32];
    size_t uDecodeSize = sizeof(arrDecodeBuf);

    if (base64_decode(pData, pEnd - pData, arrDecodeBuf, &uDecodeSize))
        return; // Decode failed

    // Parse 'user:pass' pair
    pFirst = arrDecodeBuf;
    pEnd = pFirst + uDecodeSize;
    pData = arrDecodeBuf;
    
    for (;; ++pData)
    {
        if (pData >= pEnd)
            return; // Unexpected EOL

        if (*pData == ':')
            break;
    }

    CheckAuthorization(pFirst, pData - pFirst, pData + 1, pEnd - pData - 1);
}

void CHttpServer::HandleField(const char* pName, size_t uNameLen, const char* pValue, size_t uValueLen)
{
    if (stringview_cmp(c_strHttpHeaderContentType, pName, uNameLen))
    {
        // Handle Content-Type
        stringview_copy(m_szContentType, sizeof(m_szContentType), pValue, uValueLen);
    }
    else if (stringview_cmp(c_strHttpHeaderContentLength, pName, uNameLen))
    {
        // Handle Content-Length
        m_uContentLength = atoi(pValue);
    }
    else if (stringview_cmp(c_strHttpHeaderAuthorization, pName, uNameLen))
    {
        // Parse username and password
        ParseAuthorization(pValue, uValueLen);
    }
    else
    {
        // Unknown field
        // TODO
    }
}

bool CHttpServer::ParseLine(const char* pData, const char* pEnd)
{
    if (m_eState == State::StartLine)
    {
        // Read method
        const char* pFirst = pData;

        for (;; ++pData)
        {
            if (pData >= pEnd)
                return false; // Unexpected EOL

            if (*pData == ' ')
                break;
        }

        stringview_copy((char*)m_szMethod, sizeof(m_szMethod), pFirst, pData - pFirst);
        ++pData; // skip the SP

        // Read the target
        bool bQuery = false;
        pFirst = pData;
    
        for (;; ++pData)
        {
            if (pData >= pEnd)
                return false; // Unexpected EOL

            if (*pData == ' ')
                break;

            if (*pData == '?')
            {
                bQuery = true;
                break;
            }
        }

        stringview_copy(m_szTarget, sizeof(m_szTarget), pFirst, pData - pFirst);
        ++pData; // skip the SP or '?'

        // Read the query
        if (bQuery)
        {
            pFirst = pData;

            for (;; ++pData)
            {
                if (pData >= pEnd)
                    return false; // Unexpected EOL

                if (*pData == ' ')
                    break;
            }

            stringview_copy(m_szQuery, sizeof(m_szQuery), pFirst, pData - pFirst);
            ++pData; // skip the SP
        }

        // Read HTTP version
        // Ignored - not needed

        // Next state are the headers
        m_eState = State::Fields;
    }
    else if (pData == pEnd)
    {
        // Empty header field means no more header fields to come
        if (m_uContentLength)
        {
            // If a content exists, read it
            m_eState = State::Body;
        }
        else
        {
            // No body to come - handle the full request
            EndOfRequest();
        }
    }
    else
    {
        // Read field name
        const char* pFieldName = pData;
        size_t uFieldNameLen = 0;

        for (;; ++pData)
        {
            if (pData >= pEnd)
                return false; // Unexpected EOL

            if (*pData == ':')
                break;
        }

        uFieldNameLen = pData - pFieldName;
        ++pData; // skip the ':'

        // Skip white spaces
        for (;; ++pData)
        {
            if (pData >= pEnd)
                return false; // Unexpected EOL

            if (*pData != ' ')
                break;
        }

        // Read value
        size_t uFieldValueLen = pEnd - pData;

        // Handle the field
        HandleField(pFieldName, uFieldNameLen, pData, uFieldValueLen);
    }

    return true;
}

void CHttpServer::Run()
{
    bool bError = false;

    // Receive buffer
    char arrRecv[128];

    //! Current read pointer
    char* pCurr = arrRecv;

    // While socket is connected
    while (m_ethCli.connected() && !bError)
    {
        // Read all bytes one by one
        int nAvail = m_ethCli.available();
        for (int i = 0; (i < nAvail) && !bError; ++i)
        {
            // Save current byte in buffer
            *pCurr = m_ethCli.read();

            // Check if we ended a line
            if ((m_eState != State::Body) &&  (*pCurr == '\n'))
            {
                if ((pCurr > arrRecv) && *(pCurr-1) == '\r')
                {
                    // Parse full line
                    bError = !ParseLine(arrRecv, pCurr - 1); // The parsed line does not include the CR LF

                    // Reset
                    pCurr = arrRecv;
                }
                else
                {
                    bError = true; // Bad line ending
                }
            }
            else
            {
                // Advance pointer
                ++pCurr;

                size_t uBufLen = pCurr - arrRecv;

                if ((m_eState == State::Body) && (uBufLen == m_uContentLength))
                {
                    HandleBody(arrRecv);
                    EndOfRequest();

                    // Reset
                    pCurr = arrRecv;
                }

                if (uBufLen >= sizeof(arrRecv))
                    bError = true; // RX buffer overflow
            }
        }
    }
}

