#pragma once
#include <vector>
#include "dllexport.h"

namespace UtilTools
{
    class DLL_API StringUtils
    {
    public:
        // ת�ַ���
        STATIC String toString(int n);
        STATIC String toString(long n);
        STATIC String toString(unsigned int n);
        STATIC String toString(unsigned long n);
        STATIC String toString(double f, bool bTrimZero);
        STATIC String toString(double f, int nCntAfterMark = -1); // С��λ��

        // �ַ���ת����
        STATIC int toInt(const String &s);
        STATIC long toLong(const String &s);
        STATIC float toFloat(const String &s);
        STATIC double toDouble(const String &s);

        // �ַ���ת�� && ֧�ֺ���
        STATIC String format(const TCHAR *pszFmt, ...);
        STATIC std::wstring stringToWString(const std::string &s);
        STATIC std::string wstringToString(const std::wstring &ws);

        // �ָ�
        STATIC std::vector<String> spilt(const String &s, TCHAR delimiter, bool bRemoveEmptyEntries = false);
        STATIC std::vector<String> spilt(const String &s, const String &delimiters, bool bRemoveEmptyEntries = false, bool bDelimitersAsMulSeparator = false);

        // ��(��)���ж�
        STATIC bool isEmpty(const String &s); // �Ƿ�Ϊ��
        STATIC bool isBlank(const String &s); // �Ƿ�Ϊ�� or �ַ����е��ַ���Ϊ�հ׷�

        // �����ж�
        STATIC bool equals(const String &s1, const String &s2);
        STATIC bool contains(const String &s, const String &subs);
        STATIC bool endsWith(const String &s, const String &subs);
        STATIC bool startsWith(const String &s, const String &subs);

        // ȥ���ס�β�հ׷�
        STATIC String trim(const String &s);
        STATIC String trimLeft(const String &s);
        STATIC String trimRight(const String &s);

        // �ж��Ƿ�Ϊ��ĸ
        STATIC bool isAlpha(const String &s);
        STATIC bool isLower(const String &s);
        STATIC bool isUpper(const String &s);

        // ��Сдת��
        STATIC String toLower(const String &s);
        STATIC String toUpper(const String &s);

        // �ж��Ƿ�Ϊ����
        STATIC bool isBoolean(const String &s);
        STATIC bool isInteger(const String &s);
        STATIC bool isNumeric(const String &s);

        // ��������
        STATIC bool parseInteger(const String &s, int &value);
        STATIC bool parseBoolean(const String &s, bool &value);
        STATIC bool parseNumeric(const String &s, double &value);

        STATIC int parseInteger(const String &s, bool *ok = NULL);
        STATIC bool parseBoolean(const String &s, bool *ok = NULL);
        STATIC double parseNumeric(const String &s, bool *ok = NULL);

        // תʮ������
        STATIC String toHexWString(wchar_t ch); // ֧�ֺ����ַ�
        STATIC String toHexString(unsigned char ch); // ��֧�ֺ����ַ�
        STATIC String toHexString(const String &s, const String &separator = _T(""));
    };
}
