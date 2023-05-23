#ifndef _CHTTPSERVER_H_
#define _CHTTPSERVER_H_

#include <Ethernet.h>

//! Handles HTTP transactions
class CHttpServer
{
private:
    EthernetClient& m_ethCli;

public:
    CHttpServer(EthernetClient& ethCli);

    //! Called while connected
    //! @return True to continue
    bool Loop();
};

#endif