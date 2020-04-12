/*
Comm Base Library(WIN98/NT/2000) ver 1.1
Compile by�� BC++ 5; C++ BUILDER 4, 5, 6, X; VC++ 5, 6; VC.NET; GCC;
copyright(c) 2004.5 - 2005.8 llbird wushaojian@21cn.com
*/ 
#ifndef _CN_COMM_H_
#define _CN_COMM_H_

#pragma warning(disable: 4530)
#pragma warning(disable: 4786)
#pragma warning(disable: 4800)

#include <afx.h>
#include <assert.h>
#include <stdio.h>
#include <windows.h>
#include "icomcallback.h"
//�͵����ڵ���Ϣ WPARAM �˿ں�
#define ON_COM_RECEIVE WM_USER + 618
#define ON_COM_CTS WM_USER + 619 //LPARAM 1 valid
#define ON_COM_DSR WM_USER + 621 //LPARAM 1 valid
#define ON_COM_RING WM_USER + 623
#define ON_COM_RLSD WM_USER + 624
#define ON_COM_BREAK WM_USER + 625
#define ON_COM_TXEMPTY WM_USER + 626
#define ON_COM_ERROR WM_USER + 627 //LPARAM save Error ID
#define DEFAULT_COM_MASK_EVENT EV_RXCHAR | EV_ERR | EV_CTS | EV_DSR | EV_BREAK | EV_TXEMPTY | EV_RING | EV_RLSD


class cnComm
{
public:
//------------------------------Construction-----------------------------------
//��1������Ϊ�Ƿ��ڴ򿪴���ʱ���������̣߳� ��2������ΪIO��ʽ ������ʽ��0��/ �첽�ص���ʽ��Ĭ�ϣ�
    cnComm(bool fAutoBeginThread = true, DWORD dwIOMode =FILE_FLAG_OVERLAPPED): _dwIOMode(dwIOMode), _fAutoBeginThread(fAutoBeginThread)
    {
        Init();
    }
    virtual ~cnComm()
    {
        Close();
        UnInit();
    }
//----------------------------------Attributes----------------------------------
//�жϴ����Ƿ��
    inline bool IsOpen()
    {
        return _hCommHandle != INVALID_HANDLE_VALUE;
    }
//�жϴ����Ƿ��
    operator bool()
    {
        return _hCommHandle != INVALID_HANDLE_VALUE;
    }
//��ô��ھ��
    inline HANDLE GetHandle()
    {
        return _hCommHandle;
    }
//��ô��ھ��
    operator HANDLE()
    {
        return _hCommHandle;
    }
//��ô��ڲ��� DCB
    DCB *GetState()
    {
        return IsOpen() && ::GetCommState(_hCommHandle, &_DCB) == TRUE ?&_DCB: NULL;
    }
//���ô��ڲ��� DCB
    bool SetState(DCB *pdcb = NULL)
    {
        return IsOpen() ? ::SetCommState(_hCommHandle, pdcb == NULL ? &_DCB:pdcb) == TRUE: false;
    }
//���ô��ڲ����������ʣ�ֹͣλ���� ֧�������ַ��� "9600, 8, n, 1"
    bool SetState(TCHAR *szSetStr)
    {
        if (IsOpen())
        {
            if (::GetCommState(_hCommHandle, &_DCB) != TRUE)
            return false;
            if (::BuildCommDCB((szSetStr), &_DCB) != TRUE)
            return false;
            return ::SetCommState(_hCommHandle, &_DCB) == TRUE;
        }
        return false;
    }
//���ô��ڲ����������ʣ�ֹͣλ����
    bool SetState(DWORD dwBaudRate, DWORD dwByteSize = 8, DWORD dwParity =NOPARITY, DWORD dwStopBits = ONESTOPBIT)
    {
        if (IsOpen())
        {
            if (::GetCommState(_hCommHandle, &_DCB) != TRUE)
            return false;
            _DCB.BaudRate = dwBaudRate;
            _DCB.ByteSize = (unsigned char)dwByteSize;
            _DCB.Parity = (unsigned char)dwParity;
            _DCB.StopBits = (unsigned char)dwStopBits;
            return ::SetCommState(_hCommHandle, &_DCB) == TRUE;
        }
        return false;
    }
//��ó�ʱ�ṹ
    LPCOMMTIMEOUTS GetTimeouts(void)
    {
        return IsOpen() && ::GetCommTimeouts(_hCommHandle, &_CO) == TRUE ?&_CO: NULL;
    }
//���ó�ʱ
    bool SetTimeouts(LPCOMMTIMEOUTS lpCO)
    {
        return IsOpen() ? ::SetCommTimeouts(_hCommHandle, lpCO) == TRUE:false;
    }
//���ô��ڵ�I/O��������С
    bool SetBufferSize(DWORD dwInputSize, DWORD dwOutputSize)
    {
        return IsOpen() ? ::SetupComm(_hCommHandle, dwInputSize, dwOutputSize)== TRUE: false;
    }
//���ûص�����
    inline void SetCallBack(IComCallBack *pReceiveCallBack)
    {
        _pReceiveCallBack = pReceiveCallBack;
    }
//������Ϣ�Ĵ��ھ��
    inline void SetWnd(HWND hWnd)
    {
        assert(::IsWindow(hWnd));
        _hNotifyWnd = hWnd;
    }
//�趨����֪ͨ, �����ַ���Сֵ
    inline void SetNotifyNum(DWORD dwNum)
    {
        _dwNotifyNum = dwNum;
    }
//�߳��Ƿ�����
    inline bool IsThreadRunning()
    {
        return _hThreadHandle != NULL;
    }
//����߳̾��
    inline HANDLE GetThread()
    {
        return _hThreadHandle;
    }
//����Ҫ���ӵ��¼��� ��ǰ������Ч
    void SetMaskEvent(DWORD dwEvent = DEFAULT_COM_MASK_EVENT)
    {
        _dwMaskEvent = dwEvent;
    }
//��ö����������ַ���
    int GetInputSize()
    {
        COMSTAT Stat;
        DWORD dwError;
        return ::ClearCommError(_hCommHandle, &dwError, &Stat) == TRUE ? Stat.cbInQue : (DWORD) - 1L;
    }
//----------------------------------Operations----------------------------------
//�򿪴��� ȱʡ 9600, 8, n, 1
    bool Open(DWORD dwPort)
    {
        return Open(dwPort, 19200);
    }
//�򿪴��� ȱʡ baud_rate, 8, n, 1
    bool Open(DWORD dwPort, DWORD dwBaudRate)
    {
        if (dwPort < 1 || dwPort > 1024)
        return false;
        BindCommPort(dwPort);
        if (!OpenCommPort())
        return false;
        if (!SetupPort())
        return false;
        return SetState(dwBaudRate);
    }
//�򿪴���, ʹ������"9600, 8, n, 1"�������ַ������ô���
    bool Open(DWORD dwPort, TCHAR *szSetStr)
    {
        if (dwPort < 1 || dwPort > 1024)
        return false;
        BindCommPort(dwPort);
        if (!OpenCommPort())
        return false;
        if (!SetupPort())
        return false;
        return SetState(szSetStr);
    }
//��ȡ���� dwBufferLength���ַ��� Buffer ����ʵ�ʶ������ַ��� �ɶ���������
    DWORD Read(LPVOID Buffer, DWORD dwBufferLength, DWORD dwWaitTime = 10)
    {
        if (!IsOpen()) return 0;
        COMSTAT Stat;
        DWORD dwError;
        if (::ClearCommError(_hCommHandle, &dwError, &Stat) && dwError > 0)
        {
            ::PurgeComm(_hCommHandle,PURGE_RXABORT | PURGE_RXCLEAR);
            return 0;
        }
        if (!Stat.cbInQue) return 0;   // ������������
        unsigned long uReadLength = 0;
        dwBufferLength = dwBufferLength > Stat.cbInQue ? Stat.cbInQue :dwBufferLength;
        if (!::ReadFile(_hCommHandle, Buffer, dwBufferLength, &uReadLength,&_ReadOverlapped))
        {
            if (::GetLastError() == ERROR_IO_PENDING)
            {
                WaitForSingleObject(_ReadOverlapped.hEvent, dwWaitTime);
            // �����첽I/O
                if (!::GetOverlappedResult(_hCommHandle, &_ReadOverlapped,&uReadLength, false))
                {
                    if (::GetLastError() != ERROR_IO_INCOMPLETE) uReadLength = 0;
                }
            }
            else    uReadLength = 0;
        }
            return uReadLength;
    }
//��ȡ���� dwBufferLength - 1 ���ַ��� szBuffer ����ANSI C ģʽ�ַ���ָ�� �ʺ�һ���ַ�ͨѶ
    char *ReadString(char *szBuffer, DWORD dwBufferLength, DWORD dwWaitTime = 100)
    {
        unsigned long uReadLength = Read(szBuffer, dwBufferLength - 1, dwWaitTime);
        szBuffer[uReadLength] = '\0';
        return szBuffer;
    }
//д���� ��д�������� "abcd" or "\x0\x1\x2"
    DWORD Write(LPVOID Buffer, DWORD dwBufferLength)
    {
        if (!IsOpen())
        return 0;
        DWORD dwError;
        if (::ClearCommError(_hCommHandle, &dwError, NULL) && dwError > 0)
        ::PurgeComm(_hCommHandle, PURGE_TXABORT | PURGE_TXCLEAR);
        unsigned long uWriteLength = 0;
        if (!::WriteFile(_hCommHandle, Buffer, dwBufferLength, &uWriteLength,&_WriteOverlapped))
        if (::GetLastError() != ERROR_IO_PENDING)
        uWriteLength = 0;
        return uWriteLength;
    }
//д���� дANSI C ģʽ�ַ���ָ�� 
    DWORD Write(const char *szBuffer)
    {
        assert(szBuffer);
        return Write((void*)szBuffer, strlen(szBuffer));
    }
//������ ͬ��Ӧ��
    DWORD ReadSync(LPVOID Buffer, DWORD dwBufferLength)
    {
        if (!IsOpen())
        return 0;
        DWORD dwError;
        if (::ClearCommError(_hCommHandle, &dwError, NULL) && dwError > 0)
        {
        ::PurgeComm(_hCommHandle,PURGE_RXABORT | PURGE_RXCLEAR);
        return 0;
        }
        DWORD uReadLength = 0;
        ::ReadFile(_hCommHandle, Buffer, dwBufferLength, &uReadLength, NULL);
        return uReadLength;
    }
//д���� ͬ��Ӧ��
    DWORD WriteSync(LPVOID Buffer, DWORD dwBufferLength)
    {
        if (!IsOpen())
        return 0;
        DWORD dwError;
        if (::ClearCommError(_hCommHandle, &dwError, NULL) && dwError > 0)
        ::PurgeComm(_hCommHandle, PURGE_TXABORT | PURGE_TXCLEAR);
        unsigned long uWriteLength = 0;
        ::WriteFile(_hCommHandle, Buffer, dwBufferLength, &uWriteLength, NULL);
        return uWriteLength;
    }
//д���� szBuffer ���������ʽ�ַ��� ��������������
    DWORD Write(char *szBuffer, DWORD dwBufferLength, char *szFormat, ...)
    {
        if (!IsOpen())
        return 0;
        va_list va;
        va_start(va, szFormat);
        _vsnprintf(szBuffer, dwBufferLength, szFormat, va);
        va_end(va);
        return Write(szBuffer);
    }
//д���� szBuffer ���������ʽ�ַ��� ����黺�������� С�����
    DWORD Write(char *szBuffer, char *szFormat, ...)
    {
        if (!IsOpen())
        return 0;
        va_list va;
        va_start(va, szFormat);
        vsprintf(szBuffer, szFormat, va);
        va_end(va);
        return Write(szBuffer);
    }
//�رմ��� ͬʱҲ�رչ����߳�
    virtual void Close()
    {
        if (IsOpen())
        {
        PurgeComm(_hCommHandle, PURGE_TXABORT | PURGE_TXCLEAR);
        EndThread();
        ::CloseHandle(_hCommHandle);
        _hCommHandle = INVALID_HANDLE_VALUE;
        }
    }
//DTR ��ƽ����
    bool SetDTR(bool OnOrOff)
    {
        return IsOpen() ? EscapeCommFunction(_hCommHandle, OnOrOff ? SETDTR :CLRDTR): false;
    }
//RTS ��ƽ����
    bool SetRTS(bool OnOrOff)
    {
        return IsOpen() ? EscapeCommFunction(_hCommHandle, OnOrOff ? SETRTS :CLRRTS): false;
    }
//
    bool SetBreak(bool OnOrOff)
    {
        return IsOpen() ? EscapeCommFunction(_hCommHandle, OnOrOff ? SETBREAK: CLRBREAK): false;
    }
//�����߳̿��� �������߳�
    bool BeginThread()
    {
        if (!IsThreadRunning())
        {
            _fRunFlag = true;
            _hThreadHandle = NULL;
            DWORD id;
            _hThreadHandle = ::CreateThread(NULL, 0, CommThreadProc, this, 0,&id);
            return (_hThreadHandle != NULL);
        }
        return false;
    }
//��ͣ�����߳�
    inline bool SuspendThread()
    {
        return IsThreadRunning() ? ::SuspendThread(_hThreadHandle) !=0xFFFFFFFF: false;
    }
//�ָ������߳�
    inline bool ResumeThread()
    {
        return IsThreadRunning() ? ::ResumeThread(_hThreadHandle) !=0xFFFFFFFF: false;
    }
//��ֹ�߳�
    bool EndThread(DWORD dwWaitTime = 100)
    {
        if (IsThreadRunning())
        {
        _fRunFlag = false;
        ::SetCommMask(_hCommHandle, 0);
        ::SetEvent(_WaitOverlapped.hEvent);
        if (::WaitForSingleObject(_hThreadHandle, dwWaitTime) !=WAIT_OBJECT_0)
        if (!::TerminateThread(_hThreadHandle, 0))
        return false;
        ::CloseHandle(_hThreadHandle);
        ::ResetEvent(_WaitOverlapped.hEvent);
        _hThreadHandle = NULL;
        return true;
        }
        return false;
    }
protected:
    volatile DWORD _dwPort; //���ں�
    volatile HANDLE _hCommHandle; //���ھ��
    TCHAR _szCommStr[20]; //����COM1���Ƶ��ַ���
    DCB _DCB; //�����ʣ�ֹͣλ����
    COMMTIMEOUTS _CO; //��ʱ�ṹ
    DWORD _dwIOMode; // 0 ͬ�� Ĭ�� FILE_FLAG_OVERLAPPED�ص�I/O�첽
    OVERLAPPED _ReadOverlapped, _WriteOverlapped; // �ص�I/O
    volatile HANDLE _hThreadHandle; //�����߳�
    volatile HWND _hNotifyWnd; // ֪ͨ����
    volatile DWORD _dwNotifyNum; //���ܶ����ֽ�(>=_dwNotifyNum)����֪ͨ��Ϣ
    volatile DWORD _dwMaskEvent; //���ӵ��¼�
    volatile bool _fRunFlag; //�߳�����ѭ����־
    bool _fAutoBeginThread; //Open() �Զ� BeginThread();
    OVERLAPPED _WaitOverlapped; //WaitCommEvent use
    IComCallBack *_pReceiveCallBack; // �첽�ص�
//��ʼ��
    void Init()
    {
        memset(_szCommStr, 0, 20);
        memset(&_DCB, 0, sizeof(_DCB));
        _DCB.DCBlength = sizeof(_DCB);
        _hCommHandle = INVALID_HANDLE_VALUE;
        memset(&_ReadOverlapped, 0, sizeof(_ReadOverlapped));
        memset(&_WriteOverlapped, 0, sizeof(_WriteOverlapped));
        _ReadOverlapped.hEvent = ::CreateEvent(NULL, true, false, NULL);
        assert(_ReadOverlapped.hEvent != INVALID_HANDLE_VALUE);
        _WriteOverlapped.hEvent = ::CreateEvent(NULL, true, false, NULL);
        assert(_WriteOverlapped.hEvent != INVALID_HANDLE_VALUE);
        _hNotifyWnd = NULL;
        _dwNotifyNum = 0;
        _dwMaskEvent = DEFAULT_COM_MASK_EVENT;
        _hThreadHandle = NULL;
        memset(&_WaitOverlapped, 0, sizeof(_WaitOverlapped));
        _WaitOverlapped.hEvent = ::CreateEvent(NULL, true, false, NULL);
        assert(_WaitOverlapped.hEvent != INVALID_HANDLE_VALUE);
        _pReceiveCallBack = NULL;
    }
//����
    void UnInit()
    {
        if (_ReadOverlapped.hEvent != INVALID_HANDLE_VALUE)
        CloseHandle(_ReadOverlapped.hEvent);
        if (_WriteOverlapped.hEvent != INVALID_HANDLE_VALUE)
        CloseHandle(_WriteOverlapped.hEvent);
        if (_WaitOverlapped.hEvent != INVALID_HANDLE_VALUE)
        CloseHandle(_WaitOverlapped.hEvent);
    }
//�󶨴���
    void BindCommPort(DWORD dwPort)
    {
        assert(dwPort >= 1 && dwPort <= 1024);
        TCHAR p[5];
        _dwPort = dwPort;
        _tcscpy(_szCommStr, _T("\\\\.\\COM"));
        _ltot(_dwPort, p, 10);
        _tcscat(_szCommStr, p);
    }
//�򿪴���
    virtual bool OpenCommPort()
    {
        if (IsOpen())
        Close();
        _hCommHandle = ::CreateFile((_szCommStr), GENERIC_READ | GENERIC_WRITE, 0, NULL,OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | _dwIOMode,NULL);
        if (_fAutoBeginThread)
        {
            if (IsOpen() && BeginThread())
            return true;
            else
            {
            Close(); //�����߳�ʧ��
            return false;
            }
        }
        return IsOpen();
    }
//���ô���
    virtual bool SetupPort()
    {
        if (!IsOpen())
        return false;
        if (!::SetupComm(_hCommHandle, 4096, 4096))
        return false;
        if (!::GetCommTimeouts(_hCommHandle, &_CO))
        return false;
        _CO.ReadIntervalTimeout = 0;
        _CO.ReadTotalTimeoutMultiplier = 1;
        _CO.ReadTotalTimeoutConstant = 1000;
        _CO.WriteTotalTimeoutMultiplier = 1;
        _CO.WriteTotalTimeoutConstant = 1000;
        if (!::SetCommTimeouts(_hCommHandle, &_CO))
        return false;
        if (!::PurgeComm(_hCommHandle, PURGE_TXABORT | PURGE_RXABORT |PURGE_TXCLEAR | PURGE_RXCLEAR))
        return false;
        return true;
    }
//---------------------------------------threads callback-----------------------------------
//�߳��յ���Ϣ�Զ�����, �細�ھ����Ч, �ͳ���Ϣ, �������ڱ�ţ� ��Ϊ�麯�������ڻ���������չ
    virtual void OnReceive() //EV_RXCHAR
    {
        if (_pReceiveCallBack) {
            _pReceiveCallBack->OnReceive();
        }
        
        if (::IsWindow(_hNotifyWnd)) {
            CString m_comrecestr;
            char str[100];
            ReadString(str, 100, 100); // �������ش���������str
            ::PostMessage(_hNotifyWnd, ON_COM_RECEIVE, WPARAM(_dwPort),  (LPARAM)str);
        }
        
        //::PostMessage(_hNotifyWnd, ON_COM_RECEIVE, WPARAM(_dwPort), LPARAM (0));
    }
    virtual void OnDSR()
    {
        if (::IsWindow(_hNotifyWnd))
        {
            DWORD Status;
            if (GetCommModemStatus(_hCommHandle, &Status))
            ::PostMessage(_hNotifyWnd, ON_COM_DSR, WPARAM(_dwPort),LPARAM((Status &MS_DSR_ON) ? 1 : 0));
        }
    }
    virtual void OnCTS()
    {
        if (::IsWindow(_hNotifyWnd))
        {
            DWORD Status;
            if (GetCommModemStatus(_hCommHandle, &Status))
            ::PostMessage(_hNotifyWnd, ON_COM_CTS, WPARAM(_dwPort), LPARAM( (Status &MS_CTS_ON) ? 1 : 0));
        }
    }
    virtual void OnBreak()
    {
        if (::IsWindow(_hNotifyWnd))
        {
            ::PostMessage(_hNotifyWnd, ON_COM_BREAK, WPARAM(_dwPort), LPARAM(0));
        }
    }
    virtual void OnTXEmpty()
    {
        if (::IsWindow(_hNotifyWnd))
        ::PostMessage(_hNotifyWnd, ON_COM_TXEMPTY, WPARAM(_dwPort), LPARAM (0));
    }
    virtual void OnError()
    {
        DWORD dwError;
        ::ClearCommError(_hCommHandle, &dwError, NULL);
        if (::IsWindow(_hNotifyWnd))
        ::PostMessage(_hNotifyWnd, ON_COM_ERROR, WPARAM(_dwPort), LPARAM (dwError));
    }
    virtual void OnRing()
    {
        if (::IsWindow(_hNotifyWnd))
        ::PostMessage(_hNotifyWnd, ON_COM_RING, WPARAM(_dwPort), LPARAM(0));
    }
    virtual void OnRLSD()
    {
        if (::IsWindow(_hNotifyWnd))
        ::PostMessage(_hNotifyWnd, ON_COM_RLSD, WPARAM(_dwPort), LPARAM(0));
    }
    virtual DWORD ThreadFunc()
    {
        if (!::SetCommMask(_hCommHandle, _dwMaskEvent))
        {
            TCHAR szBuffer[256];
            _sntprintf(szBuffer, 255, _T("%s(%d) : COM%d Call WINAPI SetCommMask(%x, %x) Fail, thread work invalid! GetLastError() = %d;"), __FILE__, __LINE__, _dwPort, _hCommHandle, _dwMaskEvent, GetLastError());
            MessageBox(NULL, (szBuffer), (_T("Class cnComm")), MB_OK);
            return 1;
        }
        COMSTAT Stat;
        DWORD dwError;
        for (DWORD dwLength, dwMask = 0; _fRunFlag && IsOpen(); dwMask = 0)
        {
        if (!::WaitCommEvent(_hCommHandle, &dwMask, &_WaitOverlapped))
        {
        if (::GetLastError() == ERROR_IO_PENDING)
        // asynchronous
        ::GetOverlappedResult(_hCommHandle, &_WaitOverlapped,&dwLength, TRUE);
        else
        continue;
        }
        if (dwMask == 0)
        continue;
        switch (dwMask)
        {
            case EV_RXCHAR:
            ::ClearCommError(_hCommHandle, &dwError, &Stat);
            if (Stat.cbInQue >= _dwNotifyNum)
            OnReceive();
            break;
            case EV_TXEMPTY:
            OnTXEmpty();
            break;
            case EV_CTS:
            OnCTS();
            break;
            case EV_DSR:
            OnDSR();
            break;
            case EV_RING:
            OnRing();
            break;
            case EV_RLSD:
            OnRLSD();
            break;
            case EV_BREAK:
            OnBreak();
            break;
            case EV_ERR:
            OnError();
            break;
        } //case
        } //for
        return 0;
    }
private:
//the function private
cnComm(const cnComm &);
cnComm &operator = (const cnComm &);
//base function for thread
static DWORD WINAPI CommThreadProc(LPVOID lpPara)
{
return ((cnComm*)lpPara)->ThreadFunc();
}
};
#endif //_CN_COMM_H_