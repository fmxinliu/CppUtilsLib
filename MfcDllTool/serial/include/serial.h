#pragma once

#include "dllexport.h"
#include "icomcallback.h"

class cnComm;
class DLL_API Serial : public IComCallBack
{
public:
    // ��1������Ϊ�Ƿ��ڴ򿪴���ʱ���������̣߳� ��2������ΪIO��ʽ ������ʽ��0��/ �첽�ص���ʽ��Ĭ�ϣ�
    Serial(bool fAutoBeginThread = true, DWORD dwIOMode = FILE_FLAG_OVERLAPPED);
    virtual ~Serial();

    // �߳��յ���Ϣ�Զ��ص� 
    virtual void OnReceive() {}

    // �����Ƿ��
    bool IsOpen();

    // �򿪴���, ʹ������"9600, 8, n, 1"�������ַ������ô���
    bool Open(DWORD dwPort, TCHAR *szSetStr);

    // �رմ��� 
    void Close();

    // ��ȡ���� dwBufferLength���ַ��� Buffer ����ʵ�ʶ������ַ��� �ɶ���������
    DWORD Read(LPVOID Buffer, DWORD dwBufferLength, DWORD dwWaitTime = 10);

    // ��ȡ���� dwBufferLength - 1 ���ַ��� szBuffer ����ANSI C ģʽ�ַ���ָ�� �ʺ�һ���ַ�ͨѶ
    char *ReadString(char *szBuffer, DWORD dwBufferLength, DWORD dwWaitTime = 100);

    // д���� ��д�������� "abcd" or "\x0\x1\x2"
    DWORD Write(LPVOID Buffer, DWORD dwBufferLength);

    // д���� дANSI C ģʽ�ַ���ָ�� 
    DWORD Write(const char *szBuffer);

    // д���� szBuffer ���������ʽ�ַ��� ��������������
    DWORD Write(char *szBuffer, DWORD dwBufferLength, char *szFormat, ...);

    // д���� szBuffer ���������ʽ�ַ��� ����黺�������� С�����
    DWORD Write(char *szBuffer, char *szFormat, ...);

private:
    cnComm *pcnComm;
};