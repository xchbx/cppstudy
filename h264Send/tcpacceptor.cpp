#include "tcpacceptor.h"
#include "tcpstream.h"

TcpAcceptor::TcpAcceptor(int port, const char* address)
    :m_lsd(0), m_port(port), m_listening(false)
{
    m_address = address;
}

TcpAcceptor::~TcpAcceptor()
{
    if(m_lsd > 0)
        closesocket(m_lsd);
}

TcpStream *TcpAcceptor::acceptConnection()
{
    //accepting...
    if(m_listening == true)
        return NULL;
    struct sockaddr_in address;
    memset(&address, 0, sizeof(address));

    int len = sizeof(address);

    int sd = accept(m_lsd, (SOCKADDR*)&address, &len);
    if(sd < 0)
    {
        perror("Accept falied");
        return NULL;
    }
    printf("Connection was Establisehd!\n");
    return new TcpStream(sd, &address);
}

bool TcpAcceptor::waitForNewConnection(long sec)
{
    int selectTiming = recvTimeoutTcp(sec);
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

int TcpAcceptor::start()
{
    if(m_listening == true)
        return 0;
    m_lsd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    //setting address...
    struct sockaddr_in address;
    memset(&address, 0, sizeof(address));

    address.sin_family = AF_INET;
    address.sin_port = htons(m_port);
    if(m_address.size() > 0)
        address.sin_addr.s_addr = inet_addr(m_address.c_str());
    else
        address.sin_addr.s_addr = htonl(INADDR_ANY);

    int result = bind(m_lsd, (SOCKADDR*)&address, sizeof(address));
    if(result != 0)
    {
        perror("bind() failed");
        return result;
    }

    //listening...
    result = listen(m_lsd, 5);
    if(result != 0)
    {
        perror("listen() failed");
        return result;
    }
    printf("Server: Listening on: %ld %s\n", ntohs(address.sin_port)
           , inet_ntoa(address.sin_addr));
    return result;
}

int TcpAcceptor::recvTimeoutTcp(long sec)
{
    struct timeval timeout;
    timeout.tv_sec = sec;

    struct fd_set fds;

    FD_ZERO(&fds);
    FD_SET(m_lsd, &fds);

    return select(0, &fds, 0, 0, &timeout);
    //return 0 -1 >0
}
