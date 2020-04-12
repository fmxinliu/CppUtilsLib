#pragma once

#include "dllexport.h"

#include <stdint.h>
#include <WinSock.h>
#pragma comment(lib, "ws2_32")

class DLL_API SocketData
{
public:
    SocketData() : _errorcode(0), _connected(false) {}
    int ErrorCode() const { return this->_errorcode; }

    String Ip() const { return _ip; }
    void SetIp(String val) { _ip = val; }

    uint16_t Port() const { return _port; }
    void SetPort(uint16_t val) { _port = val; }

    int Errorcode() const { return _errorcode; }
    void SetErrorcode(int val) { _errorcode = val; }

    bool Connected() const { return _connected; }
    void SetConnected(bool val) { _connected = val; }

    SOCKET Socket() const { return _socket; }
    void SetSocket(SOCKET val) { _socket = val; }

protected:
    SOCKET _socket;
    uint16_t _port;
    String _ip;
    int _errorcode;
    bool _connected;
};
