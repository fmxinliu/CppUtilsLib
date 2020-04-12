#pragma once

#include <cstdint>
#include "dllexport.h"
#include "FinsCmdEnum.h"

namespace OmronPlc
{
    class IFinsCommand
    {
    public:
        IFinsCommand() {}
        virtual ~IFinsCommand() {}
        virtual bool Connect() = 0;
        virtual void Close() = 0;
        virtual void SetRemote(String ipaddr, uint16_t port) = 0;
        virtual bool MemoryAreaRead(MemoryArea area, uint16_t address, uint8_t bit_position, uint16_t count) = 0;
        virtual bool MemoryAreaWrite(MemoryArea area, uint16_t address, uint8_t bit_position, uint16_t count, uint8_t data[]) = 0;
        virtual const uint8_t* Response() { return _response; }

    protected:
        uint8_t *_response;
    };
}
