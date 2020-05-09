#ifndef H264NALPACKET_H
#define H264NALPACKET_H

//#include <stdint-gcc.h>
#include <stdint.h>

class H264NalPacket
{
public:
    H264NalPacket(uint8_t *data, int len);
    ~H264NalPacket();

    static const int MAX_NAL_BUF_SIZE = (1500*100);

    int     NalSize();
    void    print_nal_unit_type();
    void    init();
    bool    f();
    int     nri();
    int     type();
    uint8_t *rbsp();

private:
    int NumBytesinNALUnit;
    bool forbiden_zero_bit;
    int nal_ref_idc;
    int nal_unit_type;   //vcl nal unit: nal_unit_type equal to 1-5
    uint8_t *RBSP;

};

#endif // H264NALPACKET_H
