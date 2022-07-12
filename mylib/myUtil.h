#ifndef __MY_UTIL_H
#define __MY_UTIL_H

#ifdef __cplusplus
extern "C"
{
#endif
    void toLowerCase(char *buf, uint8_t len);
    void toUpperCase(char *buf, uint8_t len);
    bool Hex2Byte(char b1, char b0, uint8_t *out_b);
    void Byte2Hex(uint8_t b, char *b1, char *b0);
#ifdef __cplusplus
}
#endif

#endif