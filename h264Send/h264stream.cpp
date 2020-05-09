#include "h264stream.h"
#include "InputFile.hh"
#include <stdlib.h>

H264Stream::H264Stream(char *filename)
    :parser(filename)
{

}

H264Stream::~H264Stream()
{

}

void H264Stream::stream()
{
    packetizer.start(parser);
}

void H264Stream::setDestinationAddress(char *addr, uint16_t port)
{
    packetizer.set_destination_addr(addr, port);
}

void H264Stream::setClockFrequency(int clock)
{
    packetizer.set_clock_frequency(clock);
}

uint16_t H264Stream::destinationPort()
{
    return packetizer.destPort();
}

char *H264Stream::destinationAddr()
{
    return packetizer.destAddr();
}

char *H264Stream::sessionDescription()
{
    FILE *h264Sdp = NULL;

    h264Sdp = fopen("H264SDP.sdp", "w");
    if(h264Sdp == NULL)
        fprintf(stderr, "H264Stream: Error in opening file\n");

    fprintf(h264Sdp,
            "m=video %d RTP/AVP 96\r\n"
            "a=rtpmap:96 H264/90000\r\n"
            "a=fmtp:96 packetization-mode=1\r\n", this->destinationPort());
    CloseInputFile(h264Sdp);
    return NULL;
}

