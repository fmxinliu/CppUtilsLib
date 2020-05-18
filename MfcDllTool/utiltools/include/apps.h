#pragma once
#include "dllexport.h"

namespace UtilTools
{
    class DLL_API Apps
    {
        // 权限
        STATIC bool hasAdminPrivilege();
        STATIC bool hasStarted(LPTSTR title);
        STATIC bool runAsAdmin(LPTSTR exepath);
        STATIC bool restartWithAdminPrivilege(LPTSTR exepath);
        STATIC bool enableDebugPrivilege(); // 开启本进程调试特权
        STATIC bool setPrivilege(HANDLE pHandle, LPCTSTR lpszPrivilege, bool enable); // 开启/关闭进程特权

        // 窗体显示
        STATIC bool showWindow(LPTSTR title);
        STATIC bool hideWindow(LPTSTR title, bool hideTrayIcon = false);

        // 窗体置顶
        STATIC bool bringWindowToBack(LPTSTR title);
        STATIC bool bringWindowToTop(LPTSTR title);
        STATIC bool fixedWindowToTop(LPTSTR title);
        STATIC bool unfixWindowToTop(LPTSTR title);
        STATIC bool moveWindow(LPTSTR title, HWND hWndInsertAfter, int nCmdShow = SW_SHOWNORMAL);
    };
};
