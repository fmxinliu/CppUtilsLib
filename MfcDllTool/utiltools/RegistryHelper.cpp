#include "StdAfx.h"
#include "RegistryHelper.h"
#include <winreg.h>
#include <assert.h>

using namespace std;
namespace UtilTools
{
    class RegistryHelperPrivate
    {
    public:
        RegistryHelperPrivate(HKEY hKey = HKEY_LOCAL_MACHINE);
        virtual ~RegistryHelperPrivate();
        enum AccessMode { OnlyRead, AllAccess }; // 操作权限

    public:
        void Close();
        BOOL Open(LPCTSTR lpSubKey, AccessMode mode, AccessType type = RegbitDefault);
        BOOL CreateKey(LPCTSTR lpSubKey, AccessType type = RegbitDefault);
        BOOL DeleteKey(HKEY hKey, LPCTSTR lpSubKey);
        BOOL DeleteValue(LPCTSTR lpValueName);
        BOOL RestoreKey(LPCTSTR lpFileName);
        BOOL SaveKey(LPCTSTR lpFileName);
        BOOL Read(LPCTSTR lpValueName, int *pValue);
        BOOL Read(LPCTSTR lpValueName, DWORD *pValue);
        BOOL Read(LPCTSTR lpValueName, QWORD *pValue);
        BOOL Read(LPCTSTR lpValueName, String *pValue);
        BOOL Read(LPCTSTR lpValueName, vector<BYTE> *pValue);
        BOOL Read(LPCTSTR lpValueName, vector<String> *pValue);
        BOOL Write(LPCTSTR lpValueName, const int &value);
        BOOL Write(LPCTSTR lpValueName, const DWORD &value);
        BOOL Write(LPCTSTR lpValueName, const QWORD &value);
        BOOL Write(LPCTSTR lpValueName, const String &value);
        BOOL Write(LPCTSTR lpValueName, const vector<BYTE> &value);
        BOOL Write(LPCTSTR lpValueName, const vector<String> &value);

    protected:
        BOOL ReadCheck(HKEY hKey, LPCTSTR lpValueName, DWORD &dwType, DWORD &dwSize);
        REGSAM GetAccessMask(AccessMode mode, AccessType type);

    protected:
        HKEY m_hKey;
    };

    RegistryHelperPrivate::RegistryHelperPrivate(HKEY hKey)
    {
        m_hKey = hKey;
    }

    RegistryHelperPrivate::~RegistryHelperPrivate()
    {
        Close();
    }

    REGSAM RegistryHelperPrivate::GetAccessMask(AccessMode mode, AccessType type)
    {
        REGSAM access;
        if (OnlyRead == mode) {
            access = KEY_READ;
        } else {
            access = KEY_ALL_ACCESS; // 需要管理员权限
        }
        // System32 存放 64 位的注册表
        // SysWOW64 存放 32 位的注册表
        // 32 位程序在64 位系统上，注册表自动跳转到 WoW6432Node 下面
        // 32位程序打开注册表，默认为“KEY_WOW64_32KEY”标志
        // 64位程序打开注册表，默认为“KEY_WOW64_64KEY”标志
        // 32位程序想访问64位的注册表，就是不带bWow6432Node的路径，则必须加上“KEY_WOW64_64KEY”。
        if (Regbit32 == type) {
            access |= KEY_WOW64_32KEY; // 访问 32 位注册表
        } else if (Regbit64 == type) {
            access |= KEY_WOW64_64KEY; // 访问 64 位注册表
        } else {
            // 根据应用程序的位数，确定访问多少位的注册表
        }
        return access;
    }

    void RegistryHelperPrivate::Close()
    {
        if(m_hKey) {
            ::RegCloseKey(m_hKey);
            m_hKey = NULL;
        }
    }

    BOOL RegistryHelperPrivate::Open(LPCTSTR lpSubKey, AccessMode mode, AccessType type)
    {
        assert(m_hKey);
        assert(lpSubKey);

        HKEY hKey;
        REGSAM accessMask = GetAccessMask(mode, type);
        long lReturn = ::RegOpenKeyEx(m_hKey, lpSubKey, 0L, accessMask, &hKey);
        if (ERROR_SUCCESS == lReturn) {
            m_hKey = hKey;
            return TRUE;
        }
        if (ERROR_FILE_NOT_FOUND == lReturn) {
            OutputDebugString(_T("ERROR_FILE_NOT_FOUND"));
        } else if (ERROR_ACCESS_DENIED == lReturn) {
            OutputDebugString(_T("ERROR_ACCESS_DENIED"));
        }
        return FALSE;
    }

    BOOL RegistryHelperPrivate::CreateKey(LPCTSTR lpSubKey, AccessType type)
    {
        assert(m_hKey);
        assert(lpSubKey);

        HKEY hKey;
        DWORD dw;
        REGSAM accessMask = GetAccessMask(AllAccess, type);
        long lReturn = ::RegCreateKeyEx(m_hKey, lpSubKey, 0L, NULL, REG_OPTION_VOLATILE, accessMask, NULL, &hKey, &dw);

        if(ERROR_SUCCESS == lReturn)
        {
            m_hKey = hKey;
            return TRUE;
        }
        return FALSE;
    }

    BOOL RegistryHelperPrivate::DeleteValue(LPCTSTR lpValueName)
    {
        assert(m_hKey);
        assert(lpValueName);

        long lReturn = ::RegDeleteValue(m_hKey, lpValueName);

        if(ERROR_SUCCESS == lReturn)
            return TRUE;
        return FALSE;
    }

    BOOL RegistryHelperPrivate::DeleteKey(HKEY hKey, LPCTSTR lpSubKey)
    {
        assert(hKey);
        assert(lpSubKey);

        long lReturn = ::RegDeleteValue(hKey, lpSubKey);

        if(ERROR_SUCCESS == lReturn)
            return TRUE;
        return FALSE;
    }

    BOOL RegistryHelperPrivate::RestoreKey(LPCTSTR lpFileName)
    {
        assert(m_hKey);
        assert(lpFileName);

        long lReturn = ::RegRestoreKey(m_hKey, lpFileName, REG_WHOLE_HIVE_VOLATILE);

        if(ERROR_SUCCESS == lReturn)
            return TRUE;
        return FALSE;
    }

    BOOL RegistryHelperPrivate::SaveKey(LPCTSTR lpFileName)
    {
        assert(m_hKey);
        assert(lpFileName);

        long lReturn = ::RegSaveKey(m_hKey, lpFileName, NULL);

        if(ERROR_SUCCESS == lReturn)
            return TRUE;
        return FALSE;
    }

    BOOL RegistryHelperPrivate::ReadCheck(HKEY hKey, LPCTSTR lpValueName, DWORD &dwType, DWORD &dwSize)
    {
        assert(hKey);
        assert(lpValueName);

        // 获取数据类型、数据长度
        long lReturn = ::RegQueryValueEx(hKey, lpValueName, NULL, &dwType, NULL, &dwSize);
        if (ERROR_SUCCESS != lReturn) {
            return FALSE;
        }

        return TRUE;
    }

#define READ_CHECK() \
    DWORD dwType; /*数据类型*/ \
    DWORD dwSize; /*数据长度*/ \
    do { \
        if (!ReadCheck(m_hKey, lpValueName, dwType, dwSize)) { \
            return FALSE; \
        } \
        if (!pValue) { \
            return TRUE; \
        } \
    } while(0)

    BOOL RegistryHelperPrivate::Read(LPCTSTR lpValueName, int *pValue)
    {
        READ_CHECK();
        DWORD dwValue; // 数据
        long lReturn = ::RegQueryValueEx(m_hKey, lpValueName, NULL, &dwType, (LPBYTE)&dwValue, &dwSize);
        if(ERROR_SUCCESS == lReturn) {
            *pValue = (int)dwValue;
            return TRUE;
        }
        return FALSE;
    }

    BOOL RegistryHelperPrivate::Read(LPCTSTR lpValueName, DWORD *pValue)
    {
        READ_CHECK();
        DWORD dwValue; // 数据
        long lReturn = ::RegQueryValueEx(m_hKey, lpValueName, NULL, &dwType, (LPBYTE)&dwValue, &dwSize);
        if(ERROR_SUCCESS == lReturn) {
            *pValue = dwValue;
            return TRUE;
        }
        return FALSE;
    }

    BOOL RegistryHelperPrivate::Read(LPCTSTR lpValueName, QWORD *pValue)
    {
        READ_CHECK();
        QWORD qwValue; // 数据
        long lReturn = ::RegQueryValueEx(m_hKey, lpValueName, NULL, &dwType, (LPBYTE)&qwValue, &dwSize);
        if(ERROR_SUCCESS == lReturn) {
            *pValue = qwValue;
            return TRUE;
        }
        return FALSE;
    }

    BOOL RegistryHelperPrivate::Read(LPCTSTR lpValueName, String *pValue)
    {
        READ_CHECK();
        LPBYTE pszString = new BYTE[dwSize];
        long lReturn = ::RegQueryValueEx(m_hKey, lpValueName, NULL, &dwType, pszString, &dwSize);
        if (ERROR_SUCCESS == lReturn) {
            *pValue = (LPCTSTR)pszString;
        }
        delete[] pszString;
        return ERROR_SUCCESS == lReturn;
    }

    BOOL RegistryHelperPrivate::Read(LPCTSTR lpValueName, vector<String> *pValue)
    {
        READ_CHECK();
        pValue->clear();
        LPBYTE pszString = new BYTE[dwSize];
        long lReturn = ::RegQueryValueEx(m_hKey, lpValueName, NULL, &dwType, pszString, &dwSize);
        if (ERROR_SUCCESS == lReturn) {
            size_t offset = 0;
            size_t bytes = dwSize - sizeof(TCHAR); // 去掉末尾多余的结束符
            LPCTSTR end = (LPCTSTR)(pszString + bytes); // 多余的结束符
            LPTSTR start = (LPTSTR)pszString;
            while (start < end) {
                pValue->push_back(start);
                offset = _tcslen(start) + 1;
                start += offset;
            }
        }
        delete[] pszString;
        return ERROR_SUCCESS == lReturn;
    }

    BOOL RegistryHelperPrivate::Read(LPCTSTR lpValueName, vector<BYTE> *pValue)
    {
        READ_CHECK();
        pValue->clear();
        LPBYTE buffer = new BYTE[dwSize];
        long lReturn = ::RegQueryValueEx(m_hKey, lpValueName, NULL, &dwType, buffer, &dwSize);
        if (ERROR_SUCCESS == lReturn) {
            for (size_t i = 0; i < dwSize; ++i) {
                pValue->push_back(buffer[i]);
            }
        }
        delete[] buffer;
        return ERROR_SUCCESS == lReturn;
    }

    BOOL RegistryHelperPrivate::Write(LPCTSTR lpValueName, const int &value)
    {
        return Write(lpValueName, (DWORD)value);
    }

    BOOL RegistryHelperPrivate::Write(LPCTSTR lpValueName, const DWORD &value)
    {
        assert(m_hKey);
        assert(lpValueName);
        const DWORD &dwValue = value;
        long lReturn = ::RegSetValueEx(m_hKey, lpValueName, 0L, REG_DWORD, (CONST BYTE *)&dwValue, sizeof(DWORD));
        return ERROR_SUCCESS == lReturn;
    }

    BOOL RegistryHelperPrivate::Write(LPCTSTR lpValueName, const QWORD &value)
    {
        assert(m_hKey);
        assert(lpValueName);
        const QWORD &qwValue = value;
        long lReturn = ::RegSetValueEx(m_hKey, lpValueName, 0L, REG_QWORD, (CONST BYTE *)&qwValue, sizeof(QWORD));
        return ERROR_SUCCESS == lReturn;
    }

    BOOL RegistryHelperPrivate::Write(LPCTSTR lpValueName, const String &value)
    {
        assert(m_hKey);
        assert(lpValueName);
        LPCTSTR lpVal = value.c_str();
        DWORD dwSize = (1 + _tcslen(lpVal))* sizeof(TCHAR);
        long lReturn = ::RegSetValueEx(m_hKey, lpValueName, 0L, REG_SZ, (CONST BYTE *)lpVal, dwSize);
        return ERROR_SUCCESS == lReturn;
    }

    BOOL RegistryHelperPrivate::Write(LPCTSTR lpValueName, const vector<String> &value)
    {
        assert(m_hKey);
        assert(lpValueName);

        // 统计字节数
        DWORD dwSize = 0;
        for (size_t i = 0; i < value.size(); ++i) {
            dwSize += (1 + _tcslen(value[i].c_str())) * sizeof(TCHAR);
        }
        dwSize += sizeof(TCHAR); // 末尾多填充一个结束字符

        // 转化为字节数组
        LPBYTE pszString = new BYTE[dwSize];
        LPTSTR p = (LPTSTR)pszString;
        for (size_t i = 0; i < value.size(); ++i) {
            LPCTSTR s = value[i].c_str();
            size_t len = _tcslen(s) + 1;
            _tcscpy_s(p, len, s);
            p += len;
        }
        *p  = '\0'; // 结束字符，代表整个多字符串结束

        // 写注册表
        long lReturn = ::RegSetValueEx(m_hKey, lpValueName, 0L, REG_MULTI_SZ, (CONST BYTE *)pszString, dwSize);
        delete[] pszString;
        return ERROR_SUCCESS == lReturn;
    }

    BOOL RegistryHelperPrivate::Write(LPCTSTR lpValueName, const vector<BYTE> &value)
    {
        assert(m_hKey);
        assert(lpValueName);
        size_t dwSize = value.size();
        LPBYTE buffer = new BYTE[dwSize];
        for (size_t i = 0; i < dwSize; ++i) {
            buffer[i] = value[i];
        }
        long lReturn = ::RegSetValueEx(m_hKey, lpValueName, 0L, REG_BINARY, (CONST BYTE *)buffer, dwSize);
        delete[] buffer;
        return ERROR_SUCCESS == lReturn;
    }
}

namespace UtilTools
{
    template<class T>
    static bool Read(HKEY m_hKey, LPCTSTR lpSubKey, LPCTSTR lpValueName, T &value, AccessType type)
    {
        RegistryHelperPrivate d(m_hKey);
        if (!d.Open(lpSubKey, RegistryHelperPrivate::OnlyRead, type)) {
            return false;
        }
        return !!d.Read(lpValueName, &value);
    }

    template<class T>
    static bool Write(HKEY m_hKey, LPCTSTR lpSubKey, LPCTSTR lpValueName, const T &value, AccessType type)
    {
        RegistryHelperPrivate d(m_hKey);
        if (!d.Open(lpSubKey, RegistryHelperPrivate::AllAccess, type)) {
            return false;
        }
        return !!d.Write(lpValueName, value);
    }

#define READ(TYPE) \
    bool RegistryHelper::read(HKEY m_hKey, LPCTSTR lpSubKey, LPCTSTR lpValueName, TYPE &value, AccessType type) { \
        return Read(m_hKey, lpSubKey, lpValueName, value, type); \
    }

#define WRITE(TYPE) \
    bool RegistryHelper::write(HKEY m_hKey, LPCTSTR lpSubKey, LPCTSTR lpValueName, const TYPE &value, AccessType type) { \
        return Write(m_hKey, lpSubKey, lpValueName, value, type); \
    } \

    READ(int);
    READ(DWORD);
    READ(QWORD);
    READ(String);
    READ(vector<BYTE>);
    READ(vector<String>);

    WRITE(int);
    WRITE(DWORD);
    WRITE(QWORD);
    WRITE(String);
    WRITE(vector<BYTE>);
    WRITE(vector<String>);
}
