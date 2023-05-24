#ifndef _CHTTPSERVER_H_
#define _CHTTPSERVER_H_

#include <Ethernet.h>

//! Handles HTTP transactions
class CHttpServer
{
private:
    //! Connected client
    EthernetClient& m_ethCli;

    //! Parses a single line of the HTTP request
    void ParseLine(const char* pFirst, const char* pEnd);

    //! Full request was received
    void EndOfRequest();

public:
    CHttpServer(EthernetClient& ethCli);

    //! Returns after connection is handles
    void Run();
};

#endif