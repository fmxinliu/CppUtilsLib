#include "stdafx.h"
#include "UdpTransport.h"
#include "SocketData.h"

#ifndef _UNISTD_H
#define _UNISTD_H
#include <io.h>
#include <process.h>
#endif /* _UNISTD_H */

#define Ip          pSocketData->Ip()
#define Port        pSocketData->Port()
#define Socket      pSocketData->Socket()
#define Connected   pSocketData->Connected()

bool UdpTransport::Connect()
{
    SOCKET _socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (_socket < 0) {
        SetErrorCode(WSAGetLastError());
        return false;
    }

    pSocketData->SetSocket(_socket);
    //setsockopt(_socket, SOL_SOCKET, SO_RCVTIMEO, &ReceiveTimeout, sizeof(ReceiveTimeout));

    struct sockaddr_in _serveraddr;
    _serveraddr.sin_family = AF_INET;

#ifdef UNICODE
    char cip[30] = { 0 };
    wcstombs(cip, Ip.c_str(), Ip.length() * 2);
    _serveraddr.sin_addr.s_addr = inet_addr(cip); // 将宽字符转换成多字符
#else
    _serveraddr.sin_addr.s_addr = inet_addr(Ip.c_str());
#endif

    _serveraddr.sin_port = htons(Port);

    unsigned long iMode = 1; // 为0时阻塞, 非0时不阻塞
    //if (ioctlsocket(pSocketData->_socket, FIONBIO, &iMode) < 0) {
    //    errorcode = WSAGetLastError();
    //    return false;
    //}

    int timeout = 3000; // 3s
    if (setsockopt(Socket, SOL_SOCKET, SO_SNDTIMEO, (const char*)&timeout, sizeof(timeout)) < 0 || 
        setsockopt(Socket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout)) < 0) {
            SetErrorCode(WSAGetLastError());
            return false;
    }

    if (connect(_socket, (struct sockaddr *)&_serveraddr, sizeof(_serveraddr)) < 0) {
        SetErrorCode(WSAGetLastError());
        return false;
    }

    pSocketData->SetConnected(true);
    return true;
}