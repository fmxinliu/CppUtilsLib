// MfcDllTool.h : MfcDllTool DLL ����ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
    #error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"        // ������


// CMfcDllToolApp
// �йش���ʵ�ֵ���Ϣ������� MfcDllTool.cpp
//

class CMfcDllToolApp : public CWinApp
{
public:
    CMfcDllToolApp();

// ��д
public:
    virtual BOOL InitInstance();

    DECLARE_MESSAGE_MAP()
};
