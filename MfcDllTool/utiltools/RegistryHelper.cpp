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
        enum AccessMode { OnlyRead, AllAccess }; // ����Ȩ��
        enum PrivilegeMode { Backup, Restore }; // ��д��Ȩ

    public:
        void Close();
        BOOL IsKeyExist(LPCTSTR lpSubKey, AccessType type = RegbitDefault);
        BOOL IsValueExist(LPCTSTR lpValueName, AccessType type = RegbitDefault);
        BOOL Open(LPCTSTR lpSubKey, AccessMode mode, AccessType type = RegbitDefault);
        BOOL CreateKey(LPCTSTR lpSubKey, AccessType type = RegbitDefault);
        BOOL DeleteKey(LPCTSTR lpSubKey, AccessType type = RegbitDefault);
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
        BOOL SetPrivilege(PrivilegeMode mode, BOOL bEnablePrivilege);
        REGSAM GetAccessMask(AccessMode mode, AccessType type);
        REGSAM GetAccessMask(AccessType type);

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

    BOOL RegistryHelperPrivate::SetPrivilege(PrivilegeMode mode, BOOL bEnablePrivilege)
    {
        BOOL ret;
        LUID luid;
        HANDLE hToken;
        TOKEN_PRIVILEGES tp;
        LPCTSTR lpszPrivilege;

        ret = OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);
        if (!ret) {
            OutputDebugString(_T("�޷��򿪽��̲�������\n"));
        }

        if (Backup == mode) {
            lpszPrivilege = SE_BACKUP_NAME;
        } else if (Restore == mode) {
            lpszPrivilege = SE_RESTORE_NAME;
        } else {
            ret = FALSE;
        }

        if (ret && !LookupPrivilegeValue(NULL, lpszPrivilege, &luid)) {
            OutputDebugString(_T("�޷��鿴ϵͳȨ�޵���Ȩֵ\n"));
            ret = FALSE;
        }

        tp.PrivilegeCount = 1;
        tp.Privileges[0].Luid = luid;
        if (bEnablePrivilege) {
            tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
        } else {
            tp.Privileges[0].Attributes = 0;
        }

        if (ret && !AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(TOKEN_PRIVILEGES), NULL, NULL)) {
            OutputDebugString(_T("�޷�ָ���������Ƶ���Ȩ\n"));
            ret = FALSE;
        }

        if(!ret || ERROR_SUCCESS != GetLastError()) {
            OutputDebugString(_T("Ȩ������ʧ�ܣ�\n"));
            ret = FALSE;
        }

        CloseHandle(hToken);
        return ret;
    }

    REGSAM RegistryHelperPrivate::GetAccessMask(AccessMode mode, AccessType type)
    {
        REGSAM access;
        if (OnlyRead == mode) {
            access = KEY_READ;
        } else {
            access = KEY_ALL_ACCESS; // ��Ҫ����ԱȨ��
        }
        if (RegbitDefault != type) {
            access |= GetAccessMask(type);
        }
        return access;
    }

    REGSAM RegistryHelperPrivate::GetAccessMask(AccessType type)
    {
        REGSAM access;
        // System32 ��� 64 λ��ע���
        // SysWOW64 ��� 32 λ��ע���
        // 32 λ������64 λϵͳ�ϣ�ע����Զ���ת�� WoW6432Node ����
        // 32λ�����ע���Ĭ��Ϊ��KEY_WOW64_32KEY����־
        // 64λ�����ע���Ĭ��Ϊ��KEY_WOW64_64KEY����־
        // 32λ���������64λ��ע������ǲ���bWow6432Node��·�����������ϡ�KEY_WOW64_64KEY����
        if (Regbit32 == type) {
            access = KEY_WOW64_32KEY; // ���� 32 λע���
        } else if (Regbit64 == type) {
            access = KEY_WOW64_64KEY; // ���� 64 λע���
        } else {
            // ����Ӧ�ó����λ����ȷ�����ʶ���λ��ע���
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
            OutputDebugString(_T("ERROR_FILE_NOT_FOUND��ע��������\n"));
        } else if (ERROR_ACCESS_DENIED == lReturn) {
            OutputDebugString(_T("ERROR_ACCESS_DENIED��ע�������ʱ��ܾ�\n"));
        }
        return FALSE;
    }

    BOOL RegistryHelperPrivate::IsValueExist(LPCTSTR lpValueName, AccessType type)
    {
        assert(m_hKey);
        assert(lpValueName);
        long lReturn = ::RegQueryValueEx(m_hKey, lpValueName, NULL, NULL, NULL, NULL);
        if (ERROR_FILE_NOT_FOUND == lReturn) {
            return FALSE;
        }
        return TRUE;
    }

    BOOL RegistryHelperPrivate::IsKeyExist(LPCTSTR lpSubKey, AccessType type)
    {
        assert(m_hKey);
        assert(lpSubKey);

        HKEY hKey;
        REGSAM accessMask = GetAccessMask(OnlyRead, type);
        long lReturn = ::RegOpenKeyEx(m_hKey, lpSubKey, 0L, accessMask, &hKey);
        if (ERROR_FILE_NOT_FOUND == lReturn) {
            return FALSE;
        }
        return TRUE;
    }

    BOOL RegistryHelperPrivate::CreateKey(LPCTSTR lpSubKey, AccessType type)
    {
        assert(m_hKey);
        assert(lpSubKey);

        HKEY hKey;
        REGSAM accessMask = GetAccessMask(AllAccess, type);
        long lReturn = ::RegCreateKeyEx(m_hKey, lpSubKey, 0L, NULL, REG_OPTION_NON_VOLATILE, accessMask, NULL, &hKey, NULL);
        if(ERROR_SUCCESS == lReturn) {
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
        if(ERROR_SUCCESS == lReturn) {
            return TRUE;
        }
        return FALSE;
    }

    BOOL RegistryHelperPrivate::DeleteKey(LPCTSTR lpSubKey, AccessType type)
    {
        assert(m_hKey);
        assert(lpSubKey);

        long lReturn;
        if (RegbitDefault == type) {
            lReturn = ::RegDeleteKey(m_hKey, lpSubKey);
        } else {
            REGSAM accessMask = GetAccessMask(type);
            lReturn = ::RegDeleteKeyEx(m_hKey, lpSubKey, accessMask, 0);
        }
        if(ERROR_SUCCESS == lReturn) {
            return TRUE;
        }
        return FALSE;
    }

    BOOL RegistryHelperPrivate::RestoreKey(LPCTSTR lpFileName)
    {
        assert(m_hKey);
        assert(lpFileName);

        SetPrivilege(Restore, TRUE); // �򿪻ָ���Ȩ
        long lReturn = ::RegRestoreKey(m_hKey, lpFileName, REG_FORCE_RESTORE);
        if(ERROR_SUCCESS == lReturn) {
            SetPrivilege(Restore, FALSE);
            return TRUE;
        }
        if (ERROR_FILE_NOT_FOUND == lReturn) {
            OutputDebugString(_T("ERROR_FILE_NOT_FOUND���ļ��Ѿ�����\n"));
        } else if (ERROR_PRIVILEGE_NOT_HELD == lReturn) {
            OutputDebugString(_T("ERROR_PRIVILEGE_NOT_HELD��û�ж�дȨ��\n"));
        } else if (ERROR_ACCESS_DENIED == lReturn) {
            OutputDebugString(_T("ERROR_ACCESS_DENIED��ע�������ʱ��ܾ�\n"));
        }
        SetPrivilege(Restore, FALSE);
        return FALSE;
    }

    BOOL RegistryHelperPrivate::SaveKey(LPCTSTR lpFileName)
    {
        assert(m_hKey);
        assert(lpFileName);

        if (PathFileExists(lpFileName)) {
            DeleteFile(lpFileName);
        }
        SetPrivilege(Backup, TRUE); // �򿪱�����Ȩ
        long lReturn = ::RegSaveKey(m_hKey, lpFileName, NULL);
        if(ERROR_SUCCESS == lReturn) {
            SetPrivilege(Backup, FALSE);
            return TRUE;
        }
        if (ERROR_ALREADY_EXISTS == lReturn) {
            OutputDebugString(_T("ERROR_ALREADY_EXISTS���ļ��Ѿ�����\n"));
        } else if (ERROR_PRIVILEGE_NOT_HELD == lReturn) {
            OutputDebugString(_T("ERROR_PRIVILEGE_NOT_HELD��û�ж�дȨ��\n"));
        } else if (ERROR_ACCESS_DENIED == lReturn) {
            OutputDebugString(_T("ERROR_ACCESS_DENIED��ע�������ʱ��ܾ�\n"));
        }
        SetPrivilege(Backup, FALSE);
        return FALSE;
    }

    BOOL RegistryHelperPrivate::ReadCheck(HKEY hKey, LPCTSTR lpValueName, DWORD &dwType, DWORD &dwSize)
    {
        assert(hKey);
        assert(lpValueName);

        // ��ȡ�������͡����ݳ���
        long lReturn = ::RegQueryValueEx(hKey, lpValueName, NULL, &dwType, NULL, &dwSize);
        if (ERROR_SUCCESS != lReturn) {
            return FALSE;
        }

        return TRUE;
    }

#define READ_CHECK() \
    DWORD dwType; /*��������*/ \
    DWORD dwSize; /*���ݳ���*/ \
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
        DWORD dwValue; // ����
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
        DWORD dwValue; // ����
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
        QWORD qwValue; // ����
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
            size_t bytes = dwSize - sizeof(TCHAR); // ȥ��ĩβ����Ľ�����
            LPCTSTR end = (LPCTSTR)(pszString + bytes); // ����Ľ�����
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

        // ͳ���ֽ���
        DWORD dwSize = 0;
        for (size_t i = 0; i < value.size(); ++i) {
            dwSize += (1 + _tcslen(value[i].c_str())) * sizeof(TCHAR);
        }
        dwSize += sizeof(TCHAR); // ĩβ�����һ�������ַ�

        // ת��Ϊ�ֽ�����
        LPBYTE pszString = new BYTE[dwSize];
        LPTSTR p = (LPTSTR)pszString;
        for (size_t i = 0; i < value.size(); ++i) {
            LPCTSTR s = value[i].c_str();
            size_t len = _tcslen(s) + 1;
            _tcscpy_s(p, len, s);
            p += len;
        }
        *p  = '\0'; // �����ַ��������������ַ�������

        // дע���
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

namespace UtilTools
{
    bool RegistryHelper::createKey(HKEY hKey, LPCTSTR lpSubKey, AccessType type)
    {
        RegistryHelperPrivate d(hKey);
        if (!d.IsKeyExist(lpSubKey, type)) {
            return !!d.CreateKey(lpSubKey, type);
        }
        return true;
    }

    bool RegistryHelper::deleteKey(HKEY hKey, LPCTSTR lpSubKey, AccessType type)
    {
        RegistryHelperPrivate d(hKey);
        if (d.IsKeyExist(lpSubKey, type)) {
            return !!d.DeleteKey(lpSubKey, type);
        }
        return true;
    }

    bool RegistryHelper::deleteValue(HKEY hKey, LPCTSTR lpSubKey, LPCTSTR lpValueName, AccessType type)
    {
        RegistryHelperPrivate d(hKey);
        if (!d.Open(lpSubKey, RegistryHelperPrivate::AllAccess, type)) {
            return false;
        }
        if (d.IsValueExist(lpValueName, type)) {
            return !!d.DeleteValue(lpValueName);
        }
        return true;
    }

    bool RegistryHelper::saveKey(HKEY hKey, LPCTSTR lpSubKey, LPCTSTR lpFileName, AccessType type)
    {
        RegistryHelperPrivate d(hKey);
        if (!d.Open(lpSubKey, RegistryHelperPrivate::AllAccess, type)) {
            return false;
        }
        return !!d.SaveKey(lpFileName);
    }

    bool RegistryHelper::restoreKey(HKEY hKey, LPCTSTR lpSubKey, LPCTSTR lpFileName, AccessType type)
    {
        RegistryHelperPrivate d(hKey);
        if (!d.Open(lpSubKey, RegistryHelperPrivate::AllAccess, type)) {
            return false;
        }
        return !!d.RestoreKey(lpFileName);
    }
}
