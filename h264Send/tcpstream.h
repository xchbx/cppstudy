#ifndef TCPSTREAM_H
#define TCPSTREAM_H

#include <WinSock2.h>
#include <string>

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
    void close();
    void disconnecte();

    std::string peerIp();
    int peerPort();
};

#endif // TCPSTREAM_H
