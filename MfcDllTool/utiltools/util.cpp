#include "StdAfx.h"
#include "util.h"
#include <string>      // std::string
#include <sstream>     // std::stringstream
#include <codecvt>
#include <assert.h>
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

String format(const TCHAR *pszFmt, ...)
{
    assert(pszFmt);
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

vector<String> SpiltString(const String &s, TCHAR delimiter, bool bRemoveEmptyEntries)
{
    String si;
    vector<String> sv;
    istringstream iss(s);
    while (getline(iss, si, delimiter)) {
        if (bRemoveEmptyEntries && !si.length()) {
            continue;
        }
        sv.push_back(si);
    }

    // 处理字符串为空 or 以分隔符结尾的情况
    if (s.empty() || s.back() == delimiter) {
        sv.push_back(_T(""));
    }

    return sv;
}

// delimiters - “整体作为一个分隔符”
static vector<String> SplitUseOneSeparator(const String &s, const String &delimiters)
{
    String si;
    vector<String> sv;

    // 处理空串、空分割符的情况
    if (s.empty() || delimiters.empty()) {
        sv.push_back(_T(""));
        return sv;
    }

    string::size_type startpos = 0;
    string::size_type endpos = s.find(delimiters);
    while(string::npos != endpos) {
        sv.push_back(s.substr(startpos, endpos - startpos));
        startpos = endpos + delimiters.size();
        endpos = s.find(delimiters, startpos);
    }

    if(s.length() != startpos) {
        sv.push_back(s.substr(startpos)); // 截取剩余字符串
    } else {
        sv.push_back(_T("")); // 处理字符串以分隔符结尾的情况
    }

    return sv;
}

// delimiters - “每个字符作为一个分隔符”
static vector<String> SplitUseMulSeparator(const String &s, const String &delimiters)
{
    String si;
    vector<String> sv;

    typedef string::size_type string_size;
    string_size startpos = 0;
    string_size endpos = 0;

    bool found = true;
    while(found && s.size() != startpos) {
        // 查找分隔符
        found = false;
        while(!found && s.size() != endpos) {
            for(string_size i = 0; i < delimiters.size(); ++i) {
                if (s[endpos] == delimiters[i]) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                ++endpos;
            }
        }

        // 分割字符串
        if (found) {
            sv.push_back(s.substr(startpos, endpos - startpos));
            startpos = ++endpos;
        } else {
            sv.push_back(s.substr(startpos)); // 截取剩余字符串
        }
    }

    // 处理字符串以分隔符结尾的情况
    if (found && s.size() == startpos) {
        sv.push_back(_T(""));
    }

    return sv;
}

vector<String> SpiltString(const String &s, const String &delimiters, bool bRemoveEmptyEntries, bool bDelimitersAsMulSeparator)
{
    vector<String> sv;
    vector<String> _sv;

    if (bDelimitersAsMulSeparator) {
        _sv = SplitUseMulSeparator(s, delimiters);
    } else {
        _sv = SplitUseOneSeparator(s, delimiters);
    }

    // 去除空字符串
    if (bRemoveEmptyEntries) {
        vector<String>::const_iterator it = _sv.cbegin();
        while (it != _sv.cend()) {
            const String &si = *it++;
            if (bRemoveEmptyEntries && !si.length()) {
                continue;
            }
            sv.push_back(si);
        }
    } else {
        return _sv;
    }

    return sv;
}

bool isEmpty(const String &s)
{
    return s.empty();
}

// 判断是否为空白符
static bool isWhitespace(TCHAR ch)
{
    return ch == ' ' || ch == '\t' || ch == '\n';
}

bool isBlank(const String &s)
{
    size_t length = s.length();
    if (length == 0) {
        return true;
    }

    for(size_t i = 0; i < length; ++i) {
        // 判断字符是否为空格、制表符、换行符
        if (!isWhitespace(s.at(i))) {
            return false;
        }
    }
    return true;
}

bool equals(const String &s1, const String &s2)
{
    return s1 == s2;
    //return s1.compare(s2) == 0;
}

bool contains(const String &s, const String &subs)
{
    return s.find(subs) != string::npos;
}

bool endsWith(const String &s, const String &subs)
{
    return subs.empty() || (s.find(subs) != string::npos) && (s.find(subs) == s.length() - subs.length());
}

bool startsWith(const String &s, const String &subs)
{
    return s.find(subs) == 0;
}

String trim(const String &s)
{
    std::size_t idx1 = s.find_first_not_of(_T(" "));
    std::size_t idx2 = s.find_last_not_of(_T(" "));

    if (string::npos == idx1 && string::npos == idx2) {
        return _T(""); // 处理空串
    } else if (0 == idx1 && s.length() - 1 == idx2) {
        return s; // 处理无空白符的情况
    }

    String _s = s;
    if (string::npos != idx1) {
        _s.erase(0, idx1);
    }
    if (string::npos != idx2) {
        _s.erase(idx2 + 1);
    }
    return _s;
}

String trimLeft(const String &s)
{
    std::size_t idx = s.find_first_not_of(_T(" "));
    if (string::npos != idx) {
        String _s = s;
        _s.erase(0, idx);
        return _s;
    }
    return (s.find_first_of(_T(" ")) != 0) ? s : _T(""); // 处理空串
}

String trimRight(const String &s)
{
    std::size_t idx = s.find_last_not_of(_T(" "));
    if (string::npos != idx) {
        String _s = s;
        _s.erase(idx + 1);
        return _s;
    }
    return (s.find_last_of(_T(" ")) != s.length() - 1) ? s : _T(""); // 处理空串
}
}
