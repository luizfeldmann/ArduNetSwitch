#include "CAppServer.h"
#include "stringview.h"
#include "HttpConstants.h"
#include "webpages.h"
#include "base64.h"
#include "persistent.h"

static const char c_szAdminPageURL[] = "/admin.html";

CAppServer::CAppServer(EthernetClient& ethCli)
    : CHttpServer(ethCli)
    , m_eAuth(Auth::None)
    , m_eContentType(ContentType::None)
    , m_eTarget(Target::NotFound)
    , m_eCommand(ESwitchCommand::None)
{

}

CAppServer::SCredChangeReq::SCredChangeReq()
    : szUser{ 0 }
    , szPass{ 0 }
{

}

void CAppServer::Reset()
{
    CHttpServer::Reset();
    
    if (Auth::Refresh != m_eAuth)
        m_eAuth = Auth::None;

    m_eContentType = ContentType::None;
    m_eTarget = Target::NotFound;
    m_eCommand = ESwitchCommand::None;
}

void CAppServer::Method(const char* pMethod, size_t uLen)
{
    // Unused
}

void CAppServer::Target(const char* pTarget, size_t uLen)
{
    if (stringview_cmp("/"          , pTarget, uLen) ||
        stringview_cmp("/index.html", pTarget, uLen))
    {
        m_eTarget = Target::Index;
    }
    else if (stringview_cmp(c_szAdminPageURL, pTarget, uLen))
    {
        m_eTarget = Target::Admin;
    }
    else if (stringview_cmp("/change", pTarget, uLen))
    {
        m_eTarget = Target::Change;
    }
    else if (stringview_cmp("/initial", pTarget, uLen))
    {
        m_eTarget = Target::Initial;
    }
    else if (stringview_cmp("/switch", pTarget, uLen))
    {
        m_eTarget = Target::Switch;
    }
}

void CAppServer::Query(const char* pQuery, size_t uLen)
{
    // Received form data: key=value(&...)
    const char* const pEnd = pQuery + uLen;

    const char* pNameStart = pQuery;
    size_t pNameLen = 0;

    const char* pValueStart = nullptr;
    size_t pValueLen = 0;

    for (;; ++pQuery)
    {
        if (pQuery >= pEnd)
        {
            if (!pNameLen || !pValueStart)
                return; // Unexpected EOF

            // End of form - handle last field
            pValueLen = pQuery - pValueStart;
            HandleFormData(pNameStart, pNameLen, pValueStart, pValueLen);

            break;
        }

        if (*pQuery == '=')
        {
            pNameLen = pQuery - pNameStart;
            pValueStart = pQuery + 1;
        }
        else if (*pQuery == '&')
        {
            // End of value - handle it
            pValueLen = pQuery - pValueStart;
            HandleFormData(pNameStart, pNameLen, pValueStart, pValueLen);

            // Reset for next pair
            pNameStart = pQuery + 1;
            pNameLen = 0;
            pValueStart = nullptr;
            pValueLen = 0;
        }
    }
}

void CAppServer::ContentType(const char* pType, size_t uLen)
{
    if (stringview_cmp(c_strHttpTypeWWWForm, pType, uLen))
    {
        m_eContentType = ContentType::WWWForm;
    }
}

void CAppServer::HandleField(const char* pName, size_t uNameLen, const char* pValue, size_t uValueLen)
{
    if (stringview_cmp(c_strHttpHeaderAuthorization, pName, uNameLen))
    {
        ParseAuthorization(pValue, uValueLen);
    }
    else
    {
        CHttpServer::HandleField(pName, uNameLen, pValue, uValueLen);
    }
}

void CAppServer::ParseAuthorization(const char* pData, size_t uSize)
{
    const char* pFirst = pData;
    const char* pEnd = pData + uSize;

    // Read scheme
    for (;; ++pData)
    {
        if (pData >= pEnd)
            return; // Unexpected EOL

        if (*pData == ' ')
            break;
    }

    if (!stringview_cmp(c_strHttpBasic, pFirst, pData - pFirst))
        return; // Violation of Auth scheme

    ++pData; // Consume the SP

    // Decode base64 digest
    char arrDecodeBuf[32];
    size_t uDecodeSize = sizeof(arrDecodeBuf);

    if (base64_decode(pData, pEnd - pData, arrDecodeBuf, &uDecodeSize))
        return; // Decode failed

    // Parse 'user:pass' pair
    pFirst = arrDecodeBuf;
    pEnd = pFirst + uDecodeSize;
    pData = arrDecodeBuf;

    for (;; ++pData)
    {
        if (pData >= pEnd)
            return; // Unexpected EOL

        if (*pData == ':')
            break;
    }

    CheckAuthorization(pFirst, pData - pFirst, pData + 1, pEnd - pData - 1);
}

void CAppServer::CheckAuthorization(const char* pUser, size_t uUserLen, const char* pPass, size_t uPassLen)
{
    if (Auth::Refresh == m_eAuth)
        return; // If need to refresh, ignore the read values

    m_eAuth = (Persist_CheckUserName(pUser, uUserLen) && Persist_CheckPassword(pPass, uPassLen))
        ? Auth::Good
        : Auth::Bad;
}

void CAppServer::Body(const char* pBody, size_t uContentLength)
{
    if (ContentType::WWWForm == m_eContentType)
    {
        // Treat form the same as query
        Query(pBody, uContentLength);
    }
    else
    {
        // Unknown type of content
    }
}

void CAppServer::HandleFormData(const char* pName, size_t uNameLen, const char* pValue, size_t uValueLen)
{
    static const char c_szSwitchCmdOn[] = "on";
    static const char c_szSwitchCmdOff[] = "off";

    if (Target::Change == m_eTarget)
    {
        static const char c_szFormUser[] = "username";
        static const char c_szFormPass[] = "password";

        if (stringview_cmp(c_szFormUser, pName, uNameLen))
        {
            stringview_copy(m_stCredentialsChangeRequest.szUser, sizeof(m_stCredentialsChangeRequest.szUser),
                pValue, uValueLen);
        }
        else if (stringview_cmp(c_szFormPass, pName, uNameLen))
        {
            stringview_copy(m_stCredentialsChangeRequest.szPass, sizeof(m_stCredentialsChangeRequest.szPass),
                pValue, uValueLen);
        }
    }
    else if (Target::Initial == m_eTarget)
    {
        static const char c_szInitialState[] = "initial";

        if (stringview_cmp(c_szInitialState, pName, uNameLen))
        {
            if (stringview_cmp(c_szSwitchCmdOn, pValue, uValueLen))
                m_eCommand = ESwitchCommand::On;
            else if (stringview_cmp(c_szSwitchCmdOff, pValue, uValueLen))
                m_eCommand = ESwitchCommand::Off;
        }
    }
    else if (Target::Switch == m_eTarget)
    {
        static const char c_szSwitchCommand[] = "command";

        static const char c_szSwitchCmdTog[] = "toggle";
        static const char c_szSwitchCmdPls[] = "pulse";

        if (stringview_cmp(c_szSwitchCommand, pName, uNameLen))
        {
            if (stringview_cmp(c_szSwitchCmdOn, pValue, uValueLen))
                m_eCommand = ESwitchCommand::On;
            else if (stringview_cmp(c_szSwitchCmdOff, pValue, uValueLen))
                m_eCommand = ESwitchCommand::Off;
            else if (stringview_cmp(c_szSwitchCmdTog, pValue, uValueLen))
                m_eCommand = ESwitchCommand::Toggle;
            else if (stringview_cmp(c_szSwitchCmdPls, pValue, uValueLen))
                m_eCommand = ESwitchCommand::Pulse;
        }
    }
}

void CAppServer::GetResponse(CHttpResponse& Response)
{
    if (Target::NotFound == m_eTarget)
    {
        Response.m_eStatusCode = EHttpStatusCodes::HTTP_NOTFOUND;
        Response.m_sContentType = c_strHttpTypeHtml;
        Response.m_pContentStart = _binary_notfound_html_start;
        Response.m_uContentLength = _binary_notfound_html_end - Response.m_pContentStart;
    }
    else if (Target::Index == m_eTarget)
    {
        Response.m_eStatusCode = EHttpStatusCodes::HTTP_OK;
        Response.m_sContentType = c_strHttpTypeHtml;
        Response.m_pContentStart = _binary_index_html_start;
        Response.m_uContentLength = _binary_index_html_end - Response.m_pContentStart;
    }
    else if (Auth::None == m_eAuth || Auth::Refresh == m_eAuth)
    {
        m_eAuth = Auth::None; // Clear the refresh tag

        Response.m_eStatusCode = EHttpStatusCodes::HTTP_UNAUTHORIZED;
        Response.m_sAuthenticate = c_strHttpBasic;
    }
    else if (Auth::Bad == m_eAuth)
    {
        Response.m_eStatusCode = EHttpStatusCodes::HTTP_FORBIDDEN;
        Response.m_sAuthenticate = c_strHttpBasic;
        Response.m_pContentStart = _binary_forbidden_html_start;
        Response.m_uContentLength = _binary_forbidden_html_end - Response.m_pContentStart;
    }
    else
    {
        GetResponseAuthenticated(Response);
    }
}

void CAppServer::GetResponseAuthenticated(CHttpResponse& Response)
{
    if (Target::Admin == m_eTarget)
    {
        // Administrator panel
        Response.m_eStatusCode = EHttpStatusCodes::HTTP_OK;
        Response.m_sContentType = c_strHttpTypeHtml;
        Response.m_pContentStart = _binary_admin_html_start;
        Response.m_uContentLength = _binary_admin_html_end - Response.m_pContentStart;
    }
    else if (Target::Change == m_eTarget)
    {
        const bool bUpdateSuccess =
            Persist_SetUserNameAndPassword(m_stCredentialsChangeRequest.szUser, m_stCredentialsChangeRequest.szPass);

        // Verify the form data
        if (bUpdateSuccess)
        {
            // Redirect back to admin and force login again
            Response.m_eStatusCode = EHttpStatusCodes::HTTP_FOUND;
            Response.m_sLocation = c_szAdminPageURL;
            m_eAuth = Auth::Refresh;
        }
        else
        {
            // Missing form data
            BadRequest(Response);
        }
    }
    else if (Target::Initial == m_eTarget)
    {
        bool bOn;
        if ((bOn = (ESwitchCommand::On == m_eCommand)) ||
            ESwitchCommand::Off == m_eCommand)
        {
            // Apply the configuration regarding the initial state
            Persist_SetSwitchInitialState(bOn);

            // Go back to admin after command is executed
            Response.m_eStatusCode = EHttpStatusCodes::HTTP_FOUND;
            Response.m_sLocation = c_szAdminPageURL;
        }
        else
        {
            // Missing form data
            BadRequest(Response);
        }
    }
    else if (Target::Switch == m_eTarget)
    {
        if (ESwitchCommand::None != m_eCommand)
        {
            OperateSwitch(m_eCommand);

            // Go back to admin after command is executed
            Response.m_eStatusCode = EHttpStatusCodes::HTTP_FOUND;
            Response.m_sLocation = c_szAdminPageURL;
        }
        else
        {
            // Missing command from form/query
            BadRequest(Response);
        }
    }
}

void CAppServer::BadRequest(CHttpResponse& Response)
{
    Response.m_eStatusCode = EHttpStatusCodes::HTTP_BADREQUEST;
    Response.m_sContentType = c_strHttpTypeHtml;
    Response.m_pContentStart = _binary_badrequest_html_start;
    Response.m_uContentLength = _binary_badrequest_html_end - Response.m_pContentStart;
}