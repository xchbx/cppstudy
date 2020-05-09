#include "h264nalpacket.h"
#include <stdlib.h>
#include <stdio.h>
#include <cstring>

char const* nal_unit_type_description_h264[32] = {
    "Unspecified", //0
    "Coded slice of a non-IDR picture", //1
    "Coded slice data partition A", //2
    "Coded slice data partition B", //3
    "Coded slice data partition C", //4
    "Coded slice of an IDR picture", //5
    "Supplemental enhancement information (SEI)", //6
    "Sequence parameter set", //7
    "Picture parameter set", //8
    "Access unit delimiter", //9
    "End of sequence", //10
    "End of stream", //11
    "Filler data", //12
    "Sequence parameter set extension", //13
    "Prefix NAL unit", //14
    "Subset sequence parameter set", //15
    "Reserved", //16
    "Reserved", //17
    "Reserved", //18
    "Coded slice of an auxiliary coded picture without partitioning", //19
    "Coded slice extension", //20
    "Reserved", //21
    "Reserved", //22
    "Reserved", //23
    "Unspecified", //24
    "Unspecified", //25
    "Unspecified", //26
    "Unspecified", //27
    "Unspecified", //28
    "Unspecified", //29
    "Unspecified", //30
    "Unspecified" //31
};

H264NalPacket::H264NalPacket(uint8_t *data, int len)
{
    init();
    NumBytesinNALUnit = len;

    /*
      +---------------+
      |0|1|2|3|4|5|6|7|
      +-+-+-+-+-+-+-+-+
      |F|NRI|  Type   |
      +---------------+
 ((nalu_buf[0] & 0x80))                 bit0: f
                | (nalu_buf[0] & 0x60)  bit1~2: nri
                | (nalu_buf[0] & 0x1f); bit3~7: type
    */
    if(len > 0) {
        forbiden_zero_bit   = (data[0] & 0X80) >> 7 ? true : false;
        nal_ref_idc         = (data[0] & 0X60) >> 5;
        nal_unit_type       = (data[0] & 0X1F);

        print_nal_unit_type();

        //RBSP
        RBSP = (uint8_t *)malloc((NumBytesinNALUnit + 1) * sizeof(uint8_t));
        if(RBSP == NULL)
            fprintf(stderr,"H264NalPacket: Error: out of memory\n");
        memcpy(RBSP, &data[0], NumBytesinNALUnit);
        RBSP[NumBytesinNALUnit] = '\0';
    }
}

void H264NalPacket::init()
{
    NumBytesinNALUnit = 0;
    forbiden_zero_bit = false;
    nal_ref_idc = 0;
    nal_unit_type = 0;
    RBSP = NULL;

}

bool H264NalPacket::f()
{
    return forbiden_zero_bit;
}

int H264NalPacket::nri()
{
    return nal_ref_idc;
}

int H264NalPacket::type()
{
    return nal_unit_type;
}

H264NalPacket::~H264NalPacket()
{
    free(RBSP);
}

int H264NalPacket::NalSize()
{
    return NumBytesinNALUnit;
}

void H264NalPacket::print_nal_unit_type()
{
    fprintf(stderr, "Type:%s\t Size:%d", nal_unit_type_description_h264[nal_unit_type], NumBytesinNALUnit);
}

uint8_t *H264NalPacket::rbsp()
{
    return RBSP;
}
