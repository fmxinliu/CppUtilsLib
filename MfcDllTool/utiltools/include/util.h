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
    DLL_API String toString(double f, int nCntAfterMark = -1); // С��λ��

    DLL_API int toInt(const String &s);
    DLL_API long toLong(const String &s);
    DLL_API float toFloat(const String &s);
    DLL_API double toDouble(const String &s);

    DLL_API String format(const TCHAR *pszFmt, ...); // ��ʽ���ַ���
    DLL_API std::wstring StringToWString(const std::string &s);  // ���ֽ� ת ���ֽ�
    DLL_API std::string WStringToString(const std::wstring &ws); // ���ֽ� ת ���ֽ�

    DLL_API std::vector<String> SpiltString(const String &s, TCHAR delimiter, bool bRemoveEmptyEntries = false);
    DLL_API std::vector<String> SpiltString(const String &s, const String &delimiters, bool bRemoveEmptyEntries = false, bool bDelimitersAsMulSeparator = false);

    DLL_API bool isEmpty(const String &s); // �Ƿ�Ϊ��
    DLL_API bool isBlank(const String &s); // �Ƿ�Ϊ�� or �ַ����е��ַ���Ϊ�հ׷�

    // �ж��Ƿ�Ϊ����
    DLL_API bool isInteger(const String &s);
    DLL_API bool isNumeric(const String &s);

    // �жϴ���ĸ�ַ���
    DLL_API bool isAlpha(const String &s);
    DLL_API bool isLower(const String &s);
    DLL_API bool isUpper(const String &s);

    DLL_API bool equals(const String &s1, const String &s2);
    DLL_API bool contains(const String &s, const String &subs);
    DLL_API bool endsWith(const String &s, const String &subs);
    DLL_API bool startsWith(const String &s, const String &subs);

    // ȥ���ס�β�հ׷�
    DLL_API String trim(const String &s);
    DLL_API String trimLeft(const String &s);
    DLL_API String trimRight(const String &s);

    // ��Сдת��
    DLL_API String toLower(const String &s);
    DLL_API String toUpper(const String &s);
};
