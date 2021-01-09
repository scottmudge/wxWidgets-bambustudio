///////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/webrequest_winhttp.h
// Purpose:     wxWebRequest WinHTTP implementation
// Author:      Tobias Taschner
// Created:     2018-10-17
// Copyright:   (c) 2018 wxWidgets development team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MSW_WEBREQUEST_WINHTTP_H
#define _WX_MSW_WEBREQUEST_WINHTTP_H

#include "wx/private/webrequest.h"

#include "wx/msw/wrapwin.h"
#include <winhttp.h>
#include "wx/buffer.h"

class wxWebSessionWinHTTP;
class wxWebRequestWinHTTP;

class wxWebResponseWinHTTP : public wxWebResponseImpl
{
public:
    wxWebResponseWinHTTP(wxWebRequestWinHTTP& request);

    wxInt64 GetContentLength() const wxOVERRIDE { return m_contentLength; }

    wxString GetURL() const wxOVERRIDE;

    wxString GetHeader(const wxString& name) const wxOVERRIDE;

    int GetStatus() const wxOVERRIDE;

    wxString GetStatusText() const wxOVERRIDE;

    bool ReadData();

    bool ReportAvailableData(DWORD dataLen);

private:
    HINTERNET m_requestHandle;
    wxInt64 m_contentLength;

    wxDECLARE_NO_COPY_CLASS(wxWebResponseWinHTTP);
};

class wxWebAuthChallengeWinHTTP : public wxWebAuthChallengeImpl
{
public:
    wxWebAuthChallengeWinHTTP(wxWebAuthChallenge::Source source,
                              wxWebRequestWinHTTP& request);

    bool Init();

    void SetCredentials(const wxString& user, const wxString& password) wxOVERRIDE;

private:
    wxWebRequestWinHTTP& m_request;
    DWORD m_target;
    DWORD m_selectedScheme;

    wxDECLARE_NO_COPY_CLASS(wxWebAuthChallengeWinHTTP);
};

class wxWebRequestWinHTTP : public wxWebRequestImpl
{
public:
    wxWebRequestWinHTTP(wxWebSession& session,
                        wxWebSessionWinHTTP& sessionWinHTTP,
                        wxEvtHandler* handler,
                        const wxString& url,
                        int id);

    ~wxWebRequestWinHTTP();

    void Start() wxOVERRIDE;

    void Cancel() wxOVERRIDE;

    wxWebResponseImplPtr GetResponse() const wxOVERRIDE
        { return m_response; }

    wxWebAuthChallengeImplPtr GetAuthChallenge() const wxOVERRIDE
        { return m_authChallenge; }

    wxFileOffset GetBytesSent() const wxOVERRIDE { return m_dataWritten; }

    wxFileOffset GetBytesExpectedToSend() const wxOVERRIDE { return m_dataSize; }

    void HandleCallback(DWORD dwInternetStatus, LPVOID lpvStatusInformation,
        DWORD dwStatusInformationLength);

    HINTERNET GetHandle() const { return m_request; }

private:
    wxWebSessionWinHTTP& m_sessionWinHTTP;
    wxString m_url;
    HINTERNET m_connect;
    HINTERNET m_request;
    wxObjectDataPtr<wxWebResponseWinHTTP> m_response;
    wxObjectDataPtr<wxWebAuthChallengeWinHTTP> m_authChallenge;
    wxMemoryBuffer m_dataWriteBuffer;
    wxFileOffset m_dataWritten;

    void SendRequest();

    void WriteData();

    void CreateResponse();

    // Retrieve the error message corresponding to the given error and set the
    // state to failed with this message as error string.
    void SetFailed(DWORD errorCode);

    void SetFailedWithLastError() { SetFailed(::GetLastError()); }

    friend class wxWebAuthChallengeWinHTTP;

    wxDECLARE_NO_COPY_CLASS(wxWebRequestWinHTTP);
};

class wxWebSessionWinHTTP : public wxWebSessionImpl
{
public:
    wxWebSessionWinHTTP();

    ~wxWebSessionWinHTTP();

    wxWebRequestImplPtr
    CreateRequest(wxWebSession& session,
                  wxEvtHandler* handler,
                  const wxString& url,
                  int id) wxOVERRIDE;

    wxVersionInfo GetLibraryVersionInfo() wxOVERRIDE;

    HINTERNET GetHandle() const { return m_handle; }

private:
    HINTERNET m_handle;

    bool Open();

    wxDECLARE_NO_COPY_CLASS(wxWebSessionWinHTTP);
};

class wxWebSessionFactoryWinHTTP : public wxWebSessionFactory
{
public:
    wxWebSessionImpl* Create() wxOVERRIDE
        { return new wxWebSessionWinHTTP(); }
};

#endif // _WX_MSW_WEBREQUEST_WINHTTP_H
