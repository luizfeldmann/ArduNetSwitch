#include "CAppServer.h"
#include "stringview.h"
#include "HttpConstants.h"
#include "webpages.h"
#include "base64.h"
#include "persistent.h"

CAppServer::CAppServer(EthernetClient& ethCli)
    : CHttpServer(ethCli)
    , m_eAuth(Auth::None)
    , m_eContentType(ContentType::None)
    , m_eTarget(Target::NotFound)
{

}

void CAppServer::Reset()
{
    CHttpServer::Reset();

    m_eAuth = Auth::None;
    m_eContentType = ContentType::None;
    m_eTarget = Target::NotFound;
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
    else if (stringview_cmp("/admin.html", pTarget, uLen))
    {
        m_eTarget = Target::Admin;
    }
}

void CAppServer::Query(const char* pQuery, size_t uLen)
{
    // Unused
}

void CAppServer::ContentType(const char* pType, size_t uLen)
{
    if (0 == stringview_cmp(c_strHttpTypeWWWForm, pType, uLen))
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
    m_eAuth = (Persist_CheckUserName(pUser, uUserLen) && Persist_CheckPassword(pPass, uPassLen))
        ? Auth::Good
        : Auth::Bad;
}

void CAppServer::Body(const char* pBody, size_t uContentLength)
{
    if (ContentType::WWWForm == m_eContentType)
    {
        // Received form data: key=value(&...)
        const char* const pEnd = pBody + uContentLength;

        const char* pNameStart = pBody;
        size_t pNameLen = 0;

        const char* pValueStart = nullptr;
        size_t pValueLen = 0;

        for (;; ++pBody)
        {
            if (pBody >= pEnd)
            {
                if (!pNameLen || !pValueStart)
                    return; // Unexpected EOF

                // End of form - handle last field
                pValueLen = pBody - pValueStart;
                HandleFormData(pNameStart, pNameLen, pValueStart, pValueLen);

                break;
            }

            if (*pBody == '=')
            {
                pNameLen = pBody - pNameStart;
                pValueStart = pBody + 1;
            }
            else if (*pBody == '&')
            {
                // End of value - handle it
                pValueLen = pBody - pValueStart;
                HandleFormData(pNameStart, pNameLen, pValueStart, pValueLen);

                // Reset for next pair
                pNameStart = pBody + 1;
                pNameLen = 0;
                pValueStart = nullptr;
                pValueLen = 0;
            }
        }
    }
    else
    {
        // Unknown type of content
    }
}

void CAppServer::HandleFormData(const char* pName, size_t uNameLen, const char* pValue, size_t uValueLen)
{

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
    else if (Auth::None == m_eAuth)
    {
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
        Response.m_eStatusCode = EHttpStatusCodes::HTTP_OK;
        Response.m_sContentType = c_strHttpTypeHtml;
        Response.m_pContentStart = _binary_admin_html_start;
        Response.m_uContentLength = _binary_admin_html_end - Response.m_pContentStart;
    }
}