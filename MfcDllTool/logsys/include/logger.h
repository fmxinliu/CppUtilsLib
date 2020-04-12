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
    virtual bool WriteLog(const char *strMessage, const char *strSuffix = ""); // Unicode 字符集下写多字节 
    virtual bool WriteLog(const wchar_t *str, const wchar_t *strSuffix = L""); // Ansi 字符集下写宽字节
    String GetLastErrorMessage(void);

protected:
    String Format(const TCHAR *pszFmt, ...);
    std::wstring StringToWString(const std::string &s);  // 多字节 转 宽字节
    std::string WStringToString(const std::wstring &ws); // 宽字节 转 多字节

protected:
    HANDLE m_hMutex;
    String m_strFilePath;
    String m_strErrorMessage;
};


