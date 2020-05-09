#ifndef UDPDATAGRAM_H
#define UDPDATAGRAM_H

#include <string>
#include "stdint.h"
class UdpDatagram
{
public:
    UdpDatagram();
    ~UdpDatagram();

    int readDatagram(char* buffer, int len);
    int writeDatagram(const char* buffer, int len, char* addr, uint16_t port);
    int socketDescriptor();
    bool wait(long sec = 0);
    std::string peerIP();
    int peerPort();
    void close();

private:
    int         m_sd;
    bool        m_binding;
    std::string m_peerIP;
    int       m_peerPort;

    int recvTimeoutUdp(long sec = 0);

};

#endif // UDPDATAGRAM_H
