#ifndef _CAPPSERVER_H_
#define _CAPPSERVER_H_

#include "CHttpServer.h"

//! Application specific server
class CAppServer : public CHttpServer
{
private:
    //! Status of authentication
    enum class Auth {
        None,  //!< No auth was provided
        Bad,   //!< Provided credentials are wrong
        Good,  //!< Provided credentials are OK
        Refresh,    //!< The credentials are invalidated and must be refreshed
    } m_eAuth;

    //! Type of data provided in request body
    enum class ContentType {
        None,       //!< No (or not supported) content
        WWWForm,    //!< Posted URL-encoded form data
    } m_eContentType;

    //! Known locations on this server
    enum class Target {
        NotFound,   //!< Target is not found
        Index,      //!< Landing page
        Admin,      //!< Admin panel page
        Change,     //!< Change user credentials
        Switch,     //!< (De)activate the switch
    } m_eTarget;

    //! Contains the requested new credentials change
    struct SCredChangeReq
    {
        char szUser[32];    //!< Requested new user
        char szPass[32];    //!< Requested new pass

        SCredChangeReq();
    } m_stCredentialsChangeRequest;

    //! Handles posted form data
    void HandleFormData(const char* pName, size_t uNameLen, const char* pValue, size_t uValueLen);

    //! Parses username and password
    void ParseAuthorization(const char* pValue, size_t uValueLen);

    //! Verifies the provided credentials are valid
    void CheckAuthorization(const char* pUser, size_t uUserLen, const char* pPass, size_t uPassLen);

    //! Responds with 400
    void BadRequest(CHttpResponse&);

    //! Generates response for secured pages after the authentication was successfully verified
    void GetResponseAuthenticated(CHttpResponse&);
    
protected:
    void Method(const char* pMethod, size_t uLen) override;

    void Target(const char* pTarget, size_t uLen) override;

    void Query(const char* pQuery, size_t uLen) override;

    void ContentType(const char* pType, size_t uLen) override;

    void Body(const char* pBody, size_t uLen) override;

    void HandleField(const char* pName, size_t uNameLen, const char* pValue, size_t uValueLen) override;

    void Reset() override;

    void GetResponse(CHttpResponse&) override;

public:
    CAppServer(EthernetClient& ethCli);
};

#endif