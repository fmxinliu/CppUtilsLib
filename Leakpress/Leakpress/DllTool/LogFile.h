#pragma once
// CLogFile ÃüÁîÄ¿±ê

class DLL_API CLogFile : public CObject
{
public:
    CLogFile();
    virtual ~CLogFile();
    BOOL InitLogFile(void);
    BOOL WriteLog(DWORD dwError, CString strExtraInfo);
    BOOL WriteLog(const char *str, CString strSuffix=_T(""));
    BOOL WriteLog(CString strMessage, CString strSuffix=_T(""));
    CString GetLastErrorMessage(void);

private:
    HANDLE m_hMutex;
    CString m_strFilePath;
    CString m_strErrorMessage;
};


