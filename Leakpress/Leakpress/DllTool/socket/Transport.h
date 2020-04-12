#pragma once

#include "ITransport.h"

class SocketData;
class DLL_API Transport : public ITransport
{
public:
    Transport();
    virtual ~Transport();
    virtual void SetRemote(String ip, uint16_t port);
    virtual bool Connect() = 0;
    virtual void Close();
    virtual int Send(const uint8_t command[], int cmdLen);
    virtual int Receive(uint8_t response[], int respLen);
    virtual int RecordErrorCode();
    virtual int ErrorCode();
    void SetErrorCode(int errorcode);

protected:
    SocketData *pSocketData;
};
