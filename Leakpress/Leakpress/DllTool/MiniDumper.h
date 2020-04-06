#ifndef MINIDUMPER_H
#define MINIDUMPER_H

// �ز����ٵ�ͷ�ļ���lib���ļ� 
#include <windows.h>  
#include <DbgHelp.h>  
#include <stdlib.h>  

#ifndef _M_IX86  
#error "The following code only works for x86!"  
#endif  


namespace MiniDumper
{
    DLL_API void SetProcessName(LPCTSTR);
    DLL_API void RegisterExceptionhandler();
    DLL_API void UnregisterExceptionhandler();
};

#endif