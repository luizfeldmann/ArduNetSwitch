#include "CHttpServer.h"
#include <Arduino.h>

CHttpServer::CHttpServer(EthernetClient& ethCli)
    : m_ethCli(ethCli)
{

}

bool CHttpServer::Loop()
{
    if (m_ethCli.available())
    {
        char c = m_ethCli.read();
        Serial.write(c);
    }

    return true;
}

