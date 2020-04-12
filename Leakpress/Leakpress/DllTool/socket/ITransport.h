#pragma once

#include <stdint.h>
#include "dllexport.h"

class DLL_API ITransport
{
public:
    virtual ~ITransport() {};
    virtual bool Connect() = 0;
    virtual void Close() = 0;
    virtual void SetRemote(String ip, uint16_t port) = 0;
    virtual int Send(const uint8_t command[], int cmdLen) = 0;
    virtual int Receive(uint8_t response[], int respLen) = 0;
    virtual int ErrorCode() = 0;
};