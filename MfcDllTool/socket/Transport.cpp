#include "stdafx.h"
#include "Transport.h"
#include "SocketData.h"

#ifndef _UNISTD_H
#define _UNISTD_H
#include <io.h>
#include <process.h>
#endif /* _UNISTD_H */

#define Socket      pSocketData->Socket()
#define Connected   pSocketData->Connected()

using namespace std;

// https://blog.csdn.net/youxiazzz12/article/details/25634143
// https://blog.csdn.net/caowei880123/article/details/8266252

Transport::Transport()
{   
    pSocketData = new SocketData;
    if (!pSocketData) {
        throw "SocketData Create fail";
    }

    WORD wVersionRequested;
    WSADATA wsaData;
    int err;
    wVersionRequested = MAKEWORD(2, 2);
    err = WSAStartup(wVersionRequested, &wsaData);
    if (err != 0) {
        printf("WSAStartup failed with error: %d\n", err);
    }

    if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) {
        printf("Could not find a usable version of Winsock.dll\n");
        WSACleanup();
    }
    else {
        printf("The Winsock 2.2 dll was found okay\n");
    }
}

Transport::~Transport()
{
    if (pSocketData) {
        delete pSocketData;
        pSocketData = NULL;
    }

    WSACleanup();
}

void Transport::SetRemote(String ip, uint16_t port)
{
    pSocketData->SetIp(ip);
    pSocketData->SetPort(port);
}

void Transport::Close()
{
    if (Connected) {
        pSocketData->SetConnected(false);
        closesocket(Socket);
        pSocketData->SetSocket(INVALID_SOCKET);
    }
}

int Transport::Send(const uint8_t command[], int cmdLen)
{
    if (!Connected) {
        SetErrorCode(GetLastError());
        throw "Send Fail: Socket is not connected.";
    }

    // sends the command
    //
    int bytesSent = send(Socket, (char*)command, cmdLen, 0);

    // it checks the number of bytes sent
    //
    if (bytesSent != cmdLen) {
        SetErrorCode(GetLastError());
        string msg = "Sending error. (Expected bytes:";
        msg += to_string((_Longlong)cmdLen);
        msg += " Sent: ";
        msg += to_string((_Longlong)bytesSent);
        msg += ")";

        RecordErrorCode();
        throw msg.c_str();
    }

    return bytesSent;
}

int Transport::Receive(uint8_t response[], int respLen)
{
    if (!Connected) {
        throw "Socket is not connected.";
    }

    // receives the response, this is a synchronous method and can hang the process
    //
    int bytesRecv = recv(Socket, (char*)response, respLen, 0);

    //
    if (bytesRecv < 0) {
        SetErrorCode(GetLastError());
        throw "Socket Recv timeout.";
    }
    //else if ()
    //{
    //}
    // check the number of bytes received
    //
    if (bytesRecv != respLen) {
        SetErrorCode(GetLastError());
        string msg = "Receiving error. (Expected:";
        msg += to_string((_Longlong)respLen);
        msg += " Received: ";
        msg += to_string((_Longlong)bytesRecv);
        msg += ")";

        RecordErrorCode();
        throw msg.c_str();
    }

    return bytesRecv;
}

int Transport::RecordErrorCode()
{
    // WSAEINTR(10004)      : 函数调用中断
    // WSAENOTSOCK(10038)   : 无效套接字
    // WSAECONNRESET(10054) : 连接被远程主机强行关闭
    // WSAESHUTDOWN(10058)  : 套接字关闭后不能收发
    // WSAETIMEDOUT(10060)  : 超时
    int errorcode = pSocketData->Errorcode();
    if (errorcode != 0) {
        printf("[%d] errorcode = %d\n", GetCurrentThreadId(), errorcode); // 0 : 连接关闭

        TCHAR szLogPath[MAX_PATH] = { 0 };
        TCHAR szLogFolder[MAX_PATH] = { 0 };
        TCHAR szEXEFolder[MAX_PATH] = { 0 };
        GetModuleFileName(NULL, szEXEFolder, MAX_PATH);
        PathRemoveFileSpec(szEXEFolder);
        wsprintf(szLogFolder, _T("%s\\log"), szEXEFolder);
        CreateDirectory(szLogFolder, NULL);
        wsprintf(szLogPath, _T("%s\\udp.txt"), szLogFolder);
        FILE *f = _tfopen(szLogPath, _T("a+"));
        _ftprintf (f, _T("[%d] errorcode = %d\n"), GetCurrentThreadId(), errorcode);
        fclose (f);
    }

    return errorcode;
}

int Transport::ErrorCode()
{
    int errorcode = pSocketData ? pSocketData->Errorcode() : 0;
    return errorcode;
}

void Transport::SetErrorCode(int errorcode)
{
    if (pSocketData) {
        pSocketData->SetErrorcode(errorcode);
    }
}