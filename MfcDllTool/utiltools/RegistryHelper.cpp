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
        BOOL Read(LPCTSTR lpValueName, int *pnVal);
        BOOL Read(LPCTSTR lpValueName, DWORD *pdwVal);
        BOOL Read(LPCTSTR lpValueName, String *lpVal);
        BOOL Write(LPCTSTR lpSubKey, int nVal);
        BOOL Write(LPCTSTR lpSubKey, DWORD dwVal);
        BOOL Write(LPCTSTR lpSubKey, LPCTSTR lpVal);
        BOOL Read(LPCTSTR lpValueName, vector<String> *lpVal);
        BOOL Write(LPCTSTR lpValueName, vector<String> lpVal);

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
            accessMask = KEY_ALL_ACCESS; // ��Ҫ����ԱȨ��
        } else {
            accessMask = KEY_READ;
        }
        // System32 ��� 64 λ��ע���
        // SysWOW64 ��� 32 λ��ע���
        // 32 λ������64 λϵͳ�ϣ�ע����Զ���ת�� WoW6432Node ����
        // 32λ�����ע���Ĭ��Ϊ��KEY_WOW64_32KEY����־
        // 64λ�����ע���Ĭ��Ϊ��KEY_WOW64_64KEY����־
        // 32λ���������64λ��ע������ǲ���bWow6432Node��·�����������ϡ�KEY_WOW64_64KEY����
        if (bWow6432Node) {
            accessMask |= KEY_WOW64_32KEY; // ���� 32 λע���
        } else {
            accessMask |= KEY_WOW64_64KEY; // ���� 64 λע���
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

    // ���ַ���
    BOOL RegistryHelperPrivate::Read(LPCTSTR lpValueName, String *lpVal)
    {
        assert(m_hKey);
        assert(lpValueName);
        assert(lpVal);

        DWORD dwType; // ����
        DWORD dwSize; // ����
        // ��ȡ����ĳ��ȼ�������������
        long lReturn = ::RegQueryValueEx(m_hKey, lpValueName, NULL, &dwType, NULL, &dwSize);
        if (ERROR_SUCCESS != lReturn || REG_SZ != dwType) {
            return FALSE;
        }

        char *pszString = new char[dwSize];
        lReturn = ::RegQueryValueEx(m_hKey, lpValueName, NULL, &dwType, (LPBYTE)pszString, &dwSize);
        if (ERROR_SUCCESS == lReturn) {
            *lpVal = pszString;
        }
        delete[] pszString;
        return ERROR_SUCCESS == lReturn;
    }

    // �����ַ���
    BOOL RegistryHelperPrivate::Read(LPCTSTR lpValueName, vector<String> *lpVal)
    {
        assert(m_hKey);
        assert(lpValueName);
        assert(lpVal);

        DWORD dwType; // ����
        DWORD dwSize; // ����
        // ��ȡ����ĳ��ȼ�������������
        long lReturn = ::RegQueryValueEx(m_hKey, lpValueName, NULL, &dwType, NULL, &dwSize);
        if (ERROR_SUCCESS != lReturn || REG_MULTI_SZ != dwType) {
            return FALSE;
        }

        unsigned char *pszString = new unsigned char[dwSize];
        lReturn = ::RegQueryValueEx(m_hKey, lpValueName, NULL, &dwType, pszString, &dwSize);
        if (ERROR_SUCCESS == lReturn) {
            size_t i = 0;
            const char *p = (const char *)pszString;
            while (i < dwSize - 1) { // ȥ����ĩβ��1���ַ�
                lpVal->push_back(p + i);
                i += strlen(p + i) + 1;
            }
        }
        delete[] pszString;
        return ERROR_SUCCESS == lReturn;
    }

    BOOL RegistryHelperPrivate::Write(LPCTSTR lpSubKey, int nVal)
    {
        assert(m_hKey);
        assert(lpSubKey);

        DWORD dwValue = (DWORD)nVal;
        long lReturn = ::RegSetValueEx(m_hKey, lpSubKey, 0L, REG_DWORD, (CONST BYTE *)&dwValue, sizeof(DWORD));

        if(ERROR_SUCCESS == lReturn)
            return TRUE;
        return FALSE;
    }

    BOOL RegistryHelperPrivate::Write(LPCTSTR lpSubKey, DWORD dwVal)
    {
        assert(m_hKey);
        assert(lpSubKey);

        long lReturn = ::RegSetValueEx(m_hKey, lpSubKey, 0L, REG_DWORD, (CONST BYTE *)&dwVal, sizeof(DWORD));

        if(ERROR_SUCCESS == lReturn)
            return TRUE;
        return FALSE;
    }

    // д�ַ���
    BOOL RegistryHelperPrivate::Write(LPCTSTR lpValueName, LPCTSTR lpVal)
    {
        assert(m_hKey);
        assert(lpValueName);
        assert(lpVal);

        long lReturn = ::RegSetValueEx(m_hKey, lpValueName, 0L, REG_SZ, (CONST BYTE *)lpVal, strlen(lpVal) + 1);

        if(lReturn==ERROR_SUCCESS)
            return TRUE;
        return FALSE;
    }

    // д���ַ���
    BOOL RegistryHelperPrivate::Write(LPCTSTR lpValueName, vector<String> lpVal)
    {
        assert(m_hKey);
        assert(lpValueName);

        // �����ַ�����
        size_t length = 0;
        for (size_t i= 0; i < lpVal.size(); ++i) {
            length += strlen(lpVal[i].c_str()) + 1;
        }
        length++; // ��ĩβ���1���ַ�

        // ת��Ϊ�ַ�����
        unsigned char *pszString = new unsigned char[length];
        memset(pszString, 0, length);
        char *p = (char *)pszString;
        for (size_t i = 0; i < lpVal.size(); ++i) {
            const char *s = lpVal[i].c_str();
            size_t len = strlen(s) + 1;
            strcpy(p, s);
            p += len;
        }

        long lReturn = ::RegSetValueEx(m_hKey, lpValueName, 0L, REG_MULTI_SZ, (CONST BYTE *)pszString, length);
        delete []pszString;

        if(lReturn==ERROR_SUCCESS)
            return TRUE;
        return FALSE;
    }
}

#define READ(m_hKey, lpSubKey, value, bWow6432Node) \
    do { \
    RegistryHelperPrivate d(m_hKey);\
    if (!d.Open(lpSubKey, bWow6432Node, false)) { \
    return false; \
    } \
    return !!d.Read(lpValueName, &value); \
    } while(0)
#define WRITE(m_hKey, lpSubKey, value, bWow6432Node) \
    do { \
    RegistryHelperPrivate d(m_hKey);\
    if (!d.Open(lpSubKey, bWow6432Node, true)) { \
    return false; \
    } \
    return !!d.Write(lpValueName, value); \
    } while(0)

namespace UtilTools
{
    bool RegistryHelper::read(HKEY m_hKey, LPCTSTR lpSubKey, LPCTSTR lpValueName, int &value, bool bWow6432Node)
    {
        READ(m_hKey, lpSubKey, value, bWow6432Node);
    }

    bool RegistryHelper::read(HKEY m_hKey, LPCTSTR lpSubKey, LPCTSTR lpValueName, DWORD &value, bool bWow6432Node)
    {
        READ(m_hKey, lpSubKey, value, bWow6432Node);
    }

    bool RegistryHelper::read(HKEY m_hKey, LPCTSTR lpSubKey, LPCTSTR lpValueName, String &value, bool bWow6432Node)
    {
        READ(m_hKey, lpSubKey, value, bWow6432Node);
    }

    bool RegistryHelper::write(HKEY m_hKey, LPCTSTR lpSubKey, LPCTSTR lpValueName, int value, bool bWow6432Node)
    {
        WRITE(m_hKey, lpSubKey, value, bWow6432Node);
    }

    bool RegistryHelper::write(HKEY m_hKey, LPCTSTR lpSubKey, LPCTSTR lpValueName, DWORD value, bool bWow6432Node)
    {
        WRITE(m_hKey, lpSubKey, value, bWow6432Node);
    }

    bool RegistryHelper::write(HKEY m_hKey, LPCTSTR lpSubKey, LPCTSTR lpValueName, String value, bool bWow6432Node)
    {
        WRITE(m_hKey, lpSubKey, value.c_str(), bWow6432Node);
    }

    bool RegistryHelper::read(HKEY m_hKey, LPCTSTR lpSubKey, LPCTSTR lpValueName, std::vector<String> &value, bool bWow6432Node)
    {
        READ(m_hKey, lpSubKey, value, bWow6432Node);
    }

    bool RegistryHelper::write(HKEY m_hKey, LPCTSTR lpSubKey, LPCTSTR lpValueName, std::vector<String> value, bool bWow6432Node)
    {
        WRITE(m_hKey, lpSubKey, value, bWow6432Node);
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

        //��ע���SysWOW64 ��� 32 λ��ע���
        REGSAM accessMask = bWow64_32KEY ? KEY_READ | KEY_WOW64_64KEY : KEY_READ;
        if(ERROR_SUCCESS == ::RegOpenKeyEx(hKey, strUrl.c_str(), 0, accessMask, &hKeyResult))
        {
            // ��ȡ����ĳ���dwSize������dwDataType
            ::RegQueryValueEx(hKeyResult, strKey.c_str(), 0, &dwDataType, NULL, &dwSize); 
            switch (dwDataType)
            {
            case REG_MULTI_SZ:
                {
                    //�����ڴ��С
                    BYTE* lpValue = new BYTE[dwSize];
                    //��ȡע�����ָ���ļ�����Ӧ��ֵ
                    LONG lRet = ::RegQueryValueEx(hKeyResult, strKey.c_str(), 0, &dwDataType, lpValue, &dwSize);
                    delete[] lpValue;
                    break;
                }
            case REG_SZ:
                {
                    //�����ڴ��С
                    TCHAR* lpValue = new TCHAR[dwSize];
                    memset(lpValue, 0, dwSize * sizeof(TCHAR));
                    //��ȡע�����ָ���ļ�����Ӧ��ֵ
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

        //�ر�ע���
        ::RegCloseKey(hKeyResult);

        return strValue;
    }
}
