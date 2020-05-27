#include "StdAfx.h"
#include "IniHelper.h"
#include <sstream>     // std::stringstream

#ifndef USE_STRINGUTILS
#include "StringUtils.h"
namespace UtilTools
{
    bool IniHelper::read(LPCTSTR lpIniFileName, LPCTSTR lpSection, LPCTSTR lpKey, int defaultValue, int &value)
    {
        TCHAR mDataStr[MAX_PATH];
        DWORD strLength = ::GetPrivateProfileString(lpSection, lpKey, NULL, mDataStr, sizeof(mDataStr)-1, lpIniFileName);
        bool convsuccess = StringUtils::isNumeric(mDataStr);
        value = convsuccess ? _ttoi(mDataStr) : defaultValue;
        return convsuccess;
    }

    bool IniHelper::read(LPCTSTR lpIniFileName, LPCTSTR lpSection, LPCTSTR lpKey, double defaultValue, double &value)
    {
        TCHAR mDataStr[MAX_PATH];
        DWORD strLength = ::GetPrivateProfileString(lpSection, lpKey, NULL, mDataStr, sizeof(mDataStr)-1, lpIniFileName);
        bool convsuccess = StringUtils::isNumeric(mDataStr);
        value = convsuccess ? _ttof(mDataStr) : defaultValue;
        return convsuccess;
    }

    int IniHelper::read(LPCTSTR lpIniFileName, LPCTSTR lpSection, LPCTSTR lpKey, int defaultValue, bool *pRet)
    {
        TCHAR mDataStr[MAX_PATH];
        DWORD strLength = ::GetPrivateProfileString(lpSection, lpKey, NULL, mDataStr, sizeof(mDataStr)-1, lpIniFileName);
        bool convsuccess = StringUtils::isNumeric(mDataStr);
        int value = convsuccess ? _ttoi(mDataStr) : defaultValue;
        if (pRet && convsuccess) {
            *pRet = convsuccess;
        }
        return value;
    }

    double IniHelper::read(LPCTSTR lpIniFileName, LPCTSTR lpSection, LPCTSTR lpKey, double defaultValue, bool *pRet)
    {
        TCHAR mDataStr[MAX_PATH];
        DWORD strLength = ::GetPrivateProfileString(lpSection, lpKey, NULL, mDataStr, sizeof(mDataStr)-1, lpIniFileName);
        bool convsuccess = StringUtils::isNumeric(mDataStr);
        double value = convsuccess ? _ttoi(mDataStr) : defaultValue;
        if (pRet && convsuccess) {
            *pRet = convsuccess;
        }
        return value;
    }
}
#endif

namespace UtilTools
{
    /// 读取无符号整型
    UINT readInt(LPCTSTR lpIniFileName, LPCTSTR lpSection, LPCTSTR lpKey, int defaultValue)
    {
        return ::GetPrivateProfileInt(lpSection, lpKey, defaultValue, lpIniFileName);
    }

    /// 读取字符串
    String readString(LPCTSTR lpIniFileName, LPCTSTR lpSection, LPCTSTR lpKey, LPTSTR defaultValue)
    {
        TCHAR mDataStr[MAX_PATH + 1] = { '\0' };
        DWORD strLength = ::GetPrivateProfileString(lpSection, lpKey, defaultValue, mDataStr, sizeof(mDataStr)-1, lpIniFileName);
        return mDataStr;
    }
}

namespace UtilTools
{
    int IniHelper::readInt(LPCTSTR lpIniFileName, LPCTSTR lpSection, LPCTSTR lpKey)
    {
        TCHAR mDataStr[MAX_PATH];
        DWORD strLength = ::GetPrivateProfileString(lpSection, lpKey, NULL, mDataStr, sizeof(mDataStr)-1, lpIniFileName);
        int cfgOfInt = _ttoi(mDataStr);
        return cfgOfInt;
    }

    double IniHelper::readDouble(LPCTSTR lpIniFileName, LPCTSTR lpSection, LPCTSTR lpKey)
    {
        TCHAR mDataStr[MAX_PATH];
        DWORD strLength = ::GetPrivateProfileString(lpSection, lpKey, NULL, mDataStr, sizeof(mDataStr)-1, lpIniFileName);
        double cfgOfInt = _ttof(mDataStr);
        return cfgOfInt;
    }

    String IniHelper::readString(LPCTSTR lpIniFileName, LPCTSTR lpSection, LPCTSTR lpKey)
    {
        TCHAR mDataStr[MAX_PATH + 1] = { '\0' };
        DWORD strLength = ::GetPrivateProfileString(lpSection, lpKey, NULL, mDataStr, sizeof(mDataStr)-1, lpIniFileName);
        return mDataStr;
    }
}

namespace UtilTools
{
    template<class T>
    String toString(T value)
    {
#ifdef UNICODE
        std::wstringstream ss;
#else
        std::stringstream ss;
#endif
        ss << value;
        return ss.str();
    }

    bool IniHelper::write(LPCTSTR lpIniFileName, LPCTSTR lpSection, LPCTSTR lpKey, const int &value)
    {
        String s = toString(value);
        return !!WritePrivateProfileString(lpSection, lpKey, s.c_str(), lpIniFileName);
    }

    bool IniHelper::write(LPCTSTR lpIniFileName, LPCTSTR lpSection, LPCTSTR lpKey, const double &value)
    {
        String s = toString(value);
        return !!WritePrivateProfileString(lpSection, lpKey, s.c_str(), lpIniFileName);
    }

    bool IniHelper::write(LPCTSTR lpIniFileName, LPCTSTR lpSection, LPCTSTR lpKey, const String &value)
    {
        return !!WritePrivateProfileString(lpSection, lpKey, value.c_str(), lpIniFileName);
    }
}
