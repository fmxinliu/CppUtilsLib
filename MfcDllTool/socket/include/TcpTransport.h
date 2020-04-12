#pragma once

#include "Transport.h"

class TcpTransport : public Transport
{
public:
    TcpTransport() {}
    virtual ~TcpTransport() {}
    virtual bool Connect();
};
