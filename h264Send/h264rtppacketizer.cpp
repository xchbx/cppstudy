#include "h264rtppacketizer.h"
#include "h264nalpacket.h"
#include "h264types.h"

#define frameRate 25

H264RtpPacketizer::H264RtpPacketizer()
{
    uint32_t seed = r.PickSeed();
    srand(seed);

    m_seq_num   = r.GetRandom16();
    m_ssrc      = r.GetRandom32();
    m_timestamp = r.GetRandom32();

}

H264RtpPacketizer::~H264RtpPacketizer()
{

}

void H264RtpPacketizer::start(H264ByteStreamParser parser)
{
    uint8_t send_buff[H264RtpPacketizer::SEND_BUFF_SIZE];
    H264NalPacket *nal;
    nalu_header_t *nalu_hdr;
    rtp_header_t  *rtp_hdr;
    fu_indicator_t *fu_ind;
    fu_header_t *fu_hdr;
    uint8_t *nalu_buf;
    int nalu_len;
    size_t len_sendbuf;

    int fu_pack_num;
    int last_fu_pack_size;
    int fu_seq;
    int fNum = 0;

    while((nal = parser.next_nalu()) != NULL) {
        fNum++;
        nalu_buf = nal->rbsp();
        nalu_len = nal->NalSize();
        if(nalu_len < 1)
            continue;
        m_timestamp += m_clock/frameRate; /* 90000 / 25 = 3600 */
#ifdef _DEBUG
        fprintf(stderr, "\t #%d\n", fNum);
#endif

        if(nalu_len <= H264RtpPacketizer::RTP_PAYLOAD_MAX_SIZE)
        {
            //single nal unit
            memset(send_buff, 0, H264RtpPacketizer::SEND_BUFF_SIZE);

            /* Copied from http://tools.ietf.org/html/rfc3550#section-5

                   Schulzrinne, et al.         Standards Track                    [Page 12]
                   FFC 3550                          RTP                          July 2003
                 5. RTP Data Transfer Protocol

                 5.1 RTP Fixed Header Fields

                   The RTP header has the following format:

                    0                   1                   2                   3
                    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
                   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
                   |V=2|P|X|  CC   |M|     PT      |       sequence number         |
                   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
                   |                           timestamp                           |
                   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
                   |           synchronization source (SSRC) identifier            |
                   +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
                   |            contributing source (CSRC) identifiers             |
                   |                             ....                              |
                   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
                 */
            rtp_hdr = (rtp_header_t *)send_buff;
            rtp_hdr->csrc_len       = 0;
            rtp_hdr->extension      = 0;
            rtp_hdr->padding        = 0;
            rtp_hdr->version        = 2;
            rtp_hdr->marker         = 0;
            rtp_hdr->payload_type   = H264;
            rtp_hdr->seq_no         = htons(++m_seq_num);
            rtp_hdr->ssrc           = htonl(m_ssrc);
            rtp_hdr->timestamp      = htonl(m_timestamp);

            /*
                         * 2. rtp single nal unit
                         */
            nalu_hdr        = (nalu_header_t *)&send_buff[12];
            nalu_hdr->f     = (nalu_buf[0] & 0X80) >> 7;
            nalu_hdr->nri   = (nalu_buf[0] & 0x60) >> 5;
            nalu_hdr->type  = (nalu_buf[0] & 0x1f);

            /*
                         * 3. nal
                         */
            memcpy(send_buff + 13, nalu_buf + 1, nalu_len -1);

            len_sendbuf = sizeof(rtp_header_t) + nalu_len;
            this->send(send_buff, len_sendbuf);

            delete nal;
        }else {
            /* nalu_len > RTP_PAYLOAD_MAX_SIZE */
            /*.8.  Fragmentation Units (FUs) */
            //FU-A

            fu_pack_num =
                    nalu_len % H264RtpPacketizer::RTP_PAYLOAD_MAX_SIZE ?
                        (nalu_len / H264RtpPacketizer::RTP_PAYLOAD_MAX_SIZE + 1)
                      : nalu_len / H264RtpPacketizer::RTP_PAYLOAD_MAX_SIZE;
            last_fu_pack_size =
                    nalu_len % H264RtpPacketizer::RTP_PAYLOAD_MAX_SIZE ?
                        nalu_len % H264RtpPacketizer::RTP_PAYLOAD_MAX_SIZE
                      : H264RtpPacketizer::RTP_PAYLOAD_MAX_SIZE;
#ifdef _DEBUG
            fprintf(stderr,"fu_pack_num: %d, last_fu_pack_size: %d\n",
                    fu_pack_num, last_fu_pack_size);
#endif
            fu_seq = 0;
            for(fu_seq = 0; fu_seq < fu_pack_num; fu_seq ++) {
                memset(send_buff, 0, H264RtpPacketizer::SEND_BUFF_SIZE);

                if(fu_seq == 0) {  //start of a fragmented NAL unit

                    //rtp
                    rtp_hdr = (rtp_header_t *)send_buff;
                    rtp_hdr->csrc_len       = 0;
                    rtp_hdr->extension      = 0;
                    rtp_hdr->padding        = 0;
                    rtp_hdr->version        = 2;
                    rtp_hdr->marker         = 0;
                    rtp_hdr->payload_type   = H264;
                    rtp_hdr->seq_no         = htons(++m_seq_num);
                    rtp_hdr->ssrc           = htonl(m_ssrc);
                    rtp_hdr->timestamp      = htonl(m_timestamp);

                    //The FU indicator octet
                    fu_ind = (fu_indicator_t *)&send_buff[12];
                    nalu_hdr->f     = (nalu_buf[0] & 0X80) >> 7;
                    nalu_hdr->nri   = (nalu_buf[0] & 0x60) >> 5;
                    nalu_hdr->type  = 28;

                    //The FU header
                    fu_hdr = (fu_header_t *)&send_buff[13];
                    fu_hdr->s    = 1;
                    fu_hdr->e    = 0;
                    fu_hdr->r    = 0;
                    fu_hdr->type = (nalu_buf[0] & 0x1f);

                    memcpy(send_buff + 14, nalu_buf + 1, H264RtpPacketizer::RTP_PAYLOAD_MAX_SIZE -1);

                    len_sendbuf = 12 + 2 + (H264RtpPacketizer::RTP_PAYLOAD_MAX_SIZE -1);
                    this->send(send_buff, len_sendbuf);

                }else if(fu_seq < fu_pack_num -1) {

                    rtp_hdr = (rtp_header_t *)send_buff;
                    rtp_hdr->csrc_len       = 0;
                    rtp_hdr->extension      = 0;
                    rtp_hdr->padding        = 0;
                    rtp_hdr->version        = 2;
                    rtp_hdr->marker         = 0;
                    rtp_hdr->payload_type   = H264;
                    rtp_hdr->seq_no         = htons(++m_seq_num);
                    rtp_hdr->ssrc           = htonl(m_ssrc);
                    rtp_hdr->timestamp      = htonl(m_timestamp);

                    //The FU indicator octet
                    fu_ind = (fu_indicator_t *)&send_buff[12];
                    nalu_hdr->f     = (nalu_buf[0] & 0X80) >> 7;
                    nalu_hdr->nri   = (nalu_buf[0] & 0x60) >> 5;
                    nalu_hdr->type  = 28;

                    //The FU header
                    fu_hdr = (fu_header_t *)&send_buff[13];
                    fu_hdr->s    = 0;
                    fu_hdr->e    = 0;
                    fu_hdr->r    = 0;
                    fu_hdr->type = (nalu_buf[0] & 0x1f);

                    memcpy(send_buff + 14, nalu_buf + H264RtpPacketizer::RTP_PAYLOAD_MAX_SIZE * fu_seq
                           , H264RtpPacketizer::RTP_PAYLOAD_MAX_SIZE);

                    len_sendbuf = 12 + 2 + (H264RtpPacketizer::RTP_PAYLOAD_MAX_SIZE);
                    this->send(send_buff, len_sendbuf);
                }else {
                    rtp_hdr = (rtp_header_t *)send_buff;
                    rtp_hdr->csrc_len       = 0;
                    rtp_hdr->extension      = 0;
                    rtp_hdr->padding        = 0;
                    rtp_hdr->version        = 2;
                    rtp_hdr->marker         = 1;
                    rtp_hdr->payload_type   = H264;
                    rtp_hdr->seq_no         = htons(++m_seq_num);
                    rtp_hdr->ssrc           = htonl(m_ssrc);
                    rtp_hdr->timestamp      = htonl(m_timestamp);

                    //The FU indicator octet
                    fu_ind = (fu_indicator_t *)&send_buff[12];
                    nalu_hdr->f     = (nalu_buf[0] & 0X80) >> 7;
                    nalu_hdr->nri   = (nalu_buf[0] & 0x60) >> 5;
                    fu_ind->type = 28;

                    //The FU header
                    fu_hdr = (fu_header_t *)&send_buff[13];
                    fu_hdr->s    = 0;
                    fu_hdr->e    = 0;
                    fu_hdr->r    = 0;
                    fu_hdr->type = (nalu_buf[0] & 0x1f);

                    memcpy(send_buff + 14, nalu_buf + H264RtpPacketizer::RTP_PAYLOAD_MAX_SIZE * fu_seq
                           , last_fu_pack_size);

                    len_sendbuf = 12 + 2 + last_fu_pack_size;
                    this->send(send_buff, len_sendbuf);

                }
            }
            delete nal;
        }
    }
}

void H264RtpPacketizer::set_ssrc(uint32_t ssrc)
{
    m_ssrc = ssrc;
}

uint32_t H264RtpPacketizer::ssrc()
{
    return m_ssrc;
}

void H264RtpPacketizer::set_clock_frequency(uint64_t clock)
{
    m_clock = clock;
}

uint64_t H264RtpPacketizer::clockFrequency()
{
    return m_clock;
}

void H264RtpPacketizer::send(uint8_t *sendbuff, int send_len)
{
    int ret;
    ret = m_socket.writeDatagram((char *)sendbuff, send_len, m_destaddr, m_destport);
    printf("ret:%d\n", ret);
    Sleep(36);
    if(ret < 0)
        fprintf(stderr, "H264RtpPacketizer: Error in sending udp datagram!\n");
}

void H264RtpPacketizer::set_destination_addr(char *addr, uint16_t port)
{
    m_destaddr = addr;
    m_destport = port;
}

uint16_t H264RtpPacketizer::destPort()
{
    return m_destport;
}

char *H264RtpPacketizer::destAddr()
{
    return m_destaddr;
}
