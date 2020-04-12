#pragma once

#include "Transport.h"

class DLL_API TcpTransport : public Transport
{
public:
    TcpTransport() {}
    virtual ~TcpTransport() {}
    virtual bool Connect();
};
