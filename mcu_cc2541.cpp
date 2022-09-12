#include <Arduino.h>
#include "./mylib/myUtil.h"
#include "myProgramer.h"
#include "cc2541/myCC2541.h"

/*打印小端16位数据*/
void printlnHex16(const __FlashStringHelper *preStr, uint16_t ret16)
{
    char hex[3] = {0, 0, 0};
    Byte2Hex(ret16 >> 8, &hex[0], &hex[1]);
    Serial.print(preStr);
    Serial.print(hex);
    Byte2Hex(ret16 & 0x00FF, &hex[0], &hex[1]);
    Serial.println(hex);
}

void printHex16(const __FlashStringHelper *preStr, uint16_t ret16)
{
    char hex[3] = {0, 0, 0};
    Byte2Hex(ret16 >> 8, &hex[0], &hex[1]);
    Serial.print(preStr);
    Serial.print(hex);
    Byte2Hex(ret16 & 0x00FF, &hex[0], &hex[1]);
    Serial.print(hex);
}

void printHex8(const __FlashStringHelper *preStr, uint8_t v)
{
    char hex[3] = {0, 0, 0};
    Byte2Hex(v, &hex[0], &hex[1]);
    Serial.print(preStr);
    Serial.print(hex);
}

void printlnHex8(const __FlashStringHelper *preStr, uint8_t v)
{
    char hex[3] = {0, 0, 0};
    Byte2Hex(v, &hex[0], &hex[1]);
    Serial.print(preStr);
    Serial.println(hex);
}

void printDebugStatus(const struct debugStatus *status)
{
    Serial.println(F("debug status: "));
    Serial.print(F("\tCHIP_ERASE_BUSY: "));
    Serial.println(status->CHIP_ERASE_BUSY, BIN);
    Serial.print(F("\tCPU_HALTED: "));
    Serial.println(status->CPU_HALTED, BIN);
    Serial.print(F("\tDEBUG_LOCKED: "));
    Serial.println(status->DEBUG_LOCKED, BIN);
    Serial.print(F("\tHALT_STATUS: "));
    Serial.println(status->HALT_STATUS, BIN);
    Serial.print(F("\tOSCILLATOR_STABLE: "));
    Serial.println(status->OSCILLATOR_STABLE, BIN);
    Serial.print(F("\tPCON_IDLE: "));
    Serial.println(status->PCON_IDLE, BIN);
    Serial.print(F("\tPM_ACTIVE: "));
    Serial.println(status->PM_ACTIVE, BIN);
    Serial.print(F("\tSTACK_OVERFLOW: "));
    Serial.println(status->STACK_OVERFLOW, BIN);
}

void printDebugConfig(const struct debugConfig *config)
{
    Serial.println(F("debug config: "));
    Serial.print(F("\tDMA_PAUSE: "));
    Serial.println(config->DMA_PAUSE, BIN);
    Serial.print(F("\tSOFT_POWER_MODE: "));
    Serial.println(config->SOFT_POWER_MODE, BIN);
    Serial.print(F("\tTIMER_SUSPEND: "));
    Serial.println(config->TIMER_SUSPEND, BIN);
    Serial.print(F("\tTIMERS_OFF: "));
    Serial.println(config->TIMERS_OFF, BIN);
}

void handleHelp()
{
    Serial.println(F("/d      enter debug mode"));
    Serial.println(F("/r      exit debug mode"));
    Serial.println(F("/gi     get chip id and version"));
    Serial.println(F("/gp     get PC value"));
    Serial.println(F("/gm     get curr Mem Bank"));
    Serial.println(F("/gs     get debug status"));
    Serial.println(F("/gc     get debug config"));
    Serial.println(F("/ga b xxxx    read code mem: b:memorybank(0~7) xxxx:0000~7FFF"));
    Serial.println(F("/gxn[n] xxxx  read xdata mem: n:(len):0~9 x(addr):0~F)"));
    Serial.println(F("/cn     cpu step next instr"));
    Serial.println(F("/ch     cpu halt"));
    Serial.println(F("/cr     cpu resume"));
    Serial.println(F("/exx...      exec instr xx...; xx max len: 3"));
    Serial.println(F("/wxx...      burst write xx... to dbgdata register; xx max len: 62"));
    Serial.println(F("/bsn b xxxx     set hardware breakpoint: n:0~3 b:memorybank(0~7) xxxx:addr"));
    Serial.println(F("/bcn     clear hardware breakpoint: n:0~3 "));
    Serial.println(F("/fe     flash erase chip"));
    Serial.println(F("/fr     read all flash"));
}

void _readChipIdAndVer()
{
    uint16_t ret16 = getChipIdAndVer();
    if (!readFailed)
    {
        printHex8(F("chip id: 0x"), ret16 & 0x00FF);
        printlnHex8(F(" ver: 0x"), ret16 >> 8);
    }
    else
    {
        Serial.println(F("read chip id failed!"));
    }
    struct ChipInfo info;
    if (getChipInfo(&info))
    {
        Serial.print(F("Flash size: "));
        switch (info.flashSize)
        {
        case 1:
            Serial.println(F("32KB"));
            break;
        case 2:
            Serial.println(F("64KB"));
            break;
        case 3:
            Serial.println(F("128KB"));
            break;
        case 4:
            Serial.println(F("256KB"));
            break;
        default:
            Serial.println(F("Unknown!"));
            break;
        }
        if (info.hasUSB)
        {
            Serial.println(F("has USB!"));
        }
        else
        {
            Serial.println(F("has not USB!"));
        }
        Serial.print(F("SRAM Size: "));
        Serial.print(info.sramSize + 1);
        Serial.println(F("KB"));
    }
    else
    {
        Serial.println(F("chip infomation read failed!"));
    }
}

void _readPC()
{
    uint16_t ret16 = getPC();
    if (!readFailed)
    {
        printlnHex16(F("PC: 0x"), ret16);
    }
    else
    {
        Serial.println(F("read PC failed!"));
    }
}

void _readDebugStatus()
{
    struct debugStatus status = readStatus();
    if (!readFailed)
    {
        printDebugStatus(&status);
    }
    else
    {
        Serial.println(F("read status failed!"));
    }
}
void _readDebugConfig()
{
    struct debugConfig config = readDebugConfig();
    if (!readFailed)
    {
        printDebugConfig(&config);
    }
    else
    {
        Serial.println(F("read debug config failed!"));
    }
}

void _readCurrMemBank()
{
    uint8_t ret8 = getCurrMemBank();
    if (!readFailed)
    {
        printlnHex16(F("MEM Bank: 0x"), ret8);
    }
    else
    {
        Serial.println(F("read curr MEM Bank failed!"));
    }
}

void _readCodeMem(char *buf, uint8_t len)
{
    uint8_t b = buf[4] - '0';
    uint8_t addrH, addrL;
    uint8_t bs[16];
    uint16_t addr;
    if (b > 7)
        goto HANDLE_ERR;
    if (!Hex2Byte(buf[6], buf[7], &addrH))
        goto HANDLE_ERR;
    if (!Hex2Byte(buf[8], buf[9], &addrL))
        goto HANDLE_ERR;
    addr = (addrH << 8) | addrL;

    if (readCodeMem(addr, b, bs, sizeof(bs)))
    {
        Serial.print(b);

        printHex16(F("-"), addr);

        Serial.print(F(":"));
        for (uint8_t i = 0; i < sizeof(bs); i++)
        {
            printHex8(F(" "), bs[i]);
        }
        Serial.println("");
    }
    else
    {
        Serial.println(F("read code memory failed!"));
    }
    return;
HANDLE_ERR:
    Serial.println(F("ERR /ga (0~7) (xxxx)"));
}

void _readXDataMem(char *buf, uint8_t len)
{
    uint8_t addrH, addrL;
    uint8_t b;

    bool nn = buf[4] == ' ' ? 0 : 1;
    uint8_t n = buf[3] - '0';
    if (n > 9)
        goto HANDLE_ERR;
    if (nn)
    {
        if (buf[4] - '0' > 9)
            goto HANDLE_ERR;
        n = n * 10 + buf[4] - '0';
    }

    uint16_t addr;
    if (!Hex2Byte(buf[5 + nn], buf[6 + nn], &addrH))
        goto HANDLE_ERR;
    if (!Hex2Byte(buf[7 + nn], buf[8 + nn], &addrL))
        goto HANDLE_ERR;
    addr = (addrH << 8) | addrL;

    printHex16(F(""), addr);
    Serial.print(F(":"));

    for (uint8_t i = 0; i < n; i++)
    {
        if (readXDataMem(addr, &b, 1))
        {
            printHex8(F(" "), b);
        }
        else
        {
            Serial.println(F("read xdata memory failed!"));
            break;
        }
        addr++;
    }
    Serial.println("");
    return;

HANDLE_ERR:
    Serial.println(F("ERR /gxn[n] xxxx  (n(len):0~9 x(addr):0~F)"));
}

void handleRead(char *buf, uint8_t len)
{
    switch (buf[2])
    {
    case 'i':
        _readChipIdAndVer();
        break;
    case 'p':
        _readPC();
        break;
    case 's':
        _readDebugStatus();
        break;
    case 'c':
        _readDebugConfig();
        break;
    case 'm':
        _readCurrMemBank();
        break;
    case 'a':
        _readCodeMem(buf, len);
        break;
    case 'x':
        _readXDataMem(buf, len);
        break;
    default:
        Serial.println(F("ERR /g[i|p|m|s|c|a]"));
        break;
    }
}

void _debugStepNext()
{
    uint16_t ret16;
    uint8_t ret8 = debugStepNext();
    if (!readFailed)
    {
        printHex8(F("acc reg: "), ret8);
        ret16 = getPC();
        if (!readFailed)
            printlnHex16(F("    PC: 0x"), ret16);
        else
            Serial.println(F("    PC: 0xXXXX"));
    }
    else
    {
        Serial.println(F("debug step next instr failed!"));
    }
}

void _debugHalt()
{
    struct debugStatus status = debugHalt();
    if (!readFailed)
    {
        printDebugStatus(&status);
    }
    else
    {
        Serial.println(F("debug halt, read status failed!"));
    }
}

void _debugResume()
{
    struct debugStatus status = debugResume();
    if (!readFailed)
    {
        printDebugStatus(&status);
    }
    else
    {
        Serial.println(F("debug resume, read status failed!"));
    }
}

void handleDebug(char *buf, uint8_t len)
{
    switch (buf[2])
    {
    case 'n':
        _debugStepNext();
        break;
    case 'h':
        _debugHalt();
        break;
    case 'r':
        _debugResume();
        break;
    default:
        Serial.println(F("ERR /c[n|h|r]"));
        break;
    }
}

void handleExecInstr(char *buf, uint8_t len)
{
    uint8_t ins[3];
    uint8_t count = 0;
    for (uint8_t i = 2; i < len; i += 2)
    {
        if (i + 1 >= len)
            break;
        uint8_t b;
        if (!Hex2Byte(buf[i], buf[i + 1], &b))
            break;
        ins[count] = b;
        count++;
        if (count > 3)
            break;
    }
    if (count < 1)
    {
        Serial.println(F("instr too short!"));
        return;
    }

    Serial.print(F("exec: "));
    for (uint8_t i = 0; i < count; i++)
    {
        printHex8(F(""), ins[i]);
    }
    Serial.println("");

    uint8_t ret8 = debugInstr(ins, count);
    if (!readFailed)
    {
        printlnHex8(F("acc reg: "), ret8);
    }
    else
    {
        Serial.println(F("exec instr failed!"));
    }
}

void handleBurstWrite(char *buf, uint8_t len)
{
    uint8_t data[(UART_INPUT_BUF_LEN - 4) / 2];
    uint8_t count = 0;
    for (uint8_t i = 2; i < len; i += 2)
    {
        if (i + 1 >= len)
            break;
        uint8_t b;
        if (!Hex2Byte(buf[i], buf[i + 1], &b))
            break;
        data[count] = b;
        count++;
    }
    if (count < 1)
    {
        Serial.println(F("data too short!"));
        return;
    }

    Serial.print(F("burst write: "));
    for (uint8_t i = 0; i < count; i++)
    {
        printHex8(F(""), data[i]);
    }
    Serial.println("");

    struct debugStatus status = burstWrite(data, count);
    if (!readFailed)
    {
        printDebugStatus(&status);
    }
    else
    {
        Serial.println(F("burst write, read status failed!"));
    }
}

void handleSetHWBreakPoint(char *buf, uint8_t len)
{
    struct debugStatus status = {0};
    struct hardBreakPointProp prop = {0};
    uint8_t b;
    uint8_t addrH, addrL;

    uint8_t n = buf[3] - '0';
    if (n > 3)
        goto HANDLE_ERR;
    prop.breakPointNum = n;

    switch (buf[2])
    {
    case 's':
        b = buf[5] - '0';
        if (b > 7)
            goto HANDLE_ERR;

        if (!Hex2Byte(buf[7], buf[8], &addrH))
            goto HANDLE_ERR;
        if (!Hex2Byte(buf[9], buf[10], &addrL))
            goto HANDLE_ERR;
        prop.enable = 1;
        prop.memoryBank = b;
        status = setHardwareBreakPoint(&prop, addrH, addrL);
        break;
    case 'c':
        prop.enable = 0;
        status = setHardwareBreakPoint(&prop, 0, 0);
        break;
    default:
        goto HANDLE_ERR;
        break;
    }
    if (!readFailed)
    {
        printDebugStatus(&status);
    }
    else
    {
        Serial.println(F("set hardware break point, read status failed!"));
    }
    return;
HANDLE_ERR:
    Serial.println(F("ERR /b[s|c][0~3] [0~7] [xxxx]"));
}

/*读取整个flash，前4个字节代表flash的字节数，最后4个字节是校验和*/
void _readAllFlash()
{
    struct ChipInfo info = {0};
    uint32_t flashSize = (uint32_t)256 * 1024;
    getChipInfo(&info);
    if (info.flashSize > 0)
        flashSize = ((uint32_t)32 * 1024) << (info.flashSize - 1);
    uint8_t bankCount = flashSize / 0x7FFF;
    uint8_t buf[512];
    uint32_t checkSum = 0;

    Serial.write((uint8_t *)&flashSize, sizeof(flashSize));
    delay(1);
    for (uint8_t bank = 0; bank < bankCount; bank++)
    {
        for (uint16_t i = 0; i <= 0x7FFF; i += sizeof(buf))
        {
            readCodeMem(i, bank, buf, sizeof(buf));
            for (size_t j = 0; j < sizeof(buf); j++)
            {
                Serial.write(buf[j]);
                checkSum += buf[j];
                // Serial.flush();
            }
            /*这样发好像老是接收不完整*/
            // Serial.write(buf, sizeof(buf));
            // Serial.flush();
        }
    }
    /*最后发送4字节校验和*/
    Serial.write((uint8_t *)&checkSum, sizeof(checkSum));
}

void _chipErase()
{
    struct debugStatus status;
    status = chipErase();
    if (!readFailed)
    {
        printDebugStatus(&status);
    }
    else
    {
        Serial.println(F("chip erase, read status failed!"));
    }
    delay(10);
}

void handleFlashControl(char *buf, uint8_t len)
{
    switch (buf[2])
    {
    case 'e':
        _chipErase();
        break;
    case 'r':
        _readAllFlash();
        break;
    default:
        Serial.println(F("ERR /f[e|r]"));
        break;
    }
}

/*退出debug模式，进入正常模式*/
void exitDebugMode(char *buf, uint8_t len)
{
    CC2541ExitDebugMode();
    Serial.println(F("exit debug mode!"));
}
/*进入debug模式*/
void enterDebugMode(char *buf, uint8_t len)
{
    CC2541EnterDebugMode();
    Serial.println(F("enter debug mode!"));
    /*使用32M外部时钟，如果不用外包时钟连电源线都可以不用接*/
    clockInit();
    // uint8_t sleepReg;
    // Serial.println(readXDataMem(0x70BE, &sleepReg, 1), BIN);
    // Serial.println(readXDataMem(0x709D, &sleepReg, 1), BIN);
}

void myprogramer_setup()
{

    handlers[HINDEX('r')] = exitDebugMode;
    handlers[HINDEX('d')] = enterDebugMode;
    handlers[HINDEX('g')] = handleRead;
    handlers[HINDEX('c')] = handleDebug;
    handlers[HINDEX('w')] = handleBurstWrite;
    handlers[HINDEX('e')] = handleExecInstr;
    handlers[HINDEX('b')] = handleSetHWBreakPoint;
    handlers[HINDEX('f')] = handleFlashControl;

    Serial.println(F("you should input /d to enter debug mode first!"));
    Serial.println(F("----->Notice: 8051 mcu is 'big endian'!<-----"));
}

bool beforeProgram(bool mustEraseChip)
{

    Serial.println(F("before program!"));

    if (!enableDMA())
    {
        Serial.println(F("enable DMA failed!"));
        return 0;
    }

    Serial.println(F("erasing chip..."));
    _chipErase();

    /*test*/
    // HexRow r = {
    //     .type = 0,
    //     .startAddr = 0x7FFF,
    //     .data = {0, 1, 2, 3, 4, 5, 6, 7},
    //     .dataLen = 8};
    // Serial.println(programing(0, &r));

    return 1;
}

uint8_t programing(uint32_t addr, uint8_t *buf512, uint16_t len, bool isEOF)
{
    uint16_t totalLen = len;
    uint8_t buf[512 + 6];

    uint8_t offset = 0;
    offset = addr % 4; /*起始地址不是4的倍数时，前面补齐0xFF到4的倍数*/

    addr -= offset;
    totalLen += offset;
    if ((totalLen % 4) > 0)
    {
        totalLen += 4 - (totalLen % 4); /*数据长度不是4的倍数时，后面补齐0xFF到4的倍数*/
    }

    memset(buf, 0xFF, sizeof(buf));
    memcpy(&buf[offset], buf512, len);

    uint16_t flashAddr = (uint16_t)(addr >> 2);
    writeFlashMem(flashAddr, buf, totalLen); /*写入flash*/

    /*验证是否正确写入*/

    readCodeMem2(flashAddr, buf, totalLen);

    for (uint16_t i = 0; i < len; i++)
    {
        if (buf[i + offset] != buf512[i])
        {
            Serial.print(F("addr "));
            Serial.print(addr + i + offset, HEX);
            Serial.print(F(" : "));
            Serial.print(buf512[i], HEX);
            Serial.print(F(" => "));
            Serial.println(buf[i + offset], HEX);
            // for (uint16_t j = 0; j < i + 6; j++)
            // {
            //     Serial.println(buf[j + offset], HEX);
            // }
            return ERR_PROGRAM_READ_BACK_UNMATCH;
        }
    }

    return ERR_OK;
}

uint8_t programingEEPROM(uint32_t addr, uint8_t *buf512, uint16_t len, bool isEOF)
{
    return ERR_PROGRAM_UNSUPPORTED;
}

uint8_t programingBin(uint32_t addr, uint8_t *buf)
{
    uint16_t flashAddr = (uint16_t)(addr >> 2);
    writeFlashMem(flashAddr, buf, 512);

    /*验证是否正确写入*/
    uint8_t buf2[512];
    readCodeMem2(flashAddr, buf2, 512);
    for (uint16_t i = 0; i < 512; i++)
    {
        if (buf[i] != buf2[i])
        {
            for (uint16_t j = 0; j < 512; j++)
            {
                Serial.print(buf[j], HEX);
                Serial.print(" ");
                Serial.println(buf2[j], HEX);
            }
            return ERR_PROGRAM_READ_BACK_UNMATCH;
        }
    }
    return ERR_OK;
}
uint8_t programingBinEEPROM(uint32_t addr, uint8_t *buf)
{
    return ERR_PROGRAM_UNSUPPORTED;
}
void afterProgram()
{
    Serial.println(F("after program!"));
    exitDebugMode(0, 0);
}