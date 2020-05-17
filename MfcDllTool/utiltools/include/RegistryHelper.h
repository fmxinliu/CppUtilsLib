#pragma once
#include <vector>
#include "dllexport.h"

namespace UtilTools
{
    enum AccessType { RegbitDefault, Regbit32, Regbit64 }; // ·ÃÎÊ32/64Î»×¢²á±í

    class DLL_API RegistryHelper
    {
    public:
        static bool read(HKEY m_hKey, LPCTSTR lpSubKey, LPCTSTR lpValueName, int &value, AccessType type = RegbitDefault);
        static bool read(HKEY m_hKey, LPCTSTR lpSubKey, LPCTSTR lpValueName, DWORD &value, AccessType type = RegbitDefault);
        static bool read(HKEY m_hKey, LPCTSTR lpSubKey, LPCTSTR lpValueName, QWORD &value, AccessType type = RegbitDefault);
        static bool read(HKEY m_hKey, LPCTSTR lpSubKey, LPCTSTR lpValueName, String &value, AccessType type = RegbitDefault);
        static bool read(HKEY m_hKey, LPCTSTR lpSubKey, LPCTSTR lpValueName, std::vector<BYTE> &value, AccessType type = RegbitDefault);
        static bool read(HKEY m_hKey, LPCTSTR lpSubKey, LPCTSTR lpValueName, std::vector<String> &value, AccessType type = RegbitDefault);
        static bool write(HKEY m_hKey, LPCTSTR lpSubKey, LPCTSTR lpValueName, const int &value, AccessType type = RegbitDefault);
        static bool write(HKEY m_hKey, LPCTSTR lpSubKey, LPCTSTR lpValueName, const DWORD &value, AccessType type = RegbitDefault);
        static bool write(HKEY m_hKey, LPCTSTR lpSubKey, LPCTSTR lpValueName, const QWORD &value, AccessType type = RegbitDefault);
        static bool write(HKEY m_hKey, LPCTSTR lpSubKey, LPCTSTR lpValueName, const String &value, AccessType type = RegbitDefault);
        static bool write(HKEY m_hKey, LPCTSTR lpSubKey, LPCTSTR lpValueName, const std::vector<BYTE> &value, AccessType type = RegbitDefault);
        static bool write(HKEY m_hKey, LPCTSTR lpSubKey, LPCTSTR lpValueName, const std::vector<String> &value, AccessType type = RegbitDefault);
        static bool createKey(HKEY m_hKey, LPCTSTR lpSubKey, AccessType type = RegbitDefault);
        static bool deleteKey(HKEY m_hKey, LPCTSTR lpSubKey, AccessType type = RegbitDefault);
        static bool deleteValue(HKEY m_hKey, LPCTSTR lpSubKey, LPCTSTR lpValueName, AccessType type = RegbitDefault);
        static bool saveKey(HKEY hKey, LPCTSTR lpSubKey, LPCTSTR lpFileName, AccessType type = RegbitDefault);
        static bool restoreKey(HKEY hKey, LPCTSTR lpSubKey, LPCTSTR lpFileName, AccessType type = RegbitDefault);
    };
}
