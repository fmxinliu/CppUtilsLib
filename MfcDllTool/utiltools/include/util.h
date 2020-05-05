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
    DLL_API String toString(double f, int nCntAfterMark=-1); // С��λ��

    DLL_API int toInt(const String &s);
    DLL_API long toLong(const String &s);
    DLL_API float toFloat(const String &s);
    DLL_API double toDouble(const String &s);

    DLL_API String format(const TCHAR *pszFmt, ...);
    DLL_API std::wstring StringToWString(const std::string &s);  // ���ֽ� ת ���ֽ�
    DLL_API std::string WStringToString(const std::wstring &ws); // ���ֽ� ת ���ֽ�
};
