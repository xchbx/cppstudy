#include "h264bytestreamparser.h"
#include "InputFile.hh"
#include "h264nalpacket.h"
//#include <stdint-gcc.h>


H264ByteStreamParser::H264ByteStreamParser()
{

}

H264ByteStreamParser::H264ByteStreamParser(char* filename)
    :filePath(filename)
{
    h264file = NULL;
    h264file = OpenInputFile(filePath);
    if(!h264file)
        fprintf(stderr, "H264ByteStreamParser: Error in Opening file\n");
}

H264ByteStreamParser::~H264ByteStreamParser()
{
    CloseInputFile(h264file);
}

H264NalPacket* H264ByteStreamParser::next_nalu()
{
    uint8_t buff[H264NalPacket::MAX_NAL_BUF_SIZE];
    char tmpbuf[4];
    char tmpbuf2[1];
    int flag = 0;
    int ret;
    int len = 0;

    do {
        ret = fread(tmpbuf2, 1, 1, h264file);
        if (!ret) {
            return NULL;
        }
        if (!flag && tmpbuf2[0] != 0x0) {
            buff[len] = tmpbuf2[0];
            (len)++;
        } else if (!flag && tmpbuf2[0] == 0x0) {
            flag = 1;
            tmpbuf[0] = tmpbuf2[0];
        } else if (flag) {
            switch (flag) {
            case 1:
                if (tmpbuf2[0] == 0x0) {
                    flag++;
                    tmpbuf[1] = tmpbuf2[0];
                } else {
                    flag = 0;
                    buff[len] = tmpbuf[0];
                    (len)++;
                    buff[len] = tmpbuf2[0];
                    (len)++;
                }
                break;
            case 2:
                if (tmpbuf2[0] == 0x0) {
                    flag++;
                    tmpbuf[2] = tmpbuf2[0];
                } else if (tmpbuf2[0] == 0x1) {
                    flag = 0;
                    if(len > 0)
                        return new H264NalPacket(buff, len);
                } else {
                    flag = 0;
                    buff[len] = tmpbuf[0];
                    (len)++;
                    buff[len] = tmpbuf[1];
                    (len)++;
                    buff[len] = tmpbuf2[0];
                    (len)++;
                }
                break;
            case 3:
                if (tmpbuf2[0] == 0x1) {
                    flag = 0;
                    if(len > 0)
                        return new H264NalPacket(buff, len);
                } else {
                    flag = 0;
                    break;
                }
            }
        }
    } while (1);
}

void H264ByteStreamParser::set_file_path(char *path)
{
    filePath = path;


}
