#include "CHttpServer.h"
#include "stringview.h"
#include <Arduino.h>

/* CHttpServer */

CHttpServer::CHttpServer(EthernetClient& ethCli)
    : m_ethCli(ethCli)
    , m_eState(State::StartLine)
    , m_uContentLength(0)
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

    GetResponse(Response);
    
    // Send response
    Response.m_sHost = GetHostName();
    Response.Send(m_ethCli);

    // Reset server state variables
    Reset();
}

void CHttpServer::Reset()
{
    m_eState = State::StartLine;
    m_uContentLength = 0;
}

void CHttpServer::HandleField(const char* pName, size_t uNameLen, const char* pValue, size_t uValueLen)
{
    if (stringview_cmp(c_strHttpHeaderContentType, pName, uNameLen))
    {
        // Handle Content-Type
        ContentType(pValue, uValueLen);
    }
    else if (stringview_cmp(c_strHttpHeaderContentLength, pName, uNameLen))
    {
        // Handle Content-Length
        m_uContentLength = atoi(pValue);
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

        Method(pFirst, pData - pFirst);
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

        Target(pFirst, pData - pFirst);
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

            Query(pFirst, pData - pFirst);
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
                    Body(arrRecv, m_uContentLength);
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