#pragma once
#include <vector>
#include "dllexport.h"

namespace UtilTools
{
    DLL_API String toString(int n);
    DLL_API String toString(long n);
    DLL_API String toString(unsigned int n);
    DLL_API String toString(unsigned long n);
    DLL_API String toString(double f, bool bTrimZero);
    DLL_API String toString(double f, int nCntAfterMark = -1); // 小数位数

    DLL_API int toInt(const String &s);
    DLL_API long toLong(const String &s);
    DLL_API float toFloat(const String &s);
    DLL_API double toDouble(const String &s);

    DLL_API String format(const TCHAR *pszFmt, ...); // 格式化字符串
    DLL_API std::wstring StringToWString(const std::string &s);  // 多字节 转 宽字节
    DLL_API std::string WStringToString(const std::wstring &ws); // 宽字节 转 多字节

    DLL_API std::vector<String> SpiltString(const String &s, TCHAR delimiter, bool bRemoveEmptyEntries = false);
    DLL_API std::vector<String> SpiltString(const String &s, const String &delimiters, bool bRemoveEmptyEntries = false, bool bDelimitersAsMulSeparator = false);

    DLL_API bool isEmpty(const String &s); // 是否为空
    DLL_API bool isBlank(const String &s); // 是否为空 or 字符串中的字符都为空白符（空格、制表符\t、换行符\n）

    DLL_API bool equals(const String &s1, const String &s2);
    DLL_API bool contains(const String &s, const String &subs);
    DLL_API bool endsWith(const String &s, const String &subs);
    DLL_API bool startsWith(const String &s, const String &subs);
};
