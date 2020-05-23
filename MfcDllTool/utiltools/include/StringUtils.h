#pragma once
#include <vector>
#include "dllexport.h"

#define VAR(str) str ## var
#define CONV_PTR(str, ptr, func) \
    string VAR(str) = func(str); \
    const char * ptr = VAR(str).c_str()

// 提供 2 个宏，处理字符串转换，并防止临时对象析构，造成内存非法访问
#ifdef UTILTOOLS_EXPORTS_STATIC
#define S2WS_PTR(str, ptr)  CONV_PTR(str, ptr, UtilTools::StringUtils::stringToWString)
#define WS2S_PTR(wstr, ptr) CONV_PTR(wstr, ptr, UtilTools::StringUtils::wstringToString)
#endif

namespace UtilTools
{
    class DLL_API StringUtils
    {
    public:
        // 转字符串
        STATIC String toString(int n);
        STATIC String toString(long n);
        STATIC String toString(unsigned int n);
        STATIC String toString(unsigned long n);
        STATIC String toString(double f, bool bTrimZero);
        STATIC String toString(double f, int nCntAfterMark = -1); // 小数位数

        // 字符串转数字
        STATIC int toInt(const String &s);
        STATIC long toLong(const String &s);
        STATIC float toFloat(const String &s);
        STATIC double toDouble(const String &s);

        // 字符串转换 && 支持汉字
        STATIC String format(const TCHAR *pszFmt, ...);
        STATIC std::wstring stringToWString(const std::string &s);
        STATIC std::string wstringToString(const std::wstring &ws);
        STATIC std::string& wstringToString(const std::wstring &ws, std::string &s);
        STATIC std::wstring& stringToWString(const std::string &s, std::wstring &ws);

        // 分割
        STATIC std::vector<String> spilt(const String &s, TCHAR delimiter, bool bRemoveEmptyEntries = false);
        STATIC std::vector<String> spilt(const String &s, const String &delimiters, bool bRemoveEmptyEntries = false, bool bDelimitersAsMulSeparator = false);

        // 空(白)串判断
        STATIC bool isEmpty(const String &s); // 是否为空
        STATIC bool isBlank(const String &s); // 是否为空 or 字符串中的字符都为空白符

        // 包含判断
        STATIC bool equals(const String &s1, const String &s2);
        STATIC bool contains(const String &s, const String &subs);
        STATIC bool endsWith(const String &s, const String &subs);
        STATIC bool startsWith(const String &s, const String &subs);

        // 去除首、尾空白符
        STATIC String trim(const String &s);
        STATIC String trimLeft(const String &s);
        STATIC String trimRight(const String &s);

        // 判断是否为字母
        STATIC bool isAlpha(const String &s);
        STATIC bool isLower(const String &s);
        STATIC bool isUpper(const String &s);

        // 大小写转换
        STATIC String toLower(const String &s);
        STATIC String toUpper(const String &s);

        // 判断是否为数字
        STATIC bool isBoolean(const String &s);
        STATIC bool isInteger(const String &s);
        STATIC bool isNumeric(const String &s);

        // 解析数字
        STATIC bool parseInteger(const String &s, int &value);
        STATIC bool parseBoolean(const String &s, bool &value);
        STATIC bool parseNumeric(const String &s, double &value);

        STATIC int parseInteger(const String &s, bool *ok = NULL);
        STATIC bool parseBoolean(const String &s, bool *ok = NULL);
        STATIC double parseNumeric(const String &s, bool *ok = NULL);

        // 转十六进制
        STATIC String toHexWString(wchar_t ch); // 支持汉字字符
        STATIC String toHexString(unsigned char ch); // 不支持汉字字符
        STATIC String toHexString(const String &s, const String &separator = _T(""));
    };
}
