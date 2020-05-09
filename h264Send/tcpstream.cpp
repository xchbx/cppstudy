#include "tcpstream.h"

TcpStream::TcpStream(int sd, struct sockaddr_in* addr)
    :m_sd(sd)
{
    char *ip = inet_ntoa(addr->sin_addr);

    m_peerPort = ntohs(addr->sin_port);
    m_peerIp = ip;
}

TcpStream::TcpStream(const TcpStream &stream)
{
    *this = stream;
}

TcpStream::~TcpStream()
{
    closesocket(m_sd);
}

size_t TcpStream::receiveFrom(char *buffer, size_t len)
{
    //receiveing...
    return recv(m_sd, buffer, len, 0);
}

size_t TcpStream::sendTo(char *buffer, size_t len)
{
    //sending...
    return send(m_sd, buffer, len, 0);
}

void TcpStream::close()
{
    //unsafe Closing...
    if(m_sd > 0)
        closesocket(m_sd);
}

void TcpStream::disconnecte()
{
    //safe Closing...
    if(m_sd > 0)
        shutdown(m_sd, SD_BOTH);
}

std::string TcpStream::peerIp()
{
    return m_peerIp;
}

int TcpStream::peerPort()
{
    return m_peerPort;
}

