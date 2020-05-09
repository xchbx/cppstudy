#ifndef TCPCONNECTOR_H
#define TCPCONNECTOR_H

#include <WinSock2.h>
class TcpStream;
class TcpConnector
{
public:
    TcpStream *connectToHost(int port, const char* host);

private:
    int resolveHost(const char* hostName, struct in_addr* addr);
};

#endif // TCPCONNECTOR_H
