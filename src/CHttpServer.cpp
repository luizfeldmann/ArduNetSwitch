#include "CHttpServer.h"
#include "CHttpResponse.h"
#include "webpages.h"
#include <Arduino.h>

CHttpServer::CHttpServer(EthernetClient& ethCli)
    : m_ethCli(ethCli)
{

}

void CHttpServer::EndOfRequest()
{
    // Send response
    const char* pWebpage = _binary_index_html_start;
    const size_t uLen = _binary_index_html_end - _binary_index_html_start;
    CHttpResponse::Send(m_ethCli, HTTP_OK, c_strHttpTypeHtml, pWebpage, uLen);
}

void CHttpServer::ParseLine(const char* pFirst, const char* pEnd)
{
    if (pFirst == pEnd)
        EndOfRequest();
    else
    {
        // To Do: parse request
    }
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
            if (*pCurr == '\n')
            {
                if ((pCurr > arrRecv) && *(pCurr-1) == '\r')
                {
                    // Parse full line
                    ParseLine(arrRecv, pCurr - 1); // The parsed line does not include the CR LF

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

                if (pCurr >= arrRecv + sizeof(arrRecv))
                    bError = true; // RX buffer overflow
            }
        }
    }
}

