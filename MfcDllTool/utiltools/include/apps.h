#pragma once
#include "dllexport.h"

namespace UtilTools
{
    class DLL_API Apps
    {
        // Ȩ��
        STATIC bool hasAdminPrivilege();
        STATIC bool hasStarted(LPTSTR title);
        STATIC bool runAsAdmin(LPTSTR exepath);
        STATIC bool restartWithAdminPrivilege(LPTSTR exepath);
        STATIC bool enableDebugPrivilege(); // ���������̵�����Ȩ
        STATIC bool setPrivilege(HANDLE pHandle, LPCTSTR lpszPrivilege, bool enable); // ����/�رս�����Ȩ

        // ������ʾ
        STATIC bool showWindow(LPTSTR title);
        STATIC bool hideWindow(LPTSTR title, bool hideTrayIcon = false);

        // �����ö�
        STATIC bool bringWindowToBack(LPTSTR title);
        STATIC bool bringWindowToTop(LPTSTR title);
        STATIC bool fixedWindowToTop(LPTSTR title);
        STATIC bool unfixWindowToTop(LPTSTR title);
        STATIC bool moveWindow(LPTSTR title, HWND hWndInsertAfter, int nCmdShow = SW_SHOWNORMAL);
    };
};
