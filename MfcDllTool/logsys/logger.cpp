#include "stdafx.h"
#include "logger.h"
#include <vector>
#include <locale>
#include <codecvt>
#include <string>

using namespace std;

Logger::Logger()
    : m_strFilePath(_T(""))
    , m_strErrorMessage(_T(""))
{
    InitLogFile();
}

Logger::~Logger()
{
    if (m_hMutex != NULL)
    {
        CloseHandle(m_hMutex);
        m_hMutex = NULL;
    }
}

bool Logger::InitLogFile(void)
{
    TCHAR szFilePath[MAX_PATH];
    TCHAR szPathName[MAX_PATH];
    TCHAR szDriver[MAX_PATH];
    TCHAR szDirectory[MAX_PATH];
    TCHAR szFileName[MAX_PATH];
    TCHAR szExt[MAX_PATH];
    TCHAR szMutexName[MAX_PATH];

    ZeroMemory(szFilePath, MAX_PATH * sizeof(TCHAR));
    ZeroMemory(szPathName, MAX_PATH * sizeof(TCHAR));
    ZeroMemory(szDriver, MAX_PATH * sizeof(TCHAR));
    ZeroMemory(szDirectory, MAX_PATH * sizeof(TCHAR));
    ZeroMemory(szFileName, MAX_PATH * sizeof(TCHAR));
    ZeroMemory(szExt, MAX_PATH * sizeof(TCHAR));
    ZeroMemory(szMutexName, MAX_PATH * sizeof(TCHAR));

    GetModuleFileName(NULL, szPathName, MAX_PATH);
    _tsplitpath_s(szPathName, szDriver, _MAX_DRIVE, szDirectory, _MAX_DIR, szFileName, _MAX_FNAME, szExt, _MAX_EXT);
    wsprintf(szFilePath, _T("%s%slog"), szDriver, szDirectory);
    CreateDirectory(szFilePath, NULL);

    m_strFilePath = szFilePath;

    m_hMutex = CreateMutex(NULL, FALSE, _T("Mutex{66656B0B-232C-49A6-B268-084EFA164E3B}"));
    if (m_hMutex == NULL)
    {
        DWORD dwError = GetLastError();
        m_strErrorMessage = Format(_T("CreateMutex Error. Error code : %d"), dwError); 
        return false;
    }

    return true;
}

String Logger::GetLastErrorMessage(void)
{
    return m_strErrorMessage;
}

bool Logger::WriteLog(DWORD dwError, String strExtraInfo)
{
    String strErrorCode = Format(_T(". Error Code : %d"), dwError);
    String strErrorMessage = strErrorCode + strExtraInfo;
    return WriteLog(strErrorMessage);
}

bool Logger::WriteLog(String strMessage, String strSuffix)
{
    CStdioFile logfile;
    DWORD dwError = 0;

    // 获取当前时间
    SYSTEMTIME st = { 0 };
    TCHAR szFormatDate[] = _T("%04u%02u%02u");
    TCHAR szFormatTime[] = _T("%02u:%02u:%02u.%03u");
    TCHAR szCurDate[32] = { 0 };
    TCHAR szCurTime[32] = { 0 };
    GetLocalTime(&st);
    wsprintf(szCurDate, szFormatDate, st.wYear, st.wMonth, st.wDay);
    wsprintf(szCurTime, szFormatTime, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);

    // 构建文件名
    String strFileSuffix = (strSuffix.length() > 0) ? (_T("-") + strSuffix).data() : _T("");
    String strFileName = Format(_T("%s\\%s%s.txt"), m_strFilePath.data(), szCurDate, strFileSuffix.data());

    WaitForSingleObject(m_hMutex, INFINITE);

    try
    {
        if (!logfile.Open(strFileName.data(), CFile::modeReadWrite | CFile::modeNoTruncate | CFile::modeCreate))
        {
            dwError = GetLastError();
            ReleaseMutex(m_hMutex);
            m_strErrorMessage = Format(_T("CreateFile Error! Error Code : %d"), dwError);
            return false;
        }

        logfile.SeekToEnd();

        String strMessageTemp = Format(_T("%s: %s\r\n"), szCurTime, strMessage.data());
        _tprintf(_T("%s"), strMessageTemp.data());

        TCHAR *pOldLocale = _tcsdup(_tsetlocale(LC_CTYPE, NULL));
        _tsetlocale(LC_CTYPE, _T("chs"));
        logfile.WriteString(strMessageTemp.data());
        _tsetlocale(LC_CTYPE, pOldLocale);
        free(pOldLocale);
    }
    catch (...)
    {
        dwError = GetLastError();
        if (logfile.m_hFile != INVALID_HANDLE_VALUE)
        {
            logfile.Close();
        }
        ReleaseMutex(m_hMutex);
        m_strErrorMessage = Format(_T("WriteLog Exception! Exception Code : %d"), dwError);
        return false;
    }

    if (logfile.m_hFile != INVALID_HANDLE_VALUE)
    {
        logfile.Close();
    }
    ReleaseMutex(m_hMutex);

    return true;
}

bool Logger::WriteLog(const char *str, const char *suffix)
{
    String strMessage;
    String strSuffix;

#if !defined(UNICODE)
    strMessage = str;
    strSuffix = suffix;
#else
    strMessage = StringToWString(str);
    strSuffix = StringToWString(suffix);
#endif

    return WriteLog(strMessage, strSuffix);
}

bool Logger::WriteLog(const wchar_t *str, const wchar_t *suffix)
{
    String strMessage;
    String strSuffix;

#if defined(UNICODE)
    strMessage = str;
    strSuffix = suffix;
#else
    strMessage = WStringToString(str);
    strSuffix = WStringToString(suffix);
#endif

    return WriteLog(strMessage, strSuffix);
}

/**
 * 功能：格式化字符串
 * 参数：
 *  @pszFmt，格式描述
 *  @...，不定参数
 * 返回值：格式化的结果字符串
 */
String Logger::Format(const TCHAR *pszFmt, ...)
{
    String str;
    va_list args;
    va_start(args, pszFmt);
    {
        size_t nLength = _vsctprintf(pszFmt, args);
        nLength += 1;  //上面返回的长度是不包含\0，这里加上
        vector<TCHAR> vectorChars(nLength);
        _vsntprintf(vectorChars.data(), nLength, pszFmt, args);
        str.assign(vectorChars.data());
    }
    va_end(args);
    return str;
}

wstring Logger::StringToWString(const string &s)
{
    //std::wstring ws(s.begin(), s.end());
    //return ws;
    wstring_convert<codecvt_utf8_utf16<wchar_t>> converter;
    return converter.from_bytes(s);
}

string Logger::WStringToString(const wstring &ws)
{
    wstring_convert<codecvt_utf8_utf16<wchar_t>> converter;
    //string narrow = converter.to_bytes(wide_utf16_source_string);
    //wstring wide = converter.from_bytes(narrow_utf8_source_string);
    return converter.to_bytes(ws);
}
