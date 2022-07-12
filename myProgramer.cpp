#include <Arduino.h>
#include "myProgramer.h"
#include "./mylib/myUtil.h"

#define MODE_CMD 0
#define MODE_WRITING_PROGRAM_HEX 1
#define MODE_WRITING_PROGRAM_BIN 2
#define MODE_WRITING_EEPROM 3

#define PROGRAM_HEX 0
#define PROGRAM_BIN 1

#define RESP_END         \
    do                   \
    {                    \
        Serial.write(0); \
    } while (0)

static uint32_t baseAddr = 0;
static uint8_t errCode = ERR_OK;
static uint8_t mode = MODE_CMD;
static uint8_t buf512[512];
static uint16_t buf512Ind = 0;
static uint32_t nextProgramStartAddr = 0;
CmdHandler handlers[26] = {0};

void printErrMsg(uint8_t errCode)
{
    switch (errCode)
    {
    case ERR_OK:
        Serial.println("ok");
        break;
    case ERR_NO_HEX:
        Serial.println(F("err! parse err: no a hex string!"));
        break;
    case ERR_LENGTH_TOO_SHORT:
        Serial.println(F("err! parse err: length to short!"));
        break;
    case ERR_LENGTH_TOO_LONG:
        Serial.println(F("err! parse err: length to long!"));
        break;
    case ERR_CHECK_SUM_FAIL:
        Serial.println(F("err! parse err: check sum failed!"));
        break;
    case ERR_DATA_LENGTH_ERR:
        Serial.println(F("err! program err: data length error!"));
        break;
    case ERR_HEX_ROW_FORMAT_ERR:
        Serial.println(F("err! program err: hex row no start with ':'!"));
        break;
    default:
        Serial.print(F("err! Unknown err: "));
        Serial.println(errCode);
        break;
    }
}

void enterCmdMode()
{
    mode = MODE_CMD;
    Serial.println(F("cmd mode:"));
}

void enterProgramMode(char *buf, uint8_t len)
{

    if (!beforeProgram())
    {
        Serial.println(F("enter program mode failed!"));
        return;
    }

    mode = buf[2] == 'b' ? MODE_WRITING_PROGRAM_BIN : MODE_WRITING_PROGRAM_HEX;
    baseAddr = 0;
    nextProgramStartAddr = 0;
    buf512Ind = 0;
    errCode = ERR_OK;
    if (mode == MODE_WRITING_PROGRAM_HEX)
        Serial.println(F("program mode(HEX):"));
    else
        Serial.println(F("program mode(BIN):"));
}

bool parseHexFileRow(char *buf, uint8_t offset, uint8_t len, HexRow *hexRow)
{
    /*
     *:LLaaaaTTD...DCC
     *LL该行数据长度
     *aaaa该行数据起始地址
     *TT类型: 00-数据；01-文件结束；02-扩展段地址；03-开始段地址；04-扩展线性地址；05-开始线性地址
     *D...D数据
     *CC 校验
     */
    //wp:10 0000 00 0C944A000C9472000C9472000C947200 D0

    /*太短*/
    if (len - offset < 10)
    {
        errCode = ERR_LENGTH_TOO_SHORT;
        return 0;
    }

    uint8_t cc = 0;
    if (!Hex2Byte(buf[offset], buf[offset + 1], &(hexRow->dataLen)))
    {
        errCode = ERR_NO_HEX;
        return 0;
    }

    /*数据太长*/
    if (hexRow->dataLen > sizeof(hexRow->data))
    {
        errCode = ERR_LENGTH_TOO_LONG;
        return 0;
    }

    /*太短*/
    if (len - offset - hexRow->dataLen * 2 < 10)
    {
        errCode = ERR_LENGTH_TOO_SHORT;
        return 0;
    }

    cc += hexRow->dataLen;

    if (!Hex2Byte(buf[offset + 2], buf[offset + 3], (uint8_t *)&hexRow->startAddr))
    {
        errCode = ERR_NO_HEX;
        return 0;
    }
    cc += (hexRow->startAddr & 0x00FF);
    hexRow->startAddr <<= 8;

    if (!Hex2Byte(buf[offset + 4], buf[offset + 5], (uint8_t *)&hexRow->startAddr))
    {
        errCode = ERR_NO_HEX;
        return 0;
    }
    cc += (hexRow->startAddr & 0x00FF);

    if (!Hex2Byte(buf[offset + 6], buf[offset + 7], &hexRow->type))
    {
        errCode = ERR_NO_HEX;
        return 0;
    }
    cc += hexRow->type;

    for (uint8_t i = 0; i < hexRow->dataLen; i++)
    {
        if (!Hex2Byte(buf[offset + 8 + (i * 2)], buf[offset + 9 + (i * 2)], &hexRow->data[i]))
        {
            errCode = ERR_NO_HEX;
            return 0;
        }
        cc += hexRow->data[i];
    }
    cc = 256 - cc;

    uint8_t b;
    if (!Hex2Byte(buf[offset + 8 + (hexRow->dataLen * 2)], buf[offset + 9 + (hexRow->dataLen * 2)], &b))
    {
        errCode = ERR_NO_HEX;
        return 0;
    }

    if (cc != b)
    {
        errCode = ERR_CHECK_SUM_FAIL;
        return 0;
    }

    errCode = ERR_OK;
    return 1;
}

uint8_t mergeHexRowAndProgram(uint32_t baseAddr, HexRow *hexRow)
{
    uint8_t errcode = ERR_OK;
    uint32_t flashAddr = baseAddr + hexRow->startAddr;
    if ((nextProgramStartAddr == flashAddr) && (buf512Ind + hexRow->dataLen) <= 512 && (hexRow->type != TT_EOF))
    {
        nextProgramStartAddr += hexRow->dataLen;
    }
    else
    {
        errcode = programing(nextProgramStartAddr - buf512Ind, buf512, buf512Ind);
        nextProgramStartAddr = flashAddr + hexRow->dataLen;
        buf512Ind = 0;
    }
    for (uint16_t i = 0; i < hexRow->dataLen; i++)
    {
        buf512[buf512Ind] = hexRow->data[i];
        buf512Ind++;
    }
    return errcode;
}

/*返回错误码*/
uint8_t execHexFileRow(HexRow *hexRow)
{
    errCode = ERR_OK;

    switch (hexRow->type)
    {
    case TT_DATA:
        errCode = mergeHexRowAndProgram(baseAddr, hexRow);
        break;
    case TT_EOF:
        errCode = mergeHexRowAndProgram(baseAddr, hexRow); /*最后剩余的数据也要写进去*/
        baseAddr = 0;
        nextProgramStartAddr = 0;
        buf512Ind = 0;
        break;
    case TT_EXT_ADDR:
        if (hexRow->dataLen != 2)
        {
            errCode = ERR_DATA_LENGTH_ERR;
            return errCode;
        }
        baseAddr = ((uint32_t)hexRow->data[0] << 8) | ((uint32_t)hexRow->data[1]);
        break;
    case TT_EXT_LINEAR_ADDR:
        if (hexRow->dataLen != 2)
        {
            errCode = ERR_DATA_LENGTH_ERR;
            return errCode;
        }
        baseAddr = ((uint32_t)hexRow->data[0] << 24) | ((uint32_t)hexRow->data[1] << 16);
        break;
    case TT_START_LINEAR_ADDR:
        break;
    default:
        break;
    }

    return errCode;
}

void _handleHelp(char *buf, uint8_t len)
{
    Serial.println(F("-------help--------"));
    Serial.println(F("/p      enter program mode, you can send hex file row by row, "
                     "if program success it will return 'ok', "
                     "otherwise, it returns err msg!"));
    Serial.println(F("/pb     enter program mode, you can send bin file '512bytes data + 2byte2 checksum', if len=0 it finish program, "
                     "if program success it will return 'ok', "
                     "otherwise, it returns err msg!"));
    handleHelp();
    Serial.println(F("-------------------"));
}

void _handleProgramHex(char *buf, uint8_t len)
{
    errCode = ERR_HEX_ROW_FORMAT_ERR;
    HexRow hexRow;
    if ((buf[0] == ':') && parseHexFileRow(buf, 1, len, &hexRow) && (execHexFileRow(&hexRow) == ERR_OK))
    {
        Serial.println("ok");
        if (hexRow.type == TT_EOF)
        {
            afterProgram();
            enterCmdMode();
        }
    }
    else
    {
        printErrMsg(errCode);
        enterCmdMode();
    }
}

/*program bin file mode*/
void programBinFileMode()
{

    uint32_t size = 0;
    uint8_t buf[514]; /*512 data bytes + 2 check sum bytes*/

    /*program size*/
    for (uint8_t i = 0; i < 4; i++)
    {
        while (!Serial.available())
            ;
        *(((uint8_t *)&size) + i) = Serial.read();
    }
    Serial.println("ok");
    RESP_END;

    for (uint32_t addr = 0; addr < size; addr += 512)
    {
        /*get one block 512 data bytes + 2 check sum bytes*/
        for (uint16_t i = 0; i < sizeof(buf); i++)
        {
            while (!Serial.available())
                ;
            buf[i] = Serial.read();
        }
        /*check sum*/
        uint16_t checkSum = 0;
        uint8_t isAllFF = 1;
        for (uint16_t i = 0; i < sizeof(buf) - 2; i++)
        {
            checkSum += buf[i];
            if (isAllFF && buf[i] != 0xFF)
                isAllFF = 0;
        }

        if ((*(uint16_t *)(buf + 512)) != checkSum)
        {
            printErrMsg(ERR_CHECK_SUM_FAIL);
            enterCmdMode();
            return;
        }

        if (isAllFF) /*全0xFF就不写入*/
        {
            Serial.println("ok");
            RESP_END;
            continue;
        }

        errCode = programingBin(addr, buf);
        if (errCode == ERR_OK)
        {
            Serial.println("ok");
            RESP_END;
        }
        else
        {

            printErrMsg(errCode);
            enterCmdMode();
            return;
        }
    }

    afterProgram();
    enterCmdMode();
}

void myprogramer_start()
{
    handlers[HINDEX('h')] = _handleHelp;
    Serial.println(F("my programer start, input /h to get help!"));
    handlers[HINDEX('p')] = enterProgramMode;

    enterCmdMode();
    RESP_END;

    pinMode(13, OUTPUT);

    /*cmd mode and program hex file mode*/
    while (1)
    {
        char buf[UART_INPUT_BUF_LEN] = {0};
        uint8_t len = 0;
        len = Serial.readBytesUntil(UART_INPUT_END_CHAR, buf, sizeof(buf));
        if (len)
        {
            switch (mode)
            {
            case MODE_CMD:
                if (buf[0] == '/')
                {
                    toLowerCase(buf, len);
                    uint8_t ind = HINDEX(buf[1]);
                    if (handlers[ind])
                    {
                        handlers[ind](buf, len); /*call cmd handler*/
                        if (mode == MODE_WRITING_PROGRAM_BIN)
                        {
                            RESP_END;
                            programBinFileMode();
                            continue;
                        }
                        break;
                    }
                    Serial.println(F("Unknown Cmd! input /h to get help"));
                }
                break;
            case MODE_WRITING_PROGRAM_HEX:
                _handleProgramHex(buf, len);
                break;
            default:
                break;
            }
            RESP_END;
        }
    }
}
