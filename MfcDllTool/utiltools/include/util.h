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
    DLL_API bool isBlank(const String &s); // 是否为空 or 字符串中的字符都为空白符

    DLL_API bool equals(const String &s1, const String &s2);
    DLL_API bool contains(const String &s, const String &subs);
    DLL_API bool endsWith(const String &s, const String &subs);
    DLL_API bool startsWith(const String &s, const String &subs);

    // 去除首、尾空白符
    DLL_API String trim(const String &s);
    DLL_API String trimLeft(const String &s);
    DLL_API String trimRight(const String &s);

    // 判断是否为字母
    DLL_API bool isAlpha(const String &s);
    DLL_API bool isLower(const String &s);
    DLL_API bool isUpper(const String &s);

    // 大小写转换
    DLL_API String toLower(const String &s);
    DLL_API String toUpper(const String &s);

    // 判断是否为数字
    DLL_API bool isBoolean(const String &s);
    DLL_API bool isInteger(const String &s);
    DLL_API bool isNumeric(const String &s);

    // 解析数字
    DLL_API bool parseInteger(const String &s, int &value);
    DLL_API bool parseBoolean(const String &s, bool &value);
    DLL_API bool parseNumeric(const String &s, double &value);

    DLL_API int parseInteger(const String &s, bool *ok = NULL);
    DLL_API bool parseBoolean(const String &s, bool *ok = NULL);
    DLL_API double parseNumeric(const String &s, bool *ok = NULL);
};
