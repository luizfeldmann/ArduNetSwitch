#include "CHttpServer.h"
#include <Arduino.h>

CHttpServer::CHttpServer(EthernetClient& ethCli)
    : m_ethCli(ethCli)
    , m_bufRecv(m_arrBufInternal, sizeof(m_arrBufInternal))
{

}

bool CHttpServer::ParseRecv()
{
    bool bContinue = true;

    return bContinue;
}

bool CHttpServer::Loop()
{
    bool bContinue = true;

    if (m_ethCli.available())
    {
        bool bInsert = m_bufRecv.push_back(m_ethCli);
        if (!bInsert)
        {
            Serial.println("Receive buffer overflow");
            bContinue = false;
        }
        else
        {
            bContinue = ParseRecv();
        }
    }

    return bContinue;
}

