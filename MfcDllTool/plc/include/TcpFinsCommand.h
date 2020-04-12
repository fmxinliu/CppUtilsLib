#pragma once

#include <vector>
#include <string>
#include "IFinsCommand.h"

class TcpTransport;
namespace OmronPlc
{
    class TcpFinsCommand : public IFinsCommand
    {
    public:
        TcpFinsCommand(uint8_t ServiceID = 0x01);
        ~TcpFinsCommand();
        virtual bool Connect();
        virtual void Close();
        virtual void SetRemote(String ipaddr, uint16_t port);
        virtual bool MemoryAreaRead(MemoryArea area, uint16_t address, uint8_t bit_position, uint16_t count);
        virtual bool MemoryAreaWrite(MemoryArea area, uint16_t address, uint8_t bit_position, uint16_t count, uint8_t data[]);

    private:
        bool NodeAddressDataSend();
        bool FrameSend(const std::vector<uint8_t> &data = std::vector<uint8_t>());

    private:
        int finsCommandLen;
        int finsResponseLen;
        std::vector<uint8_t> cmdFS;
        std::vector<uint8_t> respFS;
        std::vector<uint8_t> respFinsData;
        std::vector<uint8_t> cmdFins;
        std::vector<uint8_t> respFins;
        TcpTransport *pTransport;
        std::string lastError;
    };
}
