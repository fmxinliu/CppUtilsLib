#include "stdafx.h"
#include "serial.h"
#include "cnComm.h"

Serial::Serial(bool fAutoBeginThread, DWORD dwIOMode)
{
    pcnComm = new cnComm(fAutoBeginThread, dwIOMode);
    if (!pcnComm) {
        throw "Serial Create fail";
    }

    pcnComm->SetCallBack(this);
}

Serial::~Serial()
{
    if (pcnComm) {
        delete pcnComm;
        pcnComm = NULL;
    }
}

bool Serial::IsOpen()
{
    return pcnComm->IsOpen();
}

bool Serial::Open(DWORD dwPort, TCHAR *szSetStr)
{
    return pcnComm->Open(dwPort, szSetStr);
}

void Serial::Close()
{
    pcnComm->Close();
}

DWORD Serial::Read(LPVOID Buffer, DWORD dwBufferLength, DWORD dwWaitTime)
{
    return pcnComm->Read(Buffer, dwBufferLength, dwWaitTime);
}

char* Serial::ReadString(char *szBuffer, DWORD dwBufferLength, DWORD dwWaitTime)
{
    return pcnComm->ReadString(szBuffer, dwBufferLength, dwWaitTime);
}

DWORD Serial::Write(LPVOID Buffer, DWORD dwBufferLength)
{
    return pcnComm->Write(Buffer, dwBufferLength);
}

DWORD Serial::Write(const char *szBuffer)
{
    return pcnComm->Write(szBuffer);
}

DWORD Serial::Write(char *szBuffer, DWORD dwBufferLength, char *szFormat, ...)
{
    va_list va;
    va_start(va, szFormat);
    _vsnprintf(szBuffer, dwBufferLength, szFormat, va);
    va_end(va);
    return pcnComm->Write(szBuffer);
}

DWORD Serial::Write(char *szBuffer, char *szFormat, ...)
{
    va_list va;
    va_start(va, szFormat);
    vsprintf(szBuffer, szFormat, va);
    va_end(va);
    return pcnComm->Write(szBuffer);
}
