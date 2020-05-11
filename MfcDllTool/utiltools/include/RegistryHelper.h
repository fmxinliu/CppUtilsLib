#pragma once
#include "dllexport.h"

namespace UtilTools
{
    class DLL_API RegistryHelper
    {
    public:
        //---------------------------------------------------------------
        //function: 
        //     GetRegValue ��ȡע�����ָ������ֵ
        //Access:
        //      public 
        //Parameter:
        //     [in] int nKeyType - ע���������ͣ�����Ĳ���ֻ������������ֵ��
        //               0:HKEY_CLASSES_ROOT
        //               1:HKEY_CURRENT_USER
        //               2:HKEY_LOCAL_MACHINE
        //               3:HKEY_USERS
        //               4:HKEY_PERFORMANCE_DATA
        //               5:HKEY_CURRENT_CONFIG
        //               6:HKEY_DYN_DATA
        //               7:HKEY_CURRENT_USER_LOCAL_SETTINGS
        //               8:HKEY_PERFORMANCE_TEXT
        //               9:HKEY_PERFORMANCE_NLSTEXT
        //     [in] const std::string & strUrl - Ҫ���� �ļ���·��
        //     [in] const std::string & strKey - ָ���ļ�
        //Returns:
        //     String - ָ������ֵ
        //Remarks:
        //     ...
        //---------------------------------------------------------------
        static String GetRegValue(int nKeyType, const String& strUrl, const String& strKey, bool bWow64_32KEY = true);
    };
}

