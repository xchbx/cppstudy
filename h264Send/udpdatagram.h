#ifndef UDPDATAGRAM_H
#define UDPDATAGRAM_H

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

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
    void closeSocket();

private:
    int         m_sd;
    bool        m_binding;
    std::string m_peerIP;
    int       m_peerPort;

    int recvTimeoutUdp(long sec = 0);

};

#endif // UDPDATAGRAM_H
