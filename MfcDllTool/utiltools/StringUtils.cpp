#include "StdAfx.h"
#include "char.h"
#include "StringUtils.h"
#include <string>      // std::string
#include <sstream>     // std::stringstream
#include <codecvt>
#include <assert.h>
using namespace std;

namespace UtilTools
{
    static vector<String> SplitUseOneSeparator(const String &s, const String &delimiters);
    static vector<String> SplitUseMulSeparator(const String &s, const String &delimiters);

    String StringUtils::toString(int n)
    {
        return format(_T("%d"), n);
    }

    String StringUtils::toString(long n)
    {
        return format(_T("%ld"), n);
    }

    String StringUtils::toString(unsigned int n)
    {
        return format(_T("%u"), n);
    }

    String StringUtils::toString(unsigned long n)
    {
        return format(_T("%lu"), n);
    }

    String StringUtils::toString(double f, bool bTrimZero)
    {
        if (bTrimZero) {
            return format(_T("%g"), f); // 去掉多余的 0
        } else {
            return format(_T("%f"), f); // 不足6位小数，后面补0
        }
    }

    String StringUtils::toString(double f, int nCntAfterMark)
    {
        return format(_T("%.*f"), (nCntAfterMark < 0) ? 0 : nCntAfterMark, f);
    }

    int StringUtils::toInt(const String &s)
    {
        return _ttoi(s.c_str());
    }

    long StringUtils::toLong(const String &s)
    {
        return _ttol(s.c_str());
    }

    float StringUtils::toFloat(const String &s)
    {
        return (float)toDouble(s);
    }

    double StringUtils::toDouble(const String &s)
    {
        return _ttof(s.c_str());
    }

    String StringUtils::format(const TCHAR *pszFmt, ...)
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

    wstring StringUtils::stringToWString(const string &s)
    {
        wstring ws;
        ws = stringToWString(s, ws);
        return ws;
    }

    string StringUtils::wstringToString(const wstring &ws)
    {
        string s;
        s = wstringToString(ws, s);
        return s;
    }

    std::string& StringUtils::wstringToString(const wstring &ws, string &s)
    {
        char *curLocale = setlocale(LC_ALL, NULL); // 查看当前地域设置
        setlocale(LC_ALL, ""); // 使用当前操作系统默认的地域设置

        const wchar_t *source = ws.c_str();
        //size_t len = wcstombs(NULL, source, 0);
        size_t len = ws.length();
        if (len <= 0) {
            return s;
        }
        char *buffer = new char[len + 1];
        if (NULL == buffer) {
            return s;
        }

        size_t c = 0;
        size_t n = 0;
        while ((c < len) && (n = wcstombs(buffer + c, source + c, len - c)) != -1) {
            n++;
            c+=n;
        }

        buffer[len] = '\0';
        s.assign(buffer, len);
        delete[] buffer;

        setlocale(LC_ALL, curLocale); // 恢复地域设置
        return s;
    }

    wstring& StringUtils::stringToWString(const string &s, wstring &ws)
    {
        char *curLocale = setlocale(LC_ALL, NULL); // 查看当前地域设置
        setlocale(LC_ALL, ""); // 使用当前操作系统默认的地域设置

        const char *source = s.c_str();
        //size_t len = mbstowcs(NULL, source, 0);
        size_t len = s.length();
        if (len <= 0) {
            return ws;
        }

        wchar_t *buffer = new wchar_t[len + 1];
        if (NULL == buffer) {
            return ws;
        }

        size_t c = 0;
        size_t n = 0;
        while ((c < len) && (n = mbstowcs(buffer + c, source + c, len - c)) != -1) {
            n++;
            c+=n;
        }

        buffer[len] = '\0';
        ws.assign(buffer, len);
        delete[] buffer;

        setlocale(LC_ALL, curLocale); // 恢复地域设置
        return ws;
    }

    wstring& stringToWString(const string &s, wstring &ws)
    {
        char *pc = "1我2";
        int lpc = strlen(pc);
        wchar_t *wpc = L"1我2";
        int lwpc = wcslen(wpc);

        ///
        char *curLocale = setlocale(LC_ALL, NULL); // 查看当前地域设置
        setlocale(LC_ALL, ""); // 使用当前操作系统默认的地域设置

        //char str[8] = { '1', '2', '3', '我', '0', '5', '我', '1' };
        //string s1(str, 8);
        string s1("我你他1");

        const char *source = s1.c_str();
        vector<DWORD> lengthVec;
        size_t pi = 0;
        DWORD len = 0;

        // 检查字符串中间的的 '\0'
        // 如: char str[8] = { '1', '2', '3', '我', '\0', '5', '我', '0' };
        for (size_t i = 0; i < s1.length(); ++i) {
            if (s1[i] == '\0') {
                DWORD dwSize = MultiByteToWideChar(CP_ACP, 0, source + pi, -1, NULL, 0);
                lengthVec.push_back(dwSize);
                len += dwSize;
                pi = i + 1;
            }
        }

        if (pi < s1.length()) {
            DWORD dwSize = MultiByteToWideChar(CP_ACP, 0, source + pi, -1, NULL, 0);
            lengthVec.push_back(dwSize);
            len += dwSize;
        }

        if (len <= 0) {
            return ws;
        }

        wchar_t *buffer = new wchar_t[len];
        if (NULL == buffer) {
            return ws;
        }

        int xx = MultiByteToWideChar(CP_ACP, 0, source, 8, buffer, len);
        wstring ws1(buffer, len);

        DWORD c = 0;
        DWORD n = 0;
        while (c < len) {
            n = MultiByteToWideChar(CP_ACP, 0, source + c, -1, buffer + c, len - c);
            c+=n;
        }

        ws.assign(buffer, len);
        delete[] buffer;

        setlocale(LC_ALL, curLocale); // 恢复地域设置
        return ws;
    }

    vector<String> StringUtils::spilt(const String &s, TCHAR delimiter, bool bRemoveEmptyEntries)
    {
        String si;
        vector<String> sv;
#ifdef UNICODE
        wstringstream iss(s);
#else
        istringstream iss(s);
#endif
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

    vector<String> StringUtils::spilt(const String &s, const String &delimiters, bool bRemoveEmptyEntries, bool bDelimitersAsMulSeparator)
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

    bool StringUtils::isEmpty(const String &s)
    {
        return s.empty();
    }

    bool StringUtils::isBlank(const String &s)
    {
        if (s.empty()) {
            return true;
        }
        size_t length = s.length();
        for(size_t i = 0; i < length; ++i) {
            if (!Char::isWhitespace(s.at(i))) {
                return false;
            }
        }
        return true;
    }

    bool StringUtils::equals(const String &s1, const String &s2)
    {
        return s1 == s2;
        //return s1.compare(s2) == 0;
    }

    bool StringUtils::contains(const String &s, const String &subs)
    {
        return s.find(subs) != string::npos;
    }

    bool StringUtils::endsWith(const String &s, const String &subs)
    {
        return subs.empty() || (s.find(subs) != string::npos) && (s.find(subs) == s.length() - subs.length());
    }

    bool StringUtils::startsWith(const String &s, const String &subs)
    {
        return s.find(subs) == 0;
    }

    String StringUtils::trim(const String &s)
    {
        if (s.empty()) {
            return s; // 空字符串
        }

        std::size_t pos1 = 0;
        while (pos1 != s.length() && Char::isWhitespace(s.at(pos1))) {
            ++pos1;
        }

        std::size_t pos2, lastpos;
        pos2 = lastpos = s.length() - 1;
        while (pos2 < s.length() && Char::isWhitespace(s.at(pos2))) {
            --pos2;
        }

        if (0 == pos1 && lastpos == pos2) {
            return s; // 首尾不包含空白符
        }
        String _s = s;
        return _s.erase(pos2 + 1).erase(0, pos1);
    }

    String StringUtils::trimLeft(const String &s)
    {
        std::size_t pos = 0;
        while (pos != s.length() && Char::isWhitespace(s.at(pos))) {
            ++pos;
        }
        if (0 == pos) {
            return s; // 首部不包含空白符
        }
        String _s = s;
        return _s.erase(0, pos);
    }

    String StringUtils::trimRight(const String &s)
    {
        if (s.empty()) {
            return s; // 空字符串
        }
        std::size_t pos, lastpos;
        pos = lastpos = s.length() - 1;
        while (pos < s.length() && Char::isWhitespace(s.at(pos))) {
            --pos;
        }
        if (lastpos == pos) {
            return s; // 尾部不包含空白符
        }
        String _s = s;
        return _s.erase(pos + 1);
    }

    bool StringUtils::isAlpha(const String &s)
    {
        for (size_t i = 0; i < s.length(); i++) {
            if (!Char::isAlpha(s[i])) {
                return false;
            }
        }
        return !s.empty();
    }

    bool StringUtils::isLower(const String &s)
    {
        for (size_t i = 0; i < s.length(); i++) {
            if (!Char::isLower(s[i])) {
                return false;
            }
        }
        return !s.empty();
    }

    bool StringUtils::isUpper(const String &s)
    {
        for (size_t i = 0; i < s.length(); i++) {
            if (!Char::isUpper(s[i])) {
                return false;
            }
        }
        return !s.empty();
    }

    String StringUtils::toLower(const String &s)
    {
        String _s = s;
        for (size_t i = 0; i < s.length(); i++) {
            if (Char::isUpper(s[i])) {
                _s[i] = Char::toLower(s[i]);
            }
        }
        return _s;
    }

    String StringUtils::toUpper(const String &s)
    {
        String _s = s;
        for (size_t i = 0; i < s.length(); i++) {
            if (Char::isLower(s[i])) {
                _s[i] = Char::toUpper(s[i]);
            }
        }
        return _s;
    }

    bool StringUtils::isBoolean(const String &s)
    {
        String _s = toLower(s);
        return _T("true") == _s || _T("false") == _s;
    }

    bool StringUtils::isInteger(const String &s)
    {
        if (s.empty()) {
            return false;
        }
        // 判断符号
        size_t i = 0;
        if (Char::isSign(s[i])) {
            if (s.length() == ++i) {
                return false;
            }
        }
        while (i < s.length()) {
            if (!Char::isDigit(s[i++])) {
                return false;
            }
        }
        return true;
    }

    bool StringUtils::isNumeric(const String &s)
    {
        if (s.empty()) {
            return false;
        }

        int dotCount = 0;
        int digitCount = 0;

        // 判断符号
        size_t i = 0;
        if (Char::isSign(s[i])) {
            if (s.length() == ++i) {
                return false;
            }
        }
        for (; i < s.length(); ++i) {
            if (Char::isDot(s[i])) {
                if (0 == digitCount) {
                    return false; // 小数点前无数字
                }
                if (++dotCount > 1) {
                    return false; // 多个小数点
                }
                if (s.length() == i + 1) {
                    return false; // 小数点在最后一位
                }
                continue; // 小数点
            }
            if (!Char::isDigit(s[i])) {
                return false;
            }
            ++digitCount;
        }
        return true;
    }

    bool StringUtils::parseInteger(const String &s, int &value)
    {
        if (s.empty()) {
            return false;
        }

        int sign = 1;

        size_t i = 0;
        size_t startpos = 0;

        // 解析符号
        if (Char::isSign(s[i])) {
            if (s.length() == i) {
                return false;
            }
            if ('-' == s[i]) {
                sign = -1;
            }
            ++i;
            ++startpos;
        }
        while (i < s.length()) {
            if (!Char::isDigit(s[i++])) {
                return false;
            }
        }

        // 解析
        value = sign * toInt(s.substr(startpos));
        return true;
    }

    bool StringUtils::parseBoolean(const String &s, bool &value)
    {
        String _s = toLower(s);
        if (_T("true") == _s) {
            value = true;
            return true;
        }
        if (_T("false") == _s) {
            value = false;
            return true;
        }
        return false;
    }

    bool StringUtils::parseNumeric(const String &s, double &value)
    {
        if (s.empty()) {
            return false;
        }

        int sign = 1;
        int dotCount = 0;
        int digitCount = 0;

        size_t i = 0;
        size_t startpos = 0;

        // 解析符号
        if (Char::isSign(s[i])) {
            if (s.length() == i) {
                return false;
            }
            if ('-' == s[i]) {
                sign = -1;
            }
            ++i;
            ++startpos;
        }
        for (; i < s.length(); ++i) {
            if (Char::isDot(s[i])) {
                if (0 == digitCount) {
                    return false; // 小数点前无数字
                }
                if (++dotCount > 1) {
                    return false; // 多个小数点
                }
                if (s.length() == i + 1) {
                    return false; // 小数点在最后一位
                }
                continue; // 小数点
            }
            if (!Char::isDigit(s[i])) {
                return false;
            }
            ++digitCount;
        }

        // 解析
        value = sign * toDouble(s.substr(startpos));
        return true;
    }

    int StringUtils::parseInteger(const String &s, bool *ok)
    {
        int value = 0;
        bool ret = parseInteger(s, value);
        if (ok) {
            *ok = ret;
        }
        return value;
    }

    bool StringUtils::parseBoolean(const String &s, bool *ok)
    {
        bool value = 0;
        bool ret = parseBoolean(s, value);
        if (ok) {
            *ok = ret;
        }
        return value;
    }

    double StringUtils::parseNumeric(const String &s, bool *ok)
    {
        double value = 0;
        bool ret = parseNumeric(s, value);
        if (ok) {
            *ok = ret;
        }
        return value;
    }

    /**
     * #purpose  : 字符转十六进制
     * #note     : 不支持汉字字符
     * #param ch : 要转换成十六进制的字符
     * #return   : 转换后的字符串
     */
    String StringUtils::toHexString(unsigned char ch)
    {
        const String hex = _T("0123456789ABCDEF");

    #ifdef UNICODE
        wstringstream ss;
    #else
        stringstream ss;
    #endif
        ss << hex[ch >> 4] << hex[ch & 0xf];

        return ss.str();
    }

    /**
     * #purpose  : 字符转十六进制
     * #note     : 支持汉字字符
     * #param ch : 要转换成十六进制的字符
     * #return   : 转换后的字符串
     */
    String StringUtils::toHexWString(wchar_t ch)
    {
        const String hex = _T("0123456789ABCDEF");

    #ifdef UNICODE
        wstringstream ss;
    #else
        stringstream ss;
    #endif
        unsigned char hi = ch >> 8;
        unsigned char lo = ch & 0xff;
        ss << hex[hi >> 4] << hex[hi & 0xf]
           << hex[lo >> 4] << hex[lo & 0xf];

        return ss.str();
    }

    /**
     * #purpose         : 字符串转十六进制字符串
     * #note            : 支持汉字字符串
     * #param s         : 要转换成十六进制的字符串
     * #param separator : 十六进制字符串间的分隔符
     * #return          : 转换后的字符串
     */
    String StringUtils::toHexString(const String &s, const String &separator)
    {
        const String hex = _T("0123456789ABCDEF");

    #ifdef UNICODE
        wstringstream ss;
        #define _toHexString toHexWString
    #else
        stringstream ss;
        #define _toHexString toHexString
    #endif

        for (String::size_type i = 0; i < s.length(); ++i) {
            ss << _toHexString(s[i]);
            if (s.length() != i + 1) {
                ss << separator;
            }
        }

        return ss.str();
    }
}
