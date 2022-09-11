#ifndef __MY_PROGRAMER_H
#define __MY_PROGRAMER_H

#define UART_INPUT_BUF_LEN 128
#define UART_INPUT_END_CHAR '\n'

#define TT_DATA 0
#define TT_EOF 1
#define TT_EXT_ADDR 2          /*基地址偏移 2字节*/
#define TT_START_ADDR 3        /*不知道是什么*/
#define TT_EXT_LINEAR_ADDR 4   /*基地址偏移 2字节<<16*/
#define TT_START_LINEAR_ADDR 5 /*程序入口地址 4字节 大概-_-!!!*/

#define ERR_OK 0
#define ERR_NO_HEX 1
#define ERR_LENGTH_TOO_SHORT 2
#define ERR_LENGTH_TOO_LONG 3
#define ERR_CHECK_SUM_FAIL 4
#define ERR_DATA_LENGTH_ERR 5
#define ERR_HEX_ROW_FORMAT_ERR 6
#define ERR_PROGRAM_READ_BACK_UNMATCH 7
#define ERR_PROGRAM_ADDR_INVALID 8
#define ERR_PROGRAM_LEN_INVALID 9
#define ERR_PROGRAM_WRITE_TIMEOUT 10
#define ERR_PROGRAM_UNSUPPORTED 11

#define HINDEX(x) (unsigned char)(x - 'a')

typedef void (*CmdHandler)(char *buf, uint8_t len);
extern CmdHandler handlers[];

typedef struct
{
    uint8_t type;
    uint16_t startAddr;
    uint8_t data[50];
    uint8_t dataLen;
} HexRow;

/*必须实现该接口*/
void handleHelp();
/*必须实现该接口 编程中调用，返回错误码，不要在里面打印任何信息*/
uint8_t programing(uint32_t addr, uint8_t *buf512, uint16_t len, bool isEOF);
/*必须实现该接口 编程中调用，返回错误码，不要在里面打印任何信息*/
uint8_t programingEEPROM(uint32_t addr, uint8_t *buf512, uint16_t len, bool isEOF);
/*必须实现该接口 编程中调用，返回错误码，不要在里面打印任何信息 note: the buf size is 512*/
uint8_t programingBin(uint32_t addr, uint8_t *buf);
/*必须实现该接口 /p进入编程模式时调用*/
bool beforeProgram(bool mustEraseChip);
/*必须实现该接口 编程成功执行最后一条结束语句后调用*/
void afterProgram();

#ifdef __cplusplus
extern "C"
{
#endif
    /*必须实现该接口*/
    void myprogramer_setup();

    void myprogramer_start();

    void printErrMsg(uint8_t errCode);
#ifdef __cplusplus
}
#endif

#endif