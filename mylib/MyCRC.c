/*抄来的*/

/*
len最大255
polynomial多项式x^7+x^3+1
*/
unsigned char crc7(unsigned char *bs, unsigned char len, unsigned char polynomial)
{
    unsigned char crc = 0;
    unsigned char i, j;
    for (i = 0; i < len; i++)
    {
        crc ^= bs[i];
        for (j = 0; j < 8; j++)
        {
            if (crc & 0x80)
            {
                crc ^= polynomial;
            }
            crc <<= 1;
        }
    }
    crc >>= 1;
    return crc;
}
