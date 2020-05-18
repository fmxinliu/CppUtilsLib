#include "StdAfx.h"
#include "apps.h"

namespace UtilTools
{
    // �жϹ���ԱȨ��
    bool Apps::hasAdminPrivilege()
    {
        BOOL b;
        SID_IDENTIFIER_AUTHORITY NtAuthority = SECURITY_NT_AUTHORITY;
        PSID AdministratorsGroup;
        b = AllocateAndInitializeSid(&NtAuthority, 2, SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_ADMINS, 0, 0, 0, 0, 0, 0, &AdministratorsGroup);
        if (b) {
            if (!CheckTokenMembership(NULL, AdministratorsGroup, &b)) {
                b = FALSE;
            }
            FreeSid(AdministratorsGroup);
        }
        return !!(b);
    }

    // �жϳ����Ƿ�������
    bool Apps::hasStarted(LPTSTR title)
    {
        HANDLE hMutex = CreateMutex(NULL, TRUE, title); // ��������̹�����mutex
        DWORD dw = GetLastError();
        if (hMutex && GetLastError() == ERROR_ALREADY_EXISTS) {
            //DWORD wait_success = WaitForSingleObject( hMutex, 0 );
            //if (wait_success == WAIT_OBJECT_0 || wait_success == WAIT_ABANDONED)
            return true;
        }

        ReleaseMutex(hMutex);
        //CloseHandle(hMutex); // �������̲��˳���������mutex
        return false;
    }

    bool Apps::runAsAdmin(LPTSTR exepath)
    //BOOL runAsAdmin(HWND hWnd, LPTSTR lpFile, LPTSTR lpParameters)
    {
        HWND hWnd = NULL;
        LPTSTR lpFile = exepath;
        LPTSTR lpParameters = NULL;

        SHELLEXECUTEINFO   sei;
        ZeroMemory(&sei, sizeof(sei));

        sei.cbSize = sizeof(SHELLEXECUTEINFOW);
        sei.hwnd = hWnd;
        sei.fMask = SEE_MASK_FLAG_DDEWAIT | SEE_MASK_FLAG_NO_UI;
        sei.lpVerb = TEXT("runas");
        sei.lpFile = lpFile;
        sei.lpParameters = lpParameters;
        sei.nShow = SW_SHOWNORMAL;

        if (!ShellExecuteEx(&sei)) {
            printf("Error: ShellExecuteEx failed 0x%x\n", GetLastError());
            return FALSE;
        }
        return TRUE;
    }

    bool Apps::restartWithAdminPrivilege(LPTSTR exepath)
    {
        if (hasStarted(exepath)) {
            exit(0);
        }

        if (!hasAdminPrivilege()) {
            bool b4 = runAsAdmin(exepath);
            exit(0);
        }

        bool b3 = enableDebugPrivilege();
        return true;
    }

    //*********************************************
    // ��������: enableDebugPrivilege
    // ����˵��: ���������̵�����Ȩ�������ԱȨ�ޡ�
    // ��    ʽ��public
    // �������: void
    // ���������void
    // �� �� ֵ: true-��Ȩ�ɹ���false-��Ȩʧ��
    // ��    ��:
    // ����ʱ��: 2018-11-5
    // ��    ע��
    //*********************************************
    bool Apps::enableDebugPrivilege()
    {
        return setPrivilege(GetCurrentProcess(), SE_DEBUG_NAME, true);
    }

    bool Apps::setPrivilege(HANDLE pHandle, LPCTSTR lpszPrivilege, bool enable)
    {
        LUID luid;
        HANDLE hToken;
        TOKEN_PRIVILEGES tkp;

        if (!OpenProcessToken(pHandle, TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) {
            return false;
        }

        if (!LookupPrivilegeValue(NULL, lpszPrivilege, &luid)) {
            __try {
                if (hToken) {
                    CloseHandle(hToken);
                }
            } __except(EXCEPTION_EXECUTE_HANDLER) {};
            return false;
        }

        tkp.PrivilegeCount = 1;
        tkp.Privileges[0].Luid = luid;
        if (enable) {
            tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
        } else {
            tkp.Privileges[0].Attributes = 0;
        }
        if (!AdjustTokenPrivileges(hToken, FALSE, &tkp, sizeof(tkp), NULL, NULL)) {
            __try {
                if(hToken) {
                    CloseHandle(hToken);
                }
            } __except(EXCEPTION_EXECUTE_HANDLER) {};
            return false;
        }

        DWORD dw = GetLastError();
        CloseHandle(hToken);
        return ERROR_SUCCESS == dw;
    }

    bool Apps::showWindow(LPTSTR title)
    {
        return moveWindow(title, HWND_NOTOPMOST, SW_SHOWNORMAL);
    }

    bool Apps::hideWindow(LPTSTR title, bool hideTrayIcon)
    {
        if (hideTrayIcon)
            return moveWindow(title, HWND_NOTOPMOST, SW_HIDE); // ״̬��ͼ�겻�ɼ�
        return moveWindow(title, HWND_NOTOPMOST, SW_MINIMIZE);
    }

    bool Apps::bringWindowToBack(LPTSTR title)
    {
        return moveWindow(title, HWND_BOTTOM);
    }

    bool Apps::bringWindowToTop(LPTSTR title)
    {
        return moveWindow(title, HWND_NOTOPMOST);
    }

    bool Apps::fixedWindowToTop(LPTSTR title)
    {
        return moveWindow(title, HWND_TOPMOST);
    }

    bool Apps::unfixWindowToTop(LPTSTR title)
    {
        return moveWindow(title, HWND_NOTOPMOST);
    }

    bool Apps::moveWindow(LPTSTR title, HWND hWndInsertAfter, int nCmdShow)
    {
        HWND hWnd = FindWindow(NULL, title);
        if (hWnd) {
            HWND hForeWnd = GetForegroundWindow();
            DWORD dwForeID = GetWindowThreadProcessId(hForeWnd, NULL);
            DWORD dwCurID = GetCurrentThreadId();
            AttachThreadInput(dwCurID, dwForeID, TRUE);
            ShowWindow(hWnd, nCmdShow);
            SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
            SetWindowPos(hWnd, hWndInsertAfter, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
            SetForegroundWindow(hWnd);
            AttachThreadInput(dwCurID, dwForeID, FALSE);
        }
        return !!(hWnd);
    }
}
