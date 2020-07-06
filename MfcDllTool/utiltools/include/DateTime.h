#pragma once
#include "dllexport.h"

namespace UtilTools
{
    class DLL_API DateTime
    {
    public:
        STATIC String toString();
        STATIC String toString(String format);
    };
}
