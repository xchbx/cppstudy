#include "tcpstream.h"
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>


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
	closeSocket();
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

void TcpStream::closeSocket()
{
    //unsafe Closing...
    if(m_sd > 0)
        close(m_sd);
}

void TcpStream::disconnected()
{
    //safe Closing...
    if(m_sd > 0)
        shutdown(m_sd, SHUT_RDWR);
}

std::string TcpStream::peerIp()
{
    return m_peerIp;
}

int TcpStream::peerPort()
{
    return m_peerPort;
}

