#ifndef H264BYTESTREAMPARSER_H
#define H264BYTESTREAMPARSER_H

#include <stdint.h>
#include <stdio.h>
#include "h264nalpacket.h"

class H264ByteStreamParser
{
public:
    H264ByteStreamParser();
    H264ByteStreamParser(char*);
    ~H264ByteStreamParser();

    H264NalPacket* next_nalu();
    void set_file_path(char *path);
private:
    FILE *h264file;
    char *filePath;
};

#endif // H264BYTESTREAMPARSER_H
