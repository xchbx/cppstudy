#include <iostream>
#include <stdio.h>
#include "InputFile.hh"
#include "h264bytestreamparser.h"
#include "h264nalpacket.h"
#include "h264rtppacketizer.h"
#include "udpdatagram.h"
#include "h264stream.h"

using namespace std;

int main()
{
    H264ByteStreamParser parser("./test.264");

	H264RtpPacketizer p;
	p.set_clock_frequency(9000);
	p.set_destination_addr("239.0.0.1", 4000);
	p.start(parser);

    return 0;
}
