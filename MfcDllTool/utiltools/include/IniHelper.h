#pragma once
#include "dllexport.h"

namespace UtilTools
{
    UINT   readInt(LPCTSTR lpIniFileName, LPCTSTR lpSection, LPCTSTR lpKey, int defaultValue);
    String readString(LPCTSTR lpIniFileName, LPCTSTR lpSection, LPCTSTR lpKey, LPTSTR defaultValue);

    class DLL_API IniHelper
    {
    public:
        int    readInt(LPCTSTR lpIniFileName, LPCTSTR lpSection, LPCTSTR lpKey);
        double readDouble(LPCTSTR lpIniFileName, LPCTSTR lpSection, LPCTSTR lpKey);
        String readString(LPCTSTR lpIniFileName, LPCTSTR lpSection, LPCTSTR lpKey);
        bool   write(LPCTSTR lpIniFileName, LPCTSTR lpSection, LPCTSTR lpKey, const int &value);
        bool   write(LPCTSTR lpIniFileName, LPCTSTR lpSection, LPCTSTR lpKey, const double &value);
        bool   write(LPCTSTR lpIniFileName, LPCTSTR lpSection, LPCTSTR lpKey, const String &value);

    public:
#ifndef USE_STRINGUTILS
        bool   read(LPCTSTR lpIniFileName, LPCTSTR lpSection, LPCTSTR lpKey, int defaultValue, int &value);
        bool   read(LPCTSTR lpIniFileName, LPCTSTR lpSection, LPCTSTR lpKey, double defaultValue, double &value);
        int    read(LPCTSTR lpIniFileName, LPCTSTR lpSection, LPCTSTR lpKey, int defaultValue, bool *pRet = NULL);
        double read(LPCTSTR lpIniFileName, LPCTSTR lpSection, LPCTSTR lpKey, double defaultValue, bool *pRet = NULL);
#endif
    };
}
