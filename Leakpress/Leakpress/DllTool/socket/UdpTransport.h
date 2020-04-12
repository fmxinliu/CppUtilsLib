#pragma once

#include "Transport.h"

class DLL_API UdpTransport : public Transport
{
public:
    UdpTransport() {}
    virtual ~UdpTransport() {}
    virtual bool Connect();
};
