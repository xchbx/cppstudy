#ifndef H264RTPPACKETIZER_H
#define H264RTPPACKETIZER_H

#include "udpdatagram.h"
#include "rtprandomrands.h"
#include "h264bytestreamparser.h"

class H264RtpPacketizer
{
public:
    H264RtpPacketizer();
    ~H264RtpPacketizer();

    static const int RTPHL = 12;
    static const int RTP_PAYLOAD_MAX_SIZE = 1400;
    static const int SEND_BUFF_SIZE = 1500;

    void start(H264ByteStreamParser parser);
    void set_ssrc(uint32_t ssrc);
    uint32_t ssrc();
    void set_clock_frequency(uint64_t clock);
    uint64_t clockFrequency();
    void send(uint8_t *sendbuff, int send_len);
    void set_destination_addr(char *addr, uint16_t port);
    uint16_t destPort();
    char *destAddr();

private:
    RTPRandomRandS r;
    uint16_t m_seq_num;
    uint32_t m_ssrc;
    uint32_t m_timestamp;
    uint64_t m_clock;
    uint16_t m_destport;
    char *m_destaddr;


    UdpDatagram m_socket;


};

#endif // H264RTPPACKETIZER_H
