#ifndef TCPSTREAM_H
#define TCPSTREAM_H

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

class TcpStream
{
private:
    TcpStream(int, struct sockaddr_in*);
    TcpStream(const TcpStream &);

private:
    int m_sd;
    int m_peerPort;
    std::string m_peerIp;

public:
    friend class TcpAcceptor;
    friend class TcpConnector;

    ~TcpStream();

    size_t receiveFrom(char *buffer, size_t len);
    size_t sendTo(char *buffer, size_t len);
    void closeSocket();
    void disconnected();

    std::string peerIp();
    int peerPort();
};

#endif // TCPSTREAM_H
