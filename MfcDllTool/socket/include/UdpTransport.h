#pragma once

#include "Transport.h"

class UdpTransport : public Transport
{
public:
    UdpTransport() {}
    virtual ~UdpTransport() {}
    virtual bool Connect();
};
