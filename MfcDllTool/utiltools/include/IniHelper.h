#pragma once
#include "dllexport.h"
#include <vector>
#include <map>

namespace UtilTools
{
    UINT   readInt(LPCTSTR lpIniFileName, LPCTSTR lpSection, LPCTSTR lpKey, int defaultValue);
    String readString(LPCTSTR lpIniFileName, LPCTSTR lpSection, LPCTSTR lpKey, LPTSTR defaultValue);

    class DLL_API IniHelper
    {
    public:
        // 读写基础数据
        STATIC int    readInt(LPCTSTR lpIniFileName, LPCTSTR lpSection, LPCTSTR lpKey);
        STATIC double readDouble(LPCTSTR lpIniFileName, LPCTSTR lpSection, LPCTSTR lpKey);
        STATIC String readString(LPCTSTR lpIniFileName, LPCTSTR lpSection, LPCTSTR lpKey);
        STATIC bool   write(LPCTSTR lpIniFileName, LPCTSTR lpSection, LPCTSTR lpKey, const int &value);
        STATIC bool   write(LPCTSTR lpIniFileName, LPCTSTR lpSection, LPCTSTR lpKey, const double &value);
        STATIC bool   write(LPCTSTR lpIniFileName, LPCTSTR lpSection, LPCTSTR lpKey, const String &value);

        // 读写复杂结构
        STATIC bool   read(LPCTSTR lpIniFileName, LPCTSTR lpSection, LPCTSTR lpKey, LPVOID lpStruct, UINT uSizeStruct);
        STATIC bool   write(LPCTSTR lpIniFileName, LPCTSTR lpSection, LPCTSTR lpKey, LPVOID lpStruct, UINT uSizeStruct);

        // 包含 StringUtils 类，可使用以下函数
#ifndef USE_STRINGUTILS
        STATIC bool   read(LPCTSTR lpIniFileName, LPCTSTR lpSection, LPCTSTR lpKey, int defaultValue, int &value);
        STATIC bool   read(LPCTSTR lpIniFileName, LPCTSTR lpSection, LPCTSTR lpKey, double defaultValue, double &value);
        STATIC int    read(LPCTSTR lpIniFileName, LPCTSTR lpSection, LPCTSTR lpKey, int defaultValue, bool *pRet = NULL);
        STATIC double read(LPCTSTR lpIniFileName, LPCTSTR lpSection, LPCTSTR lpKey, double defaultValue, bool *pRet = NULL);
#endif
        // 解析 Section && Key && Field
        STATIC std::vector<String> readSectionNames(LPCTSTR lpIniFileName);
        STATIC std::vector<String> readKeys(LPCTSTR lpIniFileName, LPCTSTR lpSection);
        STATIC std::vector<String> readFields(LPCTSTR lpIniFileName, LPCTSTR lpSection);
        STATIC std::map<String, String> parseFields(LPCTSTR lpIniFileName, LPCTSTR lpSection);
    };
}
