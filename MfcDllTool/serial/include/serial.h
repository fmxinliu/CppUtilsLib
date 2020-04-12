#pragma once

#include "dllexport.h"
#include "icomcallback.h"

class cnComm;
class DLL_API Serial : public IComCallBack
{
public:
    // 第1个参数为是否在打开串口时启动监视线程， 第2个参数为IO方式 阻塞方式（0）/ 异步重叠方式（默认）
    Serial(bool fAutoBeginThread = true, DWORD dwIOMode = FILE_FLAG_OVERLAPPED);
    virtual ~Serial();

    // 线程收到消息自动回调 
    virtual void OnReceive() {}

    // 串口是否打开
    bool IsOpen();

    // 打开串口, 使用类似"9600, 8, n, 1"的设置字符串设置串口
    bool Open(DWORD dwPort, TCHAR *szSetStr);

    // 关闭串口 
    void Close();

    // 读取串口 dwBufferLength个字符到 Buffer 返回实际读到的字符数 可读任意数据
    DWORD Read(LPVOID Buffer, DWORD dwBufferLength, DWORD dwWaitTime = 10);

    // 读取串口 dwBufferLength - 1 个字符到 szBuffer 返回ANSI C 模式字符串指针 适合一般字符通讯
    char *ReadString(char *szBuffer, DWORD dwBufferLength, DWORD dwWaitTime = 100);

    // 写串口 可写任意数据 "abcd" or "\x0\x1\x2"
    DWORD Write(LPVOID Buffer, DWORD dwBufferLength);

    // 写串口 写ANSI C 模式字符串指针 
    DWORD Write(const char *szBuffer);

    // 写串口 szBuffer 可以输出格式字符串 包含缓冲区长度
    DWORD Write(char *szBuffer, DWORD dwBufferLength, char *szFormat, ...);

    // 写串口 szBuffer 可以输出格式字符串 不检查缓冲区长度 小心溢出
    DWORD Write(char *szBuffer, char *szFormat, ...);

private:
    cnComm *pcnComm;
};