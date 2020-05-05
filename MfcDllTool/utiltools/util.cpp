#include "StdAfx.h"
#include "util.h"
//#include <string>      // std::string
//#include <sstream>     // std::stringstream
#include <codecvt>
using namespace std;

namespace UtilTools
{
String toString(int n)
{
    return format(_T("%d"), n);
}

String toString(long n)
{
    return format(_T("%ld"), n);
}

String toString(unsigned int n)
{
    return format(_T("%u"), n);
}

String toString(unsigned long n)
{
    return format(_T("%lu"), n);
}

String toString(double f, bool bTrimZero)
{
    if (bTrimZero) {
        return format(_T("%g"), f); // 去掉多余的 0
    } else {
        return format(_T("%f"), f); // 不足6位小数，后面补0
    }
}

String toString(double f, int nCntAfterMark)
{
    return format(_T("%.*f"), (nCntAfterMark < 0) ? 0 : nCntAfterMark, f);
}

int toInt(const String &s)
{
    return _ttoi(s.c_str());
}

long toLong(const String &s)
{
    return _ttol(s.c_str());
}

float toFloat(const String &s)
{
    return (float)toDouble(s);
}

double toDouble(const String &s)
{
    return _ttof(s.c_str());
}

// 格式化字符串
String format(const TCHAR *pszFmt, ...)
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

wstring StringToWString(const string &s)
{
    //std::wstring ws(s.begin(), s.end());
    //return ws;
    wstring_convert<codecvt_utf8_utf16<wchar_t>> converter;
    return converter.from_bytes(s);
}

string WStringToString(const wstring &ws)
{
    wstring_convert<codecvt_utf8_utf16<wchar_t>> converter;
    //string narrow = converter.to_bytes(wide_utf16_source_string);
    //wstring wide = converter.from_bytes(narrow_utf8_source_string);
    return converter.to_bytes(ws);
}
}
