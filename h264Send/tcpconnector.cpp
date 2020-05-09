#include "tcpconnector.h"
#include "tcpstream.h"

TcpStream *TcpConnector::connectToHost(int port, const char *host)
{
    struct sockaddr_in address;

    //setting server address...
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    address.sin_addr.s_addr = inet_addr(host);

    //connecting...
    int sd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(connect(sd, (SOCKADDR*)&address, sizeof(address))!= 0)
        return NULL;
    return new TcpStream(sd, &address);
}
