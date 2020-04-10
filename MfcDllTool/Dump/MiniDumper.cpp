#include "stdafx.h"
#include "MiniDumper.h"
#include <windows.h>
#include <dbghelp.h>
#include <shellapi.h>
#include <shlobj.h>
#include <strsafe.h>

#pragma comment(lib, "dbghelp.lib")

namespace MiniDumper
{
    LPCTSTR processSuffix = _T("");

    void SetProcessName(LPCTSTR processName)
    {
        processSuffix = processName;
    }

    BOOL IsDataSectionNeeded(const WCHAR* pModuleName)
    {
        if (pModuleName == 0)
        {
            return FALSE;
        }

        WCHAR szFileName[_MAX_FNAME] = L"";
        _wsplitpath(pModuleName, NULL, NULL, szFileName, NULL);

        if (_wcsicmp(szFileName, L"ntdll") == 0)
            return TRUE;

        return FALSE;
    }

    BOOL CALLBACK MiniDumpCallback(PVOID                            pParam, 
                                   const PMINIDUMP_CALLBACK_INPUT   pInput, 
                                   PMINIDUMP_CALLBACK_OUTPUT        pOutput)
    {
        if (pInput == 0 || pOutput == 0)
            return FALSE;

        switch (pInput->CallbackType)
        {
        case ModuleCallback:
            if (pOutput->ModuleWriteFlags & ModuleWriteDataSeg)
                if (!IsDataSectionNeeded(pInput->Module.FullPath))
                    pOutput->ModuleWriteFlags &= (~ModuleWriteDataSeg);
        case IncludeModuleCallback:
        case IncludeThreadCallback:
        case ThreadCallback:
        case ThreadExCallback:
            return TRUE;
        default:;
        }

        return FALSE;
    }

    void CreateMiniDump(PEXCEPTION_POINTERS pep, LPCTSTR szFileName)
    {
        HANDLE hDumpFile;
        BOOL bMiniDumpSuccessful;

        hDumpFile = CreateFile(szFileName, GENERIC_READ|GENERIC_WRITE, 
                    FILE_SHARE_WRITE|FILE_SHARE_READ, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);


        if ((hDumpFile != NULL) && (hDumpFile != INVALID_HANDLE_VALUE))
        {
            MINIDUMP_EXCEPTION_INFORMATION mdei;
            mdei.ThreadId = GetCurrentThreadId();
            mdei.ExceptionPointers = pep;
            mdei.ClientPointers = FALSE;

            MINIDUMP_CALLBACK_INFORMATION mci;
            mci.CallbackRoutine = (MINIDUMP_CALLBACK_ROUTINE)MiniDumpCallback;
            mci.CallbackParam = 0;

            bMiniDumpSuccessful = MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), 
                        hDumpFile, MiniDumpWithDataSegs, (pep != 0) ? &mdei : 0, NULL, &mci);

            CloseHandle(hDumpFile);
        }
    }

    LONG WINAPI UnhandledExceptionDump(PEXCEPTION_POINTERS pExceptionInfo)
    {
        TCHAR szFilePath[MAX_PATH];
        TCHAR szPathName[MAX_PATH];
        TCHAR szDriver[MAX_PATH];
        TCHAR szDirectory[MAX_PATH];
        TCHAR szFileName[MAX_PATH];
        TCHAR szExt[MAX_PATH];
        TCHAR szMutexName[MAX_PATH];

        ZeroMemory(szFilePath, MAX_PATH * sizeof(TCHAR));
        ZeroMemory(szPathName, MAX_PATH * sizeof(TCHAR));
        ZeroMemory(szDriver, MAX_PATH * sizeof(TCHAR));
        ZeroMemory(szDirectory, MAX_PATH * sizeof(TCHAR));
        ZeroMemory(szFileName, MAX_PATH * sizeof(TCHAR));
        ZeroMemory(szExt, MAX_PATH * sizeof(TCHAR));
        ZeroMemory(szMutexName, MAX_PATH * sizeof(TCHAR));

        GetModuleFileName(NULL, szPathName, MAX_PATH);
        _tsplitpath_s(szPathName, szDriver, _MAX_DRIVE, szDirectory, _MAX_DIR, szFileName, _MAX_FNAME, szExt, _MAX_EXT);

        _stprintf_s(szFilePath, _T("%s%slog"), szDriver, szDirectory);

        //HANDLE hDumpFile;
        SYSTEMTIME stLocalTime;
        //MINIDUMP_EXCEPTION_INFORMATION ExpParam;
        
        GetLocalTime(&stLocalTime);
        CreateDirectory(szFilePath, NULL);

        StringCchPrintf(szFileName, MAX_PATH, _T("%s\\MiniDmp{%04d-%02d-%02d %02d-%02d-%02d}%s.dmp"), 
                   szFilePath, 
                   stLocalTime.wYear, stLocalTime.wMonth, stLocalTime.wDay, 
                   stLocalTime.wHour, stLocalTime.wMinute, stLocalTime.wSecond,
                   processSuffix);

        // 创建 dump 文件
        CreateMiniDump(pExceptionInfo, szFileName);
        MessageBox(NULL, _T("发生异常"), _T("应用程序"), MB_ICONSTOP);

        return EXCEPTION_EXECUTE_HANDLER;
    }

    // 此函数一旦成功调用，之后对 SetUnhandledExceptionFilter 的调用将无效  
    void DisableSetUnhandledExceptionFilter()
    {
        void* addr = (void*)GetProcAddress(LoadLibrary(_T("kernel32.dll")), "SetUnhandledExceptionFilter");

        if (addr)
        {
            unsigned char code[16];
            int size = 0;

            code[size++] = 0x33;
            code[size++] = 0xC0;
            code[size++] = 0xC2;
            code[size++] = 0x04;
            code[size++] = 0x00;

            DWORD dwOldFlag, dwTempFlag;
            VirtualProtect(addr, size, PAGE_READWRITE, &dwOldFlag);
            WriteProcessMemory(GetCurrentProcess(), addr, code, size, NULL);
            VirtualProtect(addr, size, dwOldFlag, &dwTempFlag);
        }
    }

    // 注册异常处理函数
    void RegisterExceptionhandler()
    {
        SetUnhandledExceptionFilter(UnhandledExceptionDump);
    }

    void UnregisterExceptionhandler()
    {
        DisableSetUnhandledExceptionFilter();
    }
};