#pragma once
#include <vector>
#include "dllexport.h"

#if defined(WIN32)
namespace UtilTools
{
    enum AccessType { RegbitDefault, Regbit32, Regbit64 }; // ·ÃÎÊ32/64Î»×¢²á±í

    class DLL_API RegistryHelper
    {
    public:
        STATIC bool read(HKEY hKey, LPCTSTR lpSubKey, LPCTSTR lpValueName, int &value, AccessType type = RegbitDefault);
        STATIC bool read(HKEY hKey, LPCTSTR lpSubKey, LPCTSTR lpValueName, DWORD &value, AccessType type = RegbitDefault);
        STATIC bool read(HKEY hKey, LPCTSTR lpSubKey, LPCTSTR lpValueName, QWORD &value, AccessType type = RegbitDefault);
        STATIC bool read(HKEY hKey, LPCTSTR lpSubKey, LPCTSTR lpValueName, String &value, AccessType type = RegbitDefault);
        STATIC bool read(HKEY hKey, LPCTSTR lpSubKey, LPCTSTR lpValueName, std::vector<BYTE> &value, AccessType type = RegbitDefault);
        STATIC bool read(HKEY hKey, LPCTSTR lpSubKey, LPCTSTR lpValueName, std::vector<String> &value, AccessType type = RegbitDefault);
        STATIC bool write(HKEY hKey, LPCTSTR lpSubKey, LPCTSTR lpValueName, const int &value, AccessType type = RegbitDefault);
        STATIC bool write(HKEY hKey, LPCTSTR lpSubKey, LPCTSTR lpValueName, const DWORD &value, AccessType type = RegbitDefault);
        STATIC bool write(HKEY hKey, LPCTSTR lpSubKey, LPCTSTR lpValueName, const QWORD &value, AccessType type = RegbitDefault);
        STATIC bool write(HKEY hKey, LPCTSTR lpSubKey, LPCTSTR lpValueName, const String &value, AccessType type = RegbitDefault);
        STATIC bool write(HKEY hKey, LPCTSTR lpSubKey, LPCTSTR lpValueName, const std::vector<BYTE> &value, AccessType type = RegbitDefault);
        STATIC bool write(HKEY hKey, LPCTSTR lpSubKey, LPCTSTR lpValueName, const std::vector<String> &value, AccessType type = RegbitDefault);
        STATIC bool createKey(HKEY hKey, LPCTSTR lpSubKey, AccessType type = RegbitDefault);
        STATIC bool deleteKey(HKEY hKey, LPCTSTR lpSubKey, AccessType type = RegbitDefault);
        STATIC bool deleteValue(HKEY hKey, LPCTSTR lpSubKey, LPCTSTR lpValueName, AccessType type = RegbitDefault);
        STATIC bool saveKey(HKEY hKey, LPCTSTR lpSubKey, LPCTSTR lpFileName, AccessType type = RegbitDefault);
        STATIC bool restoreKey(HKEY hKey, LPCTSTR lpSubKey, LPCTSTR lpFileName, AccessType type = RegbitDefault);
    };
}
#endif
