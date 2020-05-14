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

    public:
        void Close();
        BOOL Open(LPCTSTR lpSubKey, bool bWow6432Node, bool bWrite = false);
        BOOL CreateKey(LPCTSTR lpSubKey);
        BOOL DeleteKey(HKEY hKey, LPCTSTR lpSubKey);
        BOOL DeleteValue(LPCTSTR lpValueName);
        BOOL RestoreKey(LPCTSTR lpFileName);
        BOOL SaveKey(LPCTSTR lpFileName);
        BOOL Read(LPCTSTR lpValueName, int *pValue);
        BOOL Read(LPCTSTR lpValueName, DWORD *pValue);
        BOOL Read(LPCTSTR lpValueName, QWORD *pValue);
        BOOL Read(LPCTSTR lpValueName, String *pValue);
        BOOL Read(LPCTSTR lpValueName, vector<String> *pValue);
        BOOL Write(LPCTSTR lpValueName, int value);
        BOOL Write(LPCTSTR lpValueName, DWORD value);
        BOOL Write(LPCTSTR lpValueName, QWORD value);
        BOOL Write(LPCTSTR lpValueName, String value);
        BOOL Write(LPCTSTR lpValueName, vector<String> value);

    protected:
        template<class T> BOOL Read(LPCTSTR lpValueName, T *pValue);
        BOOL Check(HKEY hKey, LPCTSTR lpValueName, void *pValue, DWORD &dwType, DWORD &dwSize);

    protected:
        HKEY m_hKey;
    };

    template<class T> BOOL RegistryHelperPrivate::Read(LPCTSTR lpValueName, T *pValue)
    {
        assert(m_hKey);
        assert(lpValueName);
        assert(pValue);

        DWORD dwType; // 数据类型
        DWORD dwSize; // 数据长度

        // 获取数据类型、数据长度
        long lReturn = ::RegQueryValueEx(m_hKey, lpValueName, NULL, &dwType, NULL, &dwSize);
        if (ERROR_SUCCESS != lReturn) {
            return FALSE;
        }

        switch (dwType) {
        case REG_SZ: { // 字符串值
            char *pszString = new char[dwSize];
            lReturn = ::RegQueryValueEx(m_hKey, lpValueName, NULL, &dwType, (LPBYTE)pszString, &dwSize);
            if (ERROR_SUCCESS == lReturn) {
                *pValue = pszString;
            }
            delete[] pszString;
                     } break;
        case REG_MULTI_SZ: { // 多字符串值
            unsigned char *pszString = new unsigned char[dwSize];
            lReturn = ::RegQueryValueEx(m_hKey, lpValueName, NULL, &dwType, pszString, &dwSize);
            if (ERROR_SUCCESS == lReturn) {
                size_t i = 0;
                const char *p = (const char *)pszString;
                while (i < dwSize - 1) { // 去掉最末尾的1个字符
                    pValue->push_back(p + i);
                    i += strlen(p + i) + 1;
                }
            }
            delete[] pszString;
                           } break;
        case REG_DWORD: { // （32-位）值
            DWORD dwValue;
            lReturn = ::RegQueryValueEx(m_hKey, lpValueName, NULL, &dwType, (BYTE *)&dwValue, &dwSize);
            *pValue = dwValue;
                        } break;
        case REG_QWORD: { // （64-位）值
            QWORD qwValue;
            lReturn = ::RegQueryValueEx(m_hKey, lpValueName, NULL, &dwType, (BYTE *)&qwValue, &dwSize);
            *pValue = qwValue;
                        } break;
        default:
            break;
        }

        return lReturn;
    }

    RegistryHelperPrivate::RegistryHelperPrivate(HKEY hKey)
    {
        m_hKey = hKey;
    }

    RegistryHelperPrivate::~RegistryHelperPrivate()
    {
        Close();
    }

    void RegistryHelperPrivate::Close()
    {
        if(m_hKey) {
            ::RegCloseKey(m_hKey);
            m_hKey = NULL;
        }
    }

    BOOL RegistryHelperPrivate::Open(LPCTSTR lpSubKey, bool bWow6432Node, bool bWrite)
    {
        assert(m_hKey);
        assert(lpSubKey);

        HKEY hKey;
        REGSAM accessMask;
        if (bWrite) {
            accessMask = KEY_ALL_ACCESS; // 需要管理员权限
        } else {
            accessMask = KEY_READ;
        }
        // System32 存放 64 位的注册表
        // SysWOW64 存放 32 位的注册表
        // 32 位程序在64 位系统上，注册表自动跳转到 WoW6432Node 下面
        // 32位程序打开注册表，默认为“KEY_WOW64_32KEY”标志
        // 64位程序打开注册表，默认为“KEY_WOW64_64KEY”标志
        // 32位程序想访问64位的注册表，就是不带bWow6432Node的路径，则必须加上“KEY_WOW64_64KEY”。
        if (bWow6432Node) {
            accessMask |= KEY_WOW64_32KEY; // 访问 32 位注册表
        } else {
            accessMask |= KEY_WOW64_64KEY; // 访问 64 位注册表
        }

        long lReturn = ::RegOpenKeyEx(m_hKey, lpSubKey, 0L, accessMask, &hKey);
        if (ERROR_SUCCESS == lReturn) {
            m_hKey = hKey;
            return TRUE;
        }
        if (ERROR_FILE_NOT_FOUND == lReturn) {
            OutputDebugString("ERROR_FILE_NOT_FOUND");
        } else if (ERROR_ACCESS_DENIED == lReturn) {
            OutputDebugString("ERROR_ACCESS_DENIED");
        }
        return FALSE;
    }

    BOOL RegistryHelperPrivate::CreateKey(LPCTSTR lpSubKey)
    {
        assert(m_hKey);
        assert(lpSubKey);

        HKEY hKey;
        DWORD dw;
        long lReturn = ::RegCreateKeyEx(m_hKey, lpSubKey, 0L, NULL, REG_OPTION_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, &dw);

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

    BOOL RegistryHelperPrivate::Check(HKEY hKey, LPCTSTR lpValueName, void *pValue, DWORD &dwType, DWORD &dwSize)
    {
        assert(hKey);
        assert(lpValueName);
        assert(pValue);

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
    if (!Check(m_hKey, lpValueName, pValue, dwType, dwSize)) { \
    return FALSE; \
    } \
    } while(0)

    BOOL RegistryHelperPrivate::Read(LPCTSTR lpValueName, int *pValue)
    {
        READ_CHECK();
        DWORD dwValue; // 数据
        long lReturn = ::RegQueryValueEx(m_hKey, lpValueName, NULL, &dwType, (BYTE *)&dwValue, &dwSize);
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
        long lReturn = ::RegQueryValueEx(m_hKey, lpValueName, NULL, &dwType, (BYTE *)&dwValue, &dwSize);
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
        long lReturn = ::RegQueryValueEx(m_hKey, lpValueName, NULL, &dwType, (BYTE *)&qwValue, &dwSize);
        if(ERROR_SUCCESS == lReturn) {
            *pValue = qwValue;
            return TRUE;
        }
        return FALSE;
    }

    BOOL RegistryHelperPrivate::Read(LPCTSTR lpValueName, String *pValue)
    {
        READ_CHECK();
        char *pszString = new char[dwSize];
        long lReturn = ::RegQueryValueEx(m_hKey, lpValueName, NULL, &dwType, (LPBYTE)pszString, &dwSize);
        if (ERROR_SUCCESS == lReturn) {
            *pValue = pszString;
        }
        delete[] pszString;
        return ERROR_SUCCESS == lReturn;
    }

    // 读多字符串
    BOOL RegistryHelperPrivate::Read(LPCTSTR lpValueName, vector<String> *pValue)
    {
        READ_CHECK();
        unsigned char *pszString = new unsigned char[dwSize];
        long lReturn = ::RegQueryValueEx(m_hKey, lpValueName, NULL, &dwType, pszString, &dwSize);
        if (ERROR_SUCCESS == lReturn) {
            size_t i = 0;
            const char *p = (const char *)pszString;
            while (i < dwSize - 1) { // 去掉最末尾的1个字符
                pValue->push_back(p + i);
                i += strlen(p + i) + 1;
            }
        }
        delete[] pszString;
        return ERROR_SUCCESS == lReturn;
    }

    BOOL RegistryHelperPrivate::Write(LPCTSTR lpValueName, int value)
    {
        return Write(lpValueName, (DWORD)value);
    }

    BOOL RegistryHelperPrivate::Write(LPCTSTR lpValueName, DWORD value)
    {
        assert(m_hKey);
        assert(lpValueName);
        DWORD &dwValue = value;
        long lReturn = ::RegSetValueEx(m_hKey, lpValueName, 0L, REG_DWORD, (CONST BYTE *)&dwValue, sizeof(DWORD));
        return ERROR_SUCCESS == lReturn;
    }

    BOOL RegistryHelperPrivate::Write(LPCTSTR lpValueName, QWORD value)
    {
        assert(m_hKey);
        assert(lpValueName);
        QWORD &qwValue = value;
        long lReturn = ::RegSetValueEx(m_hKey, lpValueName, 0L, REG_QWORD, (CONST BYTE *)&qwValue, sizeof(QWORD));
        return ERROR_SUCCESS == lReturn;
    }

    BOOL RegistryHelperPrivate::Write(LPCTSTR lpValueName, String value)
    {
        assert(m_hKey);
        assert(lpValueName);
        LPCTSTR lpVal = value.c_str();
        long lReturn = ::RegSetValueEx(m_hKey, lpValueName, 0L, REG_SZ, (CONST BYTE *)lpVal, strlen(lpVal) + 1);
        return ERROR_SUCCESS == lReturn;
    }

    // 写多字符串
    BOOL RegistryHelperPrivate::Write(LPCTSTR lpValueName, vector<String> value)
    {
        assert(m_hKey);
        assert(lpValueName);

        // 计算字符长度
        size_t length = 0;
        for (size_t i= 0; i < value.size(); ++i) {
            length += strlen(value[i].c_str()) + 1;
        }
        length++; // 最末尾填充1个字符

        // 转化为字符数字
        unsigned char *pszString = new unsigned char[length];
        memset(pszString, 0, length);
        char *p = (char *)pszString;
        for (size_t i = 0; i < value.size(); ++i) {
            const char *s = value[i].c_str();
            size_t len = strlen(s) + 1;
            strcpy(p, s);
            p += len;
        }

        // 写注册表
        long lReturn = ::RegSetValueEx(m_hKey, lpValueName, 0L, REG_MULTI_SZ, (CONST BYTE *)pszString, length);
        delete []pszString;
        return ERROR_SUCCESS == lReturn;
    }
}

namespace UtilTools
{
    template<class T>
    static bool Read(HKEY m_hKey, LPCTSTR lpSubKey, LPCTSTR lpValueName, T &value, bool bWow6432Node)
    {
        RegistryHelperPrivate d(m_hKey);
        if (!d.Open(lpSubKey, bWow6432Node, false)) {
            return false;
        }
        return !!d.Read(lpValueName, &value);
    }

    template<class T>
    static bool Write(HKEY m_hKey, LPCTSTR lpSubKey, LPCTSTR lpValueName, T &value, bool bWow6432Node)
    {
        RegistryHelperPrivate d(m_hKey);
        if (!d.Open(lpSubKey, bWow6432Node, true)) {
            return false;
        }
        return !!d.Write(lpValueName, value);
    }

#define READ(TYPE) \
    bool RegistryHelper::read(HKEY m_hKey, LPCTSTR lpSubKey, LPCTSTR lpValueName, TYPE &value, bool bWow6432Node) { \
    return Read(m_hKey, lpSubKey, lpValueName, value, bWow6432Node); \
    }

#define WRITE(TYPE) \
    bool RegistryHelper::write(HKEY m_hKey, LPCTSTR lpSubKey, LPCTSTR lpValueName, TYPE value, bool bWow6432Node) { \
    return Write(m_hKey, lpSubKey, lpValueName, value, bWow6432Node); \
    } \

    READ(int);
    READ(DWORD);
    READ(QWORD);
    READ(String);
    READ(vector<String>);

    WRITE(int);
    WRITE(DWORD);
    WRITE(QWORD);
    WRITE(String);
    WRITE(vector<String>);
}
