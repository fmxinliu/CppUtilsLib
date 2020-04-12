#pragma once

#include <cstdint>
#include "dllexport.h"
#include "FinsCmdEnum.h"

#define DEFAULT_PORT 9600

namespace OmronPlc
{
    class IFinsCommand;
    class Fins
    {
    public:
        Fins(TransportType TType = TransportType::Tcp);
        virtual ~Fins();

        bool PLCConnect();
        void PLCClose();
        void SetRemote(String ipaddr, uint16_t port=DEFAULT_PORT);

        bool MemoryAreaRead(MemoryArea area, uint16_t address, uint8_t bit_position, uint16_t count);
        bool MemoryAreaWrite(MemoryArea area, uint16_t address, uint8_t bit_position, uint16_t count, uint8_t data[]);

        uint32_t ReadDM(uint16_t address);
        bool ReadDM(uint16_t address, uint32_t &value);
        bool ReadDM(uint16_t address, uint16_t &value);
        bool ReadDM(uint16_t address, int16_t &value);
        bool ReadDM(uint16_t address, uint8_t data[], uint16_t count);
        bool ReadDM(uint16_t address, uint16_t data[], uint16_t count);
        bool WriteDM(uint16_t address, const uint16_t value);
        bool WriteDM(uint16_t address, uint8_t data[], uint16_t count); // Ð´×Ö·û´®
        bool ClearDM(uint16_t address, uint16_t count);
        
        bool ReadCIOBit(uint16_t address, uint8_t bit_position, bool &value);
        bool WriteCIOBit(uint16_t address, uint8_t bit_position, const bool value);

    private:
        IFinsCommand *_finsCmd;
    };
}
