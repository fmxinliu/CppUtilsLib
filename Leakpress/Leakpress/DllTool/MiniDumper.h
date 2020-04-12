#ifndef MINIDUMPER_H
#define MINIDUMPER_H

#include "dllexport.h" 

#if !defined(_M_IX86)
    #error "The following code only works for x86!"  
#endif  

namespace MiniDumper
{
    DLL_API void SetProcessName(LPCTSTR);
    DLL_API void RegisterExceptionhandler();
    DLL_API void UnregisterExceptionhandler();
};

#endif