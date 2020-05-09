#ifndef TCPACCEPTOR_H
#define TCPACCEPTOR_H

#include <string>
#include <WinSock2.h>
class TcpStream;
class TcpAcceptor
{
public:
    TcpAcceptor(int port, const char* address = "");
    ~TcpAcceptor();
private:
    int m_lsd;
    std::string m_address;
    int m_port;
    bool m_listening;
public:
    TcpStream* acceptConnection();
    bool waitForNewConnection(long sec);
    int start();
private:
    int recvTimeoutTcp(long sec);
};

#endif // TCPACCEPTOR_H
