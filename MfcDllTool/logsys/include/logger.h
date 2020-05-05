#pragma once

#include "dllexport.h"

class DLL_API Logger
{
public:
    Logger();
    virtual ~Logger();
    virtual bool InitLogFile(void);
    virtual bool WriteLog(DWORD dwError, String strExtraInfo);
    virtual bool WriteLog(String strMessage, String strSuffix = _T(""));
    virtual bool WriteLog(const char *strMessage, const char *strSuffix = ""); // Unicode �ַ�����д���ֽ� 
    virtual bool WriteLog(const wchar_t *str, const wchar_t *strSuffix = L""); // Ansi �ַ�����д���ֽ�
    String GetLastErrorMessage(void);

protected:
    String Format(const TCHAR *pszFmt, ...);
    std::wstring StringToWString(const std::string &s);  // ���ֽ� ת ���ֽ�
    std::string WStringToString(const std::wstring &ws); // ���ֽ� ת ���ֽ�

protected:
    HANDLE m_hMutex;
    String m_strFilePath;
    String m_strErrorMessage;
};


