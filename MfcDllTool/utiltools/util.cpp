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
        return format(_T("%g"), f); // ȥ������� 0
    } else {
        return format(_T("%f"), f); // ����6λС�������油0
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
        nLength += 1;  //���淵�صĳ����ǲ�����\0���������
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

    // �����ַ���Ϊ�� or �Էָ�����β�����
    if (s.empty() || s.back() == delimiter) {
        sv.push_back(_T(""));
    }

    return sv;
}

// delimiters - ��������Ϊһ���ָ�����
static vector<String> SplitUseOneSeparator(const String &s, const String &delimiters)
{
    String si;
    vector<String> sv;

    // ����մ����շָ�������
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
        sv.push_back(s.substr(startpos)); // ��ȡʣ���ַ���
    } else {
        sv.push_back(_T("")); // �����ַ����Էָ�����β�����
    }

    return sv;
}

// delimiters - ��ÿ���ַ���Ϊһ���ָ�����
static vector<String> SplitUseMulSeparator(const String &s, const String &delimiters)
{
    String si;
    vector<String> sv;

    typedef string::size_type string_size;
    string_size startpos = 0;
    string_size endpos = 0;

    bool found = true;
    while(found && s.size() != startpos) {
        // ���ҷָ���
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

        // �ָ��ַ���
        if (found) {
            sv.push_back(s.substr(startpos, endpos - startpos));
            startpos = ++endpos;
        } else {
            sv.push_back(s.substr(startpos)); // ��ȡʣ���ַ���
        }
    }

    // �����ַ����Էָ�����β�����
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

    // ȥ�����ַ���
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

// �ж��Ƿ�Ϊ�հ׷�
static bool isWhitespace(TCHAR ch)
{
    return ch == ' ' || ch == '\t' || ch == '\n';
}

bool isBlank(const String &s)
{
    if (s.empty()) {
        return true;
    }
    size_t length = s.length();
    for(size_t i = 0; i < length; ++i) {
        // �ж��ַ��Ƿ�Ϊ�ո��Ʊ�������з�
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
    if (s.empty()) {
        return s; // ���ַ���
    }

    std::size_t pos1 = 0;
    while (pos1 != s.length() && isWhitespace(s.at(pos1))) {
        ++pos1;
    }

    std::size_t pos2, lastpos;
    pos2 = lastpos = s.length() - 1;
    while (pos2 < s.length() && isWhitespace(s.at(pos2))) {
        --pos2;
    }

    if (0 == pos1 && lastpos == pos2) {
        return s; // ��β�������հ׷�
    }
    String _s = s;
    return _s.erase(pos2 + 1).erase(0, pos1);
}

String trimLeft(const String &s)
{
    std::size_t pos = 0;
    while (pos != s.length() && isWhitespace(s.at(pos))) {
        ++pos;
    }
    if (0 == pos) {
        return s; // �ײ��������հ׷�
    }
    String _s = s;
    return _s.erase(0, pos);
}

String trimRight(const String &s)
{
    if (s.empty()) {
        return s; // ���ַ���
    }
    std::size_t pos, lastpos;
    pos = lastpos = s.length() - 1;
    while (pos < s.length() && isWhitespace(s.at(pos))) {
        --pos;
    }
    if (lastpos == pos) {
        return s; // β���������հ׷�
    }
    String _s = s;
    return _s.erase(pos + 1);
}

String toLower(const String &s)
{
    String _s = s;
    for (size_t i = 0; i < s.length(); i++) {
        if (s[i] >= 'A' && s[i] <= 'Z') {
            _s[i] += 32;
        }
    }
    return _s;
}

String toUpper(const String &s)
{
    String _s = s;
    for (size_t i = 0; i < s.length(); i++) {
        if (s[i] >= 'a' && s[i] <= 'z') {
            _s[i] -= 32;
        }
    }
    return _s;
}
}
