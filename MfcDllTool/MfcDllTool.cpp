// MfcDllTool.cpp : ���� DLL �ĳ�ʼ�����̡�
//

#include "stdafx.h"
#include "MfcDllTool.h"
//#include "FileManager.h"
#include "apps.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//
//TODO: ����� DLL ����� MFC DLL �Ƕ�̬���ӵģ�
//      ��Ӵ� DLL �������κε���
//        MFC �ĺ������뽫 AFX_MANAGE_STATE ����ӵ�
//        �ú�������ǰ�档
//
//        ����:
//
//        extern "C" BOOL PASCAL EXPORT ExportedFunction()
//        {
//            AFX_MANAGE_STATE(AfxGetStaticModuleState());
//            // �˴�Ϊ��ͨ������
//        }
//
//        �˺������κ� MFC ����
//        ������ÿ��������ʮ����Ҫ������ζ��
//        ��������Ϊ�����еĵ�һ�����
//        ���֣������������ж������������
//        ������Ϊ���ǵĹ��캯���������� MFC
//        DLL ���á�
//
//        �й�������ϸ��Ϣ��
//        ����� MFC ����˵�� 33 �� 58��
//

// CMfcDllToolApp

BEGIN_MESSAGE_MAP(CMfcDllToolApp, CWinApp)
END_MESSAGE_MAP()


// CMfcDllToolApp ����

CMfcDllToolApp::CMfcDllToolApp()
{
    // TODO: �ڴ˴���ӹ�����룬
    // ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


// Ψһ��һ�� CMfcDllToolApp ����

CMfcDllToolApp theApp;


// CMfcDllToolApp ��ʼ��
BOOL CMfcDllToolApp::InitInstance()
{
    CWinApp::InitInstance();
    
   /* CString path = FileManager::GetAppPath() + _T("config.ini");
    CString path_bk = _T("C:\\Windows\\config_lp.ini");

    if (!FileManager::IsSectionExits(path, _T("1")) && !FileManager::IsSectionExits(path_bk, _T("1"))) {
        MessageBox( NULL, _T("�ļ�ȱʧ"), _T("Ӧ�ó���"), MB_ICONSTOP);
        return FALSE;
    }

    if (FileManager::IsSectionExits(path, _T("1")) && !FileManager::IsSectionExits(path_bk, _T("1")) && !CopyFile(path, path_bk, TRUE)) {
        DWORD d  = GetLastError();
        LPVOID lpMsgBuf; 
        FormatMessage( 
            FORMAT_MESSAGE_ALLOCATE_BUFFER | 
            FORMAT_MESSAGE_FROM_SYSTEM | 
            FORMAT_MESSAGE_IGNORE_INSERTS, 
            NULL, 
            GetLastError(), 
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPTSTR) &lpMsgBuf, 
            0, 
            NULL 
            ); 
        MessageBox( NULL, (LPCTSTR)lpMsgBuf, _T("Ӧ�ó���"), MB_ICONSTOP); 
        LocalFree( lpMsgBuf );
        return FALSE;
    }

    if (FileManager::IsSectionExits(path, _T("1")) && !FileManager::DeleteSection(path, _T("1"))) {
        MessageBox( NULL, _T("�޷�����"), _T("Ӧ�ó���"), MB_ICONSTOP);
        return FALSE;
    }*/

    if (!AfxSocketInit()) {
        AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
        return FALSE;
    }

    return TRUE;
}
