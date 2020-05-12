#include "StdAfx.h"
#include "RegistryHelper.h"
#include <winreg.h>
#include <assert.h>

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
        BOOL Read(LPCTSTR lpValueName, int *pnVal);
        BOOL Read(LPCTSTR lpValueName, DWORD *pdwVal);
        BOOL Read(LPCTSTR lpValueName, String *lpVal);
        BOOL Write(LPCTSTR lpSubKey, int nVal);
        BOOL Write(LPCTSTR lpSubKey, DWORD dwVal);
        BOOL Write(LPCTSTR lpSubKey, LPCTSTR lpVal);

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

    void RegistryHelperPrivate::Close()
    {
        if(m_hKey)
        {
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

    BOOL RegistryHelperPrivate::Read(LPCTSTR lpValueName, int *pnVal)
    {
        assert(m_hKey);
        assert(lpValueName);
        assert(pnVal);

        DWORD dwType;
        DWORD dwSize = sizeof(DWORD);
        DWORD dwDest;
        long lReturn = ::RegQueryValueEx(m_hKey, lpValueName, NULL, &dwType, (BYTE *)&dwDest, &dwSize);
        if(ERROR_SUCCESS == lReturn)
        {
            *pnVal = (int)dwDest;
            return TRUE;
        }
        return FALSE;
    }

    BOOL RegistryHelperPrivate::Read(LPCTSTR lpValueName, DWORD *pdwVal)
    {
        assert(m_hKey);
        assert(lpValueName);
        assert(pdwVal);

        DWORD dwType;
        DWORD dwSize = sizeof(DWORD);
        DWORD dwDest;
        long lReturn = ::RegQueryValueEx(m_hKey, lpValueName, NULL, &dwType, (BYTE *)&dwDest, &dwSize);

        if(ERROR_SUCCESS == lReturn)
        {
            *pdwVal = dwDest;
            return TRUE;
        }
        return FALSE;
    }

    BOOL RegistryHelperPrivate::Read(LPCTSTR lpValueName, String *lpVal)
    {
        assert(m_hKey);
        assert(lpValueName);
        assert(lpVal);

        DWORD dwType;
        DWORD dwSize = 200;
        char szString[2550];

        long lReturn = ::RegQueryValueEx(m_hKey, lpValueName, NULL, &dwType, (BYTE *)szString, &dwSize);

        if(ERROR_SUCCESS == lReturn)
        {

            *lpVal = szString;
            return TRUE;
        }
        return FALSE;
    }

    BOOL RegistryHelperPrivate::Write(LPCTSTR lpSubKey, int nVal)
    {
        assert(m_hKey);
        assert(lpSubKey);

        DWORD dwValue = (DWORD)nVal;
        long lReturn = ::RegSetValueEx(m_hKey, lpSubKey, 0L, REG_DWORD, (const BYTE *)&dwValue, sizeof(DWORD));

        if(ERROR_SUCCESS == lReturn)
            return TRUE;
        return FALSE;
    }

    BOOL RegistryHelperPrivate::Write(LPCTSTR lpSubKey, DWORD dwVal)
    {
        assert(m_hKey);
        assert(lpSubKey);

        long lReturn = ::RegSetValueEx(m_hKey, lpSubKey, 0L, REG_DWORD, (const BYTE *)&dwVal, sizeof(DWORD));

        if(ERROR_SUCCESS == lReturn)
            return TRUE;
        return FALSE;
    }

    BOOL RegistryHelperPrivate::Write(LPCTSTR lpValueName, LPCTSTR lpValue)
    {
        assert(m_hKey);
        assert(lpValueName);
        assert(lpValue);

        long lReturn = ::RegSetValueEx(m_hKey, lpValueName, 0L, REG_SZ, (const BYTE *)lpValue, strlen(lpValue) + 1);

        if(lReturn==ERROR_SUCCESS)
            return TRUE;
        return FALSE;
    }
}

#define READ(m_hKey, lpSubKey, value, bWow64_32KEY) \
    do { \
    RegistryHelperPrivate d(m_hKey);\
    if (!d.Open(lpSubKey, bWow64_32KEY, false)) { \
    return false; \
    } \
    return !!d.Read(lpValueName, &value); \
    } while(0)
#define WRITE(m_hKey, lpSubKey, value, bWow64_32KEY) \
    do { \
    RegistryHelperPrivate d(m_hKey);\
    if (!d.Open(lpSubKey, bWow64_32KEY, true)) { \
    return false; \
    } \
    return !!d.Write(lpValueName, value); \
    } while(0)

namespace UtilTools
{
    bool RegistryHelper::read(HKEY m_hKey, LPCTSTR lpSubKey, LPCTSTR lpValueName, int &value, bool bWow64_32KEY)
    {
        READ(m_hKey, lpSubKey, value, bWow64_32KEY);
    }

    bool RegistryHelper::read(HKEY m_hKey, LPCTSTR lpSubKey, LPCTSTR lpValueName, DWORD &value, bool bWow64_32KEY)
    {
        READ(m_hKey, lpSubKey, value, bWow64_32KEY);
    }

    bool RegistryHelper::read(HKEY m_hKey, LPCTSTR lpSubKey, LPCTSTR lpValueName, String &value, bool bWow64_32KEY)
    {
        READ(m_hKey, lpSubKey, value, bWow64_32KEY);
    }

    bool RegistryHelper::write(HKEY m_hKey, LPCTSTR lpSubKey, LPCTSTR lpValueName, int &value, bool bWow64_32KEY)
    {
        WRITE(m_hKey, lpSubKey, value, bWow64_32KEY);
    }

    bool RegistryHelper::write(HKEY m_hKey, LPCTSTR lpSubKey, LPCTSTR lpValueName, DWORD &value, bool bWow64_32KEY)
    {
        WRITE(m_hKey, lpSubKey, value, bWow64_32KEY);
    }

    bool RegistryHelper::write(HKEY m_hKey, LPCTSTR lpSubKey, LPCTSTR lpValueName, String &value, bool bWow64_32KEY)
    {
        WRITE(m_hKey, lpSubKey, value.c_str(), bWow64_32KEY);
    }
}

namespace UtilTools
{
    String RegistryHelper::GetRegValue(int nKeyType, const String& strUrl, const String& strKey, bool bWow64_32KEY)
    {
        String strValue(_T(""));
        HKEY hKey = NULL;
        HKEY hKeyResult = NULL;
        DWORD dwSize   = 0;
        DWORD dwDataType = 0;

        switch(nKeyType)
        {
        case 0:
            {
                hKey = HKEY_CLASSES_ROOT;
                break;
            }
        case 1:
            {
                hKey = HKEY_CURRENT_USER;
                break;
            }
        case 2:
            {
                hKey = HKEY_LOCAL_MACHINE;
                break;
            }
        case 3:
            {
                hKey = HKEY_USERS;
                break;
            }
        case 4:
            {
                hKey = HKEY_PERFORMANCE_DATA;
                break;
            }
        case 5:
            {
                hKey = HKEY_CURRENT_CONFIG;
                break;
            }
        case 6:
            {
                hKey = HKEY_DYN_DATA;
                break;
            }
        case 7:
            {
                hKey = HKEY_CURRENT_USER_LOCAL_SETTINGS;
                break;
            }
        case 8:
            {
                hKey = HKEY_PERFORMANCE_TEXT;
                break;
            }
        case 9:
            {
                hKey = HKEY_PERFORMANCE_NLSTEXT;
                break;
            }
        default:
            {
                return strValue;
            }
        }

        //打开注册表，SysWOW64 存放 32 位的注册表
        REGSAM accessMask = bWow64_32KEY ? KEY_READ | KEY_WOW64_64KEY : KEY_READ;
        if(ERROR_SUCCESS == ::RegOpenKeyEx(hKey, strUrl.c_str(), 0, accessMask, &hKeyResult))
        {
            // 获取缓存的长度dwSize及类型dwDataType
            ::RegQueryValueEx(hKeyResult, strKey.c_str(), 0, &dwDataType, NULL, &dwSize); 
            switch (dwDataType)
            {
            case REG_MULTI_SZ:
                {
                    //分配内存大小
                    BYTE* lpValue = new BYTE[dwSize];
                    //获取注册表中指定的键所对应的值
                    LONG lRet = ::RegQueryValueEx(hKeyResult, strKey.c_str(), 0, &dwDataType, lpValue, &dwSize);
                    delete[] lpValue;
                    break;
                }
            case REG_SZ:
                {
                    //分配内存大小
                    TCHAR* lpValue = new TCHAR[dwSize];
                    memset(lpValue, 0, dwSize * sizeof(TCHAR));
                    //获取注册表中指定的键所对应的值
                    if (ERROR_SUCCESS == ::RegQueryValueEx(hKeyResult, strKey.c_str(), 0, &dwDataType, (LPBYTE)lpValue, &dwSize))
                    {
                        strValue = lpValue;
                    }
                    delete[] lpValue;
                    break;
                }
            default:
                break;
            }
        }

        //关闭注册表
        ::RegCloseKey(hKeyResult);

        return strValue;
    }
}
