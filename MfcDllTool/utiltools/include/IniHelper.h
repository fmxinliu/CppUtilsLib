#pragma once
#include "dllexport.h"
#include <vector>
#include <map>

namespace UtilTools
{
    class DLL_API IniHelper
    {
    public:
        // 读写基础数据
        STATIC int    readInt(LPCTSTR lpIniFileName, LPCTSTR lpSection, LPCTSTR lpKey);
        STATIC double readDouble(LPCTSTR lpIniFileName, LPCTSTR lpSection, LPCTSTR lpKey);
        STATIC String readString(LPCTSTR lpIniFileName, LPCTSTR lpSection, LPCTSTR lpKey);

        // 读写struct结构（注意结构体，嵌套String）
        STATIC bool   read(LPCTSTR lpIniFileName, LPCTSTR lpSection, LPCTSTR lpKey, LPVOID lpStruct, UINT uSizeStruct);
        STATIC bool   write(LPCTSTR lpIniFileName, LPCTSTR lpSection, LPCTSTR lpKey, LPVOID lpStruct, UINT uSizeStruct);

        // 读
        STATIC bool   read(LPCTSTR lpIniFileName, LPCTSTR lpSection, LPCTSTR lpKey, int defaultValue, int &value);
        STATIC bool   read(LPCTSTR lpIniFileName, LPCTSTR lpSection, LPCTSTR lpKey, double defaultValue, double &value);
        STATIC int    read(LPCTSTR lpIniFileName, LPCTSTR lpSection, LPCTSTR lpKey, int defaultValue, bool *pRet = NULL);
        STATIC double read(LPCTSTR lpIniFileName, LPCTSTR lpSection, LPCTSTR lpKey, double defaultValue, bool *pRet = NULL);

        // 写
        STATIC bool   write(LPCTSTR lpIniFileName, LPCTSTR lpSection, LPCTSTR lpKey, const int &value);
        STATIC bool   write(LPCTSTR lpIniFileName, LPCTSTR lpSection, LPCTSTR lpKey, const double &value);
        STATIC bool   write(LPCTSTR lpIniFileName, LPCTSTR lpSection, LPCTSTR lpKey, const String &value);
        STATIC bool   write(LPCTSTR lpIniFileName, LPCTSTR lpSection, const std::map<String, String> &value);
        STATIC bool   write(LPCTSTR lpIniFileName, const std::map<String, std::map<String, String>> &value);

        // Section & Key
        STATIC std::vector<String> readSectionNames(LPCTSTR lpIniFileName);
        STATIC std::vector<String> readKeys(LPCTSTR lpIniFileName, LPCTSTR lpSection);
        STATIC std::map<String, String> readSection(LPCTSTR lpIniFileName, LPCTSTR lpSection);
        STATIC std::map<String, std::map<String, String>> readSections(LPCTSTR lpIniFileName);

        // Section
        STATIC bool sectionExist(LPCTSTR lpIniFileName, LPCTSTR lpSection);
        STATIC bool removeSection(LPCTSTR lpIniFileName, LPCTSTR lpSection);

        // Key
        STATIC bool keyExist(LPCTSTR lpIniFileName, LPCTSTR lpSection, LPCTSTR lpKey);
        STATIC bool removeKey(LPCTSTR lpIniFileName, LPCTSTR lpSection, LPCTSTR lpKey);
    };
}
