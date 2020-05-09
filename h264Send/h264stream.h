#ifndef H264STREAM_H
#define H264STREAM_H

#include "h264bytestreamparser.h"
#include "h264rtppacketizer.h"

class H264Stream
{
public:
    H264Stream(char *);
    ~H264Stream();

    void stream();
    void setDestinationAddress(char *addr, uint16_t port);
    void setClockFrequency(int clock);
    uint16_t destinationPort();
    char *destinationAddr();
    char *sessionDescription();
private:
    H264RtpPacketizer packetizer;
    H264ByteStreamParser parser;

};

#endif // H264STREAM_H
