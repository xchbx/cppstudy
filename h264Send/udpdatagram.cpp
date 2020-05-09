#include "udpdatagram.h"

UdpDatagram::UdpDatagram()
{
    m_binding = false;
    m_peerIP = "";
    m_peerPort = 0;

    //create a Datagram socket for reading and writing Datagram
    m_sd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

}

UdpDatagram::~UdpDatagram()
{
    if(m_sd > 0)
        closesocket(m_sd);
}

int UdpDatagram::readDatagram(char *buffer, int len)
{
    //receiveing...
    int ByteReceived;
    sockaddr_in SenderAddr;
    int SenderAddrSize = sizeof(SenderAddr);

    ByteReceived = recvfrom(m_sd, buffer, len,
                            0, (SOCKADDR *)&SenderAddr, &SenderAddrSize);

    m_peerIP = inet_ntoa(SenderAddr.sin_addr);
    m_peerPort = ntohs(SenderAddr.sin_port);

    return ByteReceived;
}

int UdpDatagram::writeDatagram(const char *buffer, int len, char *addr, uint16_t port)
{
    int byteSent;
    sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    address.sin_addr.s_addr = inet_addr(addr);

    byteSent = sendto(m_sd, buffer, len, 0, (SOCKADDR*)&address, sizeof(address));

    return byteSent;
}

//bool UdpDatagram::bindTo(const char *addr, int port)
//{
//    if(m_binding)
//        return true;

//    //setting bind address...
//    struct sockaddr_in address;
//    address.sin_family = AF_INET;
//    address.sin_port = htons(port);
//    if(strcmp(addr, "") != 0)
//        address.sin_addr.s_addr = inet_addr(addr);
//    else
//        address.sin_addr.s_addr = htonl(INADDR_ANY);


//    //binding to an address
//    if(bind(m_sd, (SOCKADDR*)&address, sizeof(address)) == SOCKET_ERROR)
//        return false;
//    printf("Server: i'am waiting for new Datagram...\n");
//    return true;

//}

bool UdpDatagram::wait(long sec)
{
    int selectTiming = recvTimeoutUdp(sec);
    switch (selectTiming) {
    case 0:
        printf("Server: NO Connection in timeout\n");
        return false;
        break;
    case -1:
        printf("Server: Error in  Listening\n");
        return false;
        break;
    default:
        return true;
        break;
    }
}

std::string UdpDatagram::peerIP()
{
    return m_peerIP;
}

int UdpDatagram::peerPort()
{
    return m_peerPort;
}

void UdpDatagram::close()
{
    if(m_sd > 0)
        closesocket(m_sd);
}

int UdpDatagram::recvTimeoutUdp(long sec)
{
    struct timeval timeout;
    timeout.tv_sec = sec;

    struct fd_set fds;

    FD_ZERO(&fds);
    FD_SET(m_sd, &fds);

    return select(0, &fds, 0, 0, &timeout);

}

int UdpDatagram::socketDescriptor()
{
    return m_sd;
}

