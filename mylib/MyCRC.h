#ifndef __MY_CRC_H
#define __MY_CRC_H

#ifdef __cplusplus
extern "C"
{
#endif
    unsigned char crc7(unsigned char *bs, unsigned char len, unsigned char polynomial);

#ifdef __cplusplus
}
#endif

#endif