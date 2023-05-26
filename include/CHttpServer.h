#ifndef _CHTTPSERVER_H_
#define _CHTTPSERVER_H_

#include <Ethernet.h>
#include "CHttpResponse.h"

//! Handles HTTP transactions
class CHttpServer
{
protected:
    //! Request method is parsed
    virtual void Method(const char* pMethod, size_t uLen) = 0;

    //! Target URI is parsed
    virtual void Target(const char* pTarget, size_t uLen) = 0;

    //! Query parameters parsed
    virtual void Query(const char* pQuery, size_t uLen) = 0;

    //! Content type was parsed
    virtual void ContentType(const char* pType, size_t uLen) = 0;

    //! Handle the body
    virtual void Body(const char* pBody, size_t uLen) = 0;

    //! A header field was parsed
    virtual void HandleField(const char* pName, size_t uNameLen, const char* pValue, size_t uValueLen);

    //! Generates a response
    virtual void GetResponse(CHttpResponse&) = 0;

    //! Restore all state variables and prepare for next request
    virtual void Reset();

private:
    //! Connected client
    EthernetClient& m_ethCli;

    //! Total size of the body
    size_t m_uContentLength;

    //! Parser state machine
    enum class State
    {
        StartLine,
        Fields,
        Body,
    } m_eState;

    //! Parses a single line of the HTTP request
    //! @return True if success
    bool ParseLine(const char* pData, const char* pEnd);

    //! Full request was received
    void EndOfRequest();

public:
    CHttpServer(EthernetClient& ethCli);

    //! Reads the hostname of the server
    static const char* GetHostName();

    //! Returns after connection is handles
    void Run();
};

#endif