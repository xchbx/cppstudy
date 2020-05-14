#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <net/if.h>
#include <netinet/in.h>
#include <pthread.h>
#include "h264.h"
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <sys/time.h>
#include <time.h>
#include <unistd.h>

#define  UDP_MAX_SIZE 1400
#define SEND_BUF_SIZE 1500
#define BUFFER_SIZE 200000
//#define SSRC_NUM 10

void print_time()
{
    struct timeval tv;
    struct tm* ptm;
    char time_string[40];
    long milliseconds;

    gettimeofday (&tv, NULL);
    ptm = localtime (&tv.tv_sec);
    strftime (time_string, sizeof (time_string), "%Y-%m-%d %H:%M:%S", ptm);
    milliseconds = tv.tv_usec / 1000;

    printf ("%s.%03ld\n", time_string, milliseconds);
}

typedef struct
{
    int startcodeprefix_len;      //@< 4 for parameter sets and first slice in picture, 3 for everything else (suggested)
    unsigned len;                 //@< Length of the NAL unit (Excluding the start code, which does not belong to the NALU)
    unsigned max_size;            //@< Nal Unit Buffer size
    int forbidden_bit;            //@< should be always FALSE
    int nal_reference_idc;        //@< NALU_PRIORITY_xxxx
    int nal_unit_type;            //@< NALU_TYPE_xxxx
    char *buf;                    //@< contains the first byte followed by the EBSP
    unsigned short lost_packets;  //@< true, if packet loss is detected
} NALU_t;

FILE *h264Bitsream = NULL;                //@< the bit stream file
static int FindStartCode2(unsigned char *Buf);//0x00 00 01
static int FindStartCode3(unsigned char *Buf);//0x00 00 00 01


static int info2=0, info3=0;
RTP_FIXED_HEADER *rtp_hdr;

NALU_HEADER     *nalu_hdr;
FU_INDICATOR    *fu_ind;
FU_HEADER       *fu_hdr;

NALU_t *AllocNALU(int buffersize)
{
    NALU_t *n;

    if ((n = (NALU_t*)calloc (1, sizeof (NALU_t))) == NULL)
    {
        printf("AllocNALU: n");
        exit(0);
    }

    n->max_size=buffersize;

    if ((n->buf = (char*)calloc (buffersize, sizeof (char))) == NULL)
    {
        free (n);
        printf ("AllocNALU: n->buf");
        exit(0);
    }

    return n;
}


void FreeNALU(NALU_t *n)
{
    if (n)
    {
        if (n->buf)
        {
            free(n->buf);
            n->buf=NULL;
        }
        free (n);
    }
}

void OpenBitstreamFile (const char *filename)
{
    if (NULL == (h264Bitsream=fopen(filename, "rb")))
    {
        printf("open file error\n");
        exit(0);
    }
}

int GetAnnexbNALU (NALU_t *nalu)
{
    int pos = 0;
    int StartCodeFound, rewind;
    unsigned char *Buf;

    if ((Buf = (unsigned char*)calloc (nalu->max_size , sizeof(char))) == NULL)
        printf ("GetAnnexbNALU: Could not allocate Buf memory\n");

    nalu->startcodeprefix_len=3;

    if (3 != fread (Buf, 1, 3, h264Bitsream))
    {
        free(Buf);
        return 0;
    }
    info2 = FindStartCode2 (Buf);
    if(info2 != 1)
    {
        if(1 != fread(Buf+3, 1, 1, h264Bitsream))
        {
            free(Buf);
            return 0;
        }
        info3 = FindStartCode3 (Buf);
        if (info3 != 1)
        {
            free(Buf);
            return -1;
        }
        else
        {
            pos = 4;
            nalu->startcodeprefix_len = 4;
        }
    }
    else
    {
        nalu->startcodeprefix_len = 3;
        pos = 3;
    }

    StartCodeFound = 0;
    info2 = 0;
    info3 = 0;

    while (!StartCodeFound)
    {
        if (feof (h264Bitsream))
        {
            nalu->len = pos-nalu->startcodeprefix_len;
            memcpy (nalu->buf, &Buf[nalu->startcodeprefix_len], nalu->len);
            nalu->forbidden_bit = nalu->buf[0] & 0x80; //1 bit
            nalu->nal_reference_idc = nalu->buf[0] & 0x60; // 2 bit
            nalu->nal_unit_type = nalu->buf[0] & 0x1f;// 5 bit
            free(Buf);
            return pos;
        }
        Buf[pos++] = fgetc (h264Bitsream);
        info3 = FindStartCode3(&Buf[pos-4]);
        if(info3 != 1)
            info2 = FindStartCode2(&Buf[pos-3]);
        StartCodeFound = (info2 == 1 || info3 == 1);
    }

    // Here, we have found another start code (and read length of startcode bytes more than we should
    // have.  Hence, go back in the file
    rewind = (info3 == 1)? -4 : -3;

    if (0 != fseek (h264Bitsream, rewind, SEEK_CUR))
    {
        free(Buf);
        printf("GetAnnexbNALU: Cannot fseek in the bit stream file");
    }

    // Here the Start code, the complete NALU, and the next start code is in the Buf.
    // The size of Buf is pos, pos+rewind are the number of bytes excluding the next
    // start code, and (pos+rewind)-startcodeprefix_len is the size of the NALU excluding the start code

    nalu->len = (pos+rewind)-nalu->startcodeprefix_len;
    memcpy (nalu->buf, &Buf[nalu->startcodeprefix_len], nalu->len);
    nalu->forbidden_bit = nalu->buf[0] & 0x80;        //1 bit
    nalu->nal_reference_idc = nalu->buf[0] & 0x60;    //2 bit
    nalu->nal_unit_type = nalu->buf[0] & 0x1f;  //5 bit
    free(Buf);

    return (pos+rewind);
}

static int FindStartCode2 (unsigned char *Buf)
{
    if(Buf[0]!=0 || Buf[1]!=0 || Buf[2] !=1) return 0;
    else return 1;
}

static int FindStartCode3 (unsigned char *Buf)
{
    if(Buf[0]!=0 || Buf[1]!=0 || Buf[2] !=0 || Buf[3] !=1) return 0;
    else return 1;
}

int rtpnum = 0;


void debug(NALU_t *nFrame)
{
    if (!n)return;
    printf("%3d, len: %6d  ",rtpnum++, nFrame->len);
    printf("nal_unit_type: %x\n", nFrame->nal_unit_type);
}
struct sockaddr_in serveraddr, clientaddr;
socklen_t addr_len;
int sockfd;
//int sin_size;

//usage: %s <H264_filename> [port]
int main(int argc, char* argv[])
{
    if(argc != 3){
        fprintf(stderr, "usage: %s <H264_filename> [port]\n", argv[0]);
        exit(0);
    }

    const char* raw_file = argv[1];
    unsigned short DEST_PORT = atoi(argv[2]);

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sockfd == -1)
    {
        perror("socket");
        exit(1);
    }

    bzero(&serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(DEST_PORT);

    addr_len = sizeof(struct sockaddr_in);

    if(bind(sockfd,(const struct sockaddr *)&serveraddr, sizeof(serveraddr)) < 0)
    {
        perror("bind");
        exit(1);
    }

    OpenBitstreamFile(raw_file);
    NALU_t *nFrame;
    nFrame = AllocNALU(BUFFER_SIZE);
    char* nalu_payload;
    char sendbuf[SEND_BUF_SIZE];
    unsigned short seq_num =0;
    int bytes=0;
    float framerate=25;
    unsigned int timestamp_increse = 3600;
    //timestamp_increse=(unsigned int)(90000.0 / framerate);
    unsigned int ts_current=0;
    unsigned int SSRC_NUM = 10;

    int req_len = 0;
    char req_buf[50];
    while(1)
    {
        bzero(req_buf, sizeof(req_buf));
        bzero(&clientaddr,sizeof(clientaddr));
        req_len = recvfrom(sockfd, req_buf, sizeof(req_buf), 0,
                (struct sockaddr *)&clientaddr, &addr_len);
        if(req_len < 0)
        {
            perror("recvfrom error\n");
            exit(1);
        }

        req_buf[req_len] = '\0';
        //printf("req_len = %d\n",req_len);
        printf("Connect from client %s, port is %d.\n",
                inet_ntoa(clientaddr.sin_addr), htons(clientaddr.sin_port));

        int isCircle = 1;
        while(isCircle > 0)
        {
            isCircle--;
            while(!feof(h264Bitsream))
            {
                GetAnnexbNALU(n);
                dump(n);

                ts_current+=timestamp_increse;
                if(n->len <= UDP_MAX_SIZE){
                    memset(sendbuf,0,SEND_BUF_SIZE);
                    rtp_hdr =(RTP_FIXED_HEADER *)&sendbuf[0];
                    rtp_hdr->csrc_len = 0;
                    rtp_hdr->extension = 0;
                    rtp_hdr->padding = 0;
                    rtp_hdr->version = 2;
                    rtp_hdr->payload = H264;
                    rtp_hdr->marker  = 0;
                    rtp_hdr->seq_no = htons(++seq_num%UINT16_MAX);
                    rtp_hdr->timestamp =htonl(ts_current);
                    rtp_hdr->ssrc = htonl(SSRC_NUM);

                    nalu_hdr =(NALU_HEADER *)&sendbuf[12];
                    nalu_hdr->F = (n->forbidden_bit) >> 7;
                    nalu_hdr->NRI = (n->nal_reference_idc) >> 5;
                    nalu_hdr->TYPE = n->nal_unit_type;
                    //printf("%d %d\n",nalu_hdr->TYPE,n->nal_unit_type);

                    nalu_payload=&sendbuf[13];
                    memcpy(nalu_payload,n->buf+1,n->len-1);
                    bytes=n->len + 12;
                    //printf("%3d, len: %6d  ",rtpnum++, bytes);
                    printf("%3d   ",rtpnum-1);
                    print_time();
                    sendto(sockfd, sendbuf, bytes, 0, (struct sockaddr*)&clientaddr, addr_len);
                }
                else
                {
                    memset(sendbuf,0,SEND_BUF_SIZE);

                    rtp_hdr =(RTP_FIXED_HEADER *)&sendbuf[0];
                    rtp_hdr->csrc_len = 0;
                    rtp_hdr->extension = 0;
                    rtp_hdr->padding = 0;
                    rtp_hdr->version = 2;
                    rtp_hdr->payload = H264;
                    rtp_hdr->marker  = 0;
                    rtp_hdr->seq_no = htons(++seq_num%UINT16_MAX);
                    rtp_hdr->timestamp=htonl(ts_current);
                    rtp_hdr->ssrc    = htonl(SSRC_NUM);


                    int packetNum = n->len/UDP_MAX_SIZE;
                    if (n->len%UDP_MAX_SIZE != 0)
                        packetNum ++;

                    int lastPackSize = n->len - (packetNum-1)*UDP_MAX_SIZE;
                    int packetIndex = 1;


                    fu_ind =(FU_INDICATOR*)&sendbuf[12];
                    fu_ind->TYPE=28;

                    fu_hdr =(FU_HEADER*)&sendbuf[13];
                    fu_hdr->S=1;
                    fu_hdr->E=0;
                    fu_hdr->R=0;
                    fu_hdr->TYPE=n->nal_unit_type;

                    nalu_payload=&sendbuf[14];
                    memcpy(nalu_payload,n->buf+1,UDP_MAX_SIZE-1);
                    bytes=UDP_MAX_SIZE-1+14;

                    //printf("%3d, len: %6d  ",rtpnum++, bytes);
                    printf("%3d   ",rtpnum-1);
                    print_time();

                    sendto(sockfd, sendbuf, bytes, 0, (struct sockaddr*)&clientaddr, addr_len);

                    for(packetIndex = 2; packetIndex < packetNum;packetIndex++)
                    {
                        memset(sendbuf,0,SEND_BUF_SIZE);

                        rtp_hdr =(RTP_FIXED_HEADER *)&sendbuf[0];

                        rtp_hdr->csrc_len = 0;
                        rtp_hdr->extension = 0;
                        rtp_hdr->padding = 0;
                        rtp_hdr->version = 2;
                        rtp_hdr->payload = H264;
                        rtp_hdr->marker  = 0;
                        rtp_hdr->seq_no = htons(++seq_num%UINT16_MAX);
                        rtp_hdr->timestamp=htonl(ts_current);
                        rtp_hdr->ssrc    = htonl(SSRC_NUM);


                        fu_ind =(FU_INDICATOR*)&sendbuf[12];
                        fu_ind->F=n->forbidden_bit>>7;
                        fu_ind->NRI=n->nal_reference_idc>>5;
                        fu_ind->TYPE=28;

                        fu_hdr =(FU_HEADER*)&sendbuf[13];
                        fu_hdr->S=0;
                        fu_hdr->E=0;
                        fu_hdr->R=0;
                        fu_hdr->TYPE=n->nal_unit_type;

                        nalu_payload=&sendbuf[14];
                        memcpy(nalu_payload,n->buf+(packetIndex-1)*UDP_MAX_SIZE,UDP_MAX_SIZE);
                        bytes=UDP_MAX_SIZE+14;

                        //printf("%3d, len: %6d  ",rtpnum++, bytes);
                        sendto(sockfd, sendbuf, bytes, 0, (struct sockaddr*)&clientaddr, addr_len);
                    }

                    memset(sendbuf,0,SEND_BUF_SIZE);

                    rtp_hdr =(RTP_FIXED_HEADER *)&sendbuf[0];

                    rtp_hdr->csrc_len = 0;
                    rtp_hdr->extension = 0;
                    rtp_hdr->padding = 0;
                    rtp_hdr->version = 2;
                    rtp_hdr->payload = H264;
                    rtp_hdr->marker  = 1;
                    rtp_hdr->seq_no = htons(++seq_num%UINT16_MAX);
                    rtp_hdr->timestamp=htonl(ts_current);
                    rtp_hdr->ssrc    = htonl(SSRC_NUM);

                    fu_ind =(FU_INDICATOR*)&sendbuf[12];
                    fu_ind->F=n->forbidden_bit>>7;
                    fu_ind->NRI=n->nal_reference_idc>>5;
                    fu_ind->TYPE=28;

                    fu_hdr =(FU_HEADER*)&sendbuf[13];
                    fu_hdr->S=0;
                    fu_hdr->E=1;
                    fu_hdr->R=0;
                    fu_hdr->TYPE=n->nal_unit_type;

                    nalu_payload=&sendbuf[14];
                    memcpy(nalu_payload,n->buf+(packetIndex-1)*UDP_MAX_SIZE,lastPackSize);
                    bytes=lastPackSize+14;

                    sendto(sockfd, sendbuf, bytes, 0, (struct sockaddr*)&clientaddr, addr_len);
                    //send(sockfd, sendbuf, bytes, 0);
                }
                usleep(20000);
            }//while(!feof(bits))
            rewind(h264Bitsream);
            rtpnum = 0;
            //ts_current = 0;
            if(isCircle == 0)
            {
                printf("--------\n");
                scanf("%d",&isCircle);
                fflush(stdin);
            }
        }
        //disconnect()
        printf("-------------------\n");
    }

    FreeNALU(n);
    close(sockfd);
    return 0;
}
