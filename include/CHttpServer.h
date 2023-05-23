#ifndef _CHTTPSERVER_H_
#define _CHTTPSERVER_H_

#include "CBuffer.h"
#include <Ethernet.h>

//! Handles HTTP transactions
class CHttpServer
{
private:
    //! Connected client
    EthernetClient& m_ethCli;

    //! Internal buffer
    char m_arrBufInternal[1024];

    //! Manages the receive buffer
    CBuffer m_bufRecv;

    //! Parses incoming data
    bool ParseRecv();

public:
    CHttpServer(EthernetClient& ethCli);

    //! Called while connected
    //! @return True to continue
    bool Loop();
};

#endif