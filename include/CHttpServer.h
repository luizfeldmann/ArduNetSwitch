#ifndef _CHTTPSERVER_H_
#define _CHTTPSERVER_H_

#include <Ethernet.h>

//! Handles HTTP transactions
class CHttpServer
{
private:
    //! Connected client
    EthernetClient& m_ethCli;

    //! Request method
    char m_szMethod[8];

    //! Target URI
    char m_szTarget[16];

    //! Query parameters
    char m_szQuery[16];

    //! Total size of the body
    size_t m_uContentLength;

    //! Parser state machine
    enum class State
    {
        StartLine,
        Fields,
        Body,
    } m_eState;

    //! Status of authentication
    enum class Auth {
        None,  //!< No auth was provided
        Bad,   //!< Provided credentials are wrong
        Good,  //!< Provided credentials are OK
    } m_eAuth;

    //! Reset state of parser
    void Reset();

    //! Handles the body content
    void HandleBody(const char* pBody);

    //! Handles a header field
    void HandleField(const char* pName, size_t uNameLen, const char* pValue, size_t uValueLen);

    //! Parses username and password
    void ParseAuthorization(const char* pValue, size_t uValueLen);

    //! Verifies the provided credentials are valid
    void CheckAuthorization(const char* pUser, size_t uUserLen, const char* pPass, size_t uPassLen);

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