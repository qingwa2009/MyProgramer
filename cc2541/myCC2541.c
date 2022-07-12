#include <Arduino.h>
#include "../mylib/MyPin.h"
#include "myCC2541.h"

#define PIN_DC 5
#define PIN_DD 6
#define PIN_RST 4

#define DC_HIGH MYPIN_WRITE_HIGH(PIN_DC)
#define DC_LOW MYPIN_WRITE_LOW(PIN_DC)
#define DD_HIGH MYPIN_WRITE_HIGH(PIN_DD)
#define DD_LOW MYPIN_WRITE_LOW(PIN_DD)

bool readFailed = 0;

static struct DMAConfigParam dma0Config = {
    .srcAddrH = 0x62, /*dbgdata register*/
    .srcAddrL = 0x60,
    .destAddrH = 0, /*BUF0 start address, 0x0000*/
    .destAddrL = 0,
    .vlen = 0,                    /*Use LEN for transfer count*/
    .lenH = HIBYTE(DMA_BUF_SIZE), /*DMA channel transfer count*/
    .lenL = LOBYTE(DMA_BUF_SIZE),
    .wordSize = 0,   /*Each DMA transfer is 8 bit*/
    .TMODE = 0,      /*Single mode DMA transfer*/
    .trig = 0x1F,    /*DBG_BW trigger*/
    .srcInc = 0b00,  /*Do not increment source address*/
    .destInc = 0b01, /*Increment destination 1 B/word.*/
    .irqMask = 0,    /*Disable interrupt generation*/
    .m8 = 0,         /*Use all 8 bits for transfer count*/
    .priority = 0b01 /*Assured, DMA at least every second try*/
};

static struct DMAConfigParam dma1Config = {
    .srcAddrH = 0x00, /*BUF0 start address, 0x0000*/
    .srcAddrL = 0x00,
    .destAddrH = 0x62, /*Flash controller’s FWDATA address*/
    .destAddrL = 0x73,
    .vlen = 0,                    /*Use LEN for transfer count*/
    .lenH = HIBYTE(DMA_BUF_SIZE), /*DMA channel transfer count*/
    .lenL = LOBYTE(DMA_BUF_SIZE),
    .wordSize = 0,   /*Each DMA transfer is 8 bit*/
    .TMODE = 0,      /*Single mode DMA transfer*/
    .trig = 0x12,    /*FLASH trigger*/
    .srcInc = 0b01,  /*Increment source address 1 B/word*/
    .destInc = 0b00, /*No not increment destination address ???*/
    .irqMask = 0,    /*Disable interrupt generation*/
    .m8 = 0,         /*Use all 8 bits for transfer count*/
    .priority = 0b10 /*High, DMA has priority*/
};

uint16_t toLittleEndian16(uint16_t v)
{
    return (v << 8) | (v >> 8);
}

void _sendByte(uint8_t b)
{
    MYPIN_WRITE_MODE(PIN_DD);

    for (uint8_t i = 0; i < 8; i++)
    {
        DC_HIGH;
        if (b & 0x80)
        {
            DD_HIGH;
        }
        else
        {
            DD_LOW;
        }
        delayMicroseconds(1);
        DC_LOW;
        delayMicroseconds(1);
        b <<= 1;
    }
}

/*如果在一段时间内读取不到数据就会读取失败，readFaild=1*/
void _readBytes(uint8_t *buf, uint8_t len)
{
    MYPIN_READ_MODE_PULLUP(PIN_DD);
    uint8_t i;
    /*wait for ready*/
    for (i = 0; i < 16; i++)
    {
        if (MYPIN_READ_LOW(PIN_DD))
            break;
        for (uint8_t i = 0; i < 8; i++)
        {
            DC_HIGH;
            DC_LOW;
        }
    }
    if (i == 16)
    {
        readFailed = 1;
        return;
    }

    for (uint8_t j = 0; j < len; j++)
    {
        uint8_t b = 0;
        for (uint8_t i = 0; i < 8; i++)
        {
            // digitalWrite(PIN_DC, HIGH);
            DC_HIGH;
            b <<= 1;
            asm volatile("nop");
            asm volatile("nop");
            // asm volatile("nop");
            // asm volatile("nop");
            if (MYPIN_READ_HIGH(PIN_DD))
                b |= 1;
            // digitalWrite(PIN_DC, LOW);
            DC_LOW;
        }
        buf[j] = b;
    }

    readFailed = 0;
}
/*第0字节id，第1字节ver 失败时readFail=1*/
uint16_t getChipIdAndVer()
{
    uint16_t idver;
    _sendByte(CMD_GET_CHIP_ID);
    _readBytes((uint8_t *)&idver, sizeof(idver));
    return idver;
}

bool getChipInfo(struct ChipInfo *info)
{
    return readXDataMem(0x6276, (uint8_t *)info, sizeof(struct ChipInfo)); /*CHIPINFO0 0x6276 CHIPINFO1 0x6277*/
}

/*返回小端PC值 失败时readFail=1*/
uint16_t getPC()
{
    uint16_t pc;
    _sendByte(CMD_GET_PC);
    _readBytes((uint8_t *)&pc, sizeof(pc));
    pc = toLittleEndian16(pc);
    return pc;
}

/*返回the 3 least-significant bits are the currently used memory bank. 失败时readFail=1*/
uint8_t getCurrMemBank()
{
    uint8_t ret8;
    _sendByte(CMD_BM);
    _readBytes(&ret8, 1);
    return ret8;
}

/*失败时readFail=1*/
struct debugStatus readStatus()
{
    struct debugStatus status;
    _sendByte(CMD_READ_STATUS);
    _readBytes((uint8_t *)&status, sizeof(status));
    return status;
}

/*
The debug command SET_HW_BRKPNT is used to set one of the four available hardware breakpoints.
When a hardware breakpoint is enabled, it compares the CPU address bus with the breakpoint. When a
match occurs, the CPU is halted.
addrH: breakpoint addr high byte    
addrL: breakpoint addr low byte    
*/
struct debugStatus setHardwareBreakPoint(struct hardBreakPointProp *prop, uint8_t addrH, uint8_t addrL)
{
    struct debugStatus status;
    _sendByte(CMD_SET_HW_BRKPNT);
    _sendByte((*((uint8_t *)prop)));
    _sendByte(addrH);
    _sendByte(addrL);
    _readBytes((uint8_t *)&status, sizeof(status));
    return status;
}
/*
调试-下一句 失败时readFail=1
Step CPU instruction. The CPU executes the next instruction from
program memory and increments the program counter after execution.
return the resulting accumulator register value after the instruction
has been executed
return acc reg
*/
uint8_t debugStepNext()
{
    uint8_t ret;
    _sendByte(CMD_STEP_INSTR);
    _readBytes(&ret, sizeof(ret));
    return ret;
}
/*调试-暂停 失败时readFail=1*/
struct debugStatus debugHalt()
{
    struct debugStatus status;
    _sendByte(CMD_HALT);
    _readBytes((uint8_t *)&status, sizeof(status));
    return status;
}
/*调试-继续 失败时readFail=1*/
struct debugStatus debugResume()
{
    struct debugStatus status;
    _sendByte(CMD_RESUME);
    _readBytes((uint8_t *)&status, sizeof(status));
    return status;
}

/*调试-执行指令 len 1~3  return acc reg, 失败时readFail=1*/
uint8_t debugInstr(uint8_t *instr, uint8_t len)
{
    uint8_t ret;
    _sendByte(CMD_DEBUG_INSTR | (0x03 & len));
    for (uint8_t i = 0; i < len; i++)
    {
        _sendByte(instr[i]);
    }
    _readBytes(&ret, sizeof(ret));
    return ret;
}

uint8_t debugInstr1(uint8_t instr0)
{
    uint8_t ret;
    _sendByte(CMD_DEBUG_INSTR | 0x01);
    _sendByte(instr0);
    _readBytes(&ret, sizeof(ret));
    return ret;
}
uint8_t debugInstr2(uint8_t instr0, uint8_t instr1)
{
    uint8_t ret;
    _sendByte(CMD_DEBUG_INSTR | 0x02);
    _sendByte(instr0);
    _sendByte(instr1);
    _readBytes(&ret, sizeof(ret));
    return ret;
}
uint8_t debugInstr3(uint8_t instr0, uint8_t instr1, uint8_t instr2)
{
    uint8_t ret;
    _sendByte(CMD_DEBUG_INSTR | 0x03);
    _sendByte(instr0);
    _sendByte(instr1);
    _sendByte(instr2);
    _readBytes(&ret, sizeof(ret));
    return ret;
}
/*
addr: 0x0000 - 0x7FFF
bank: 0~7
*/
bool readCodeMem(uint16_t addr, uint8_t bank, uint8_t *buf, uint16_t len)
{
    addr += 0x8000;
    debugInstr3(0x75, 0xC7, bank);                 /*MOV MEMCTR, bank;*/
    debugInstr3(0x90, HIBYTE(addr), LOBYTE(addr)); /*MOV DPTR, address;*/
    for (uint16_t i = 0; i < len; i++)
    {
        buf[i] = debugInstr1(0xE0); /*MOVX A, @DPTR */
        if (readFailed)
        {
            return 0;
        }
        debugInstr1(0xA3); /*INC DPTR;*/
    }

    return 1;
}

bool readXDataMem(uint16_t addr, uint8_t *buf, uint8_t len)
{
    debugInstr3(0x90, addr >> 8, addr); /*MOV DPTR, address;*/
    for (uint8_t i = 0; i < len; i++)
    {
        buf[i] = debugInstr1(0xE0); /* MOVX A, @DPTR;*/
        if (readFailed)
            return 0;
        debugInstr1(0xA3); /*INC DPTR;*/
    }
    return 1;
}

void setPC(uint16_t addr)
{
    debugInstr3(0x02, addr >> 8, addr); /*LJMP address;*/
}

/*失败时readFail=1*/
struct debugConfig readDebugConfig()
{
    struct debugConfig config;
    _sendByte(CMD_RD_CONFIG);
    _readBytes((uint8_t *)&config, sizeof(config));
    return config;
}

/*失败时readFail=1*/
struct debugStatus writeDebugConfig(struct debugConfig *config)
{
    struct debugStatus status;
    _sendByte(CMD_WR_CONFIG);
    _sendByte(*(uint8_t *)config);
    _readBytes((uint8_t *)&status, sizeof(status));
    return status;
}

/*writes a sequence of 1–2048 bytes to the DBGDATA register*/
struct debugStatus burstWrite(uint8_t *bs, uint16_t len)
{
    struct debugStatus status;
    _sendByte(CMD_BURST_WRITE | (len >> 8));
    _sendByte(len);
    for (uint16_t i = 0; i < len; i++)
    {
        _sendByte(bs[i]);
    }
    _readBytes((uint8_t *)&status, sizeof(status));
    return status;
}

struct debugStatus chipErase()
{
    struct debugStatus status;
    _sendByte(CMD_CHIP_ERASE);
    _readBytes((uint8_t *)&status, sizeof(status));
    while (status.CHIP_ERASE_BUSY)
    {
        status = readStatus();
        if (readFailed)
            break;
    }
    return status;
}

/*Initializes the 32 MHz crystal oscillator*/
bool clockInit()
{
    debugInstr3(0x75, 0xC6, 0x80); /*MOV CLKCON, #80H;*/
    // uint8_t sleepReg;
    // do
    // {
    //     sleepReg = debugInstr2(0xE5, 0xBE); /*MOV A, sleepReg;*/
    //     if (readFailed)
    //     {
    //         return 0;
    //     }
    // } while (!(sleepReg & 0x40));
    delay(10);
    return 1;
}

bool enableDMA()
{
    struct debugConfig config = readDebugConfig();
    if (readFailed)
    {
        memset(&config, 0, sizeof(config));
        config.SOFT_POWER_MODE = 1;
        config.TIMER_SUSPEND = 1;
        config.DMA_PAUSE = 1;
    }
    if (!config.DMA_PAUSE)
        return 1;
    config.DMA_PAUSE = 0;
    writeDebugConfig(&config);
    config = readDebugConfig();
    if (readFailed || config.DMA_PAUSE)
        return 0;
    return 1;
}

void writeSFRMem(uint8_t addr, uint8_t v)
{
    debugInstr3(0x75, addr, v);
}

void writeXDataMem(uint16_t addr, uint8_t *bs, uint8_t len)
{
    /*Enable use of DMA in debug configuration*/

    debugInstr3(0x90, addr >> 8, addr); /*MOV DPTR, address;*/
    for (uint8_t i = 0; i < len; i++)
    {
        debugInstr2(0x74, bs[i]); /* MOV A, #bs[n];*/
        debugInstr1(0xF0);        /* MOVX @DPTR, A;*/
        debugInstr1(0xA3);        /*INC DPTR;*/
    }
}

/*
flash addr 总共18位，这里提供最高的16位，flash一次必须读4的倍数位
len最少4
与writeFlashMem对应
*/
bool readCodeMem2(uint16_t addr, uint8_t *buf, uint16_t len)
{

    uint8_t bank = addr / 0x2000; /*addr/(32*1024>>2)*/
    uint16_t addrEnd = addr + (len >> 2) - 1;
    addr <<= 2;
    addr &= 0x7FFF;
    // return addrEnd > 0x7FFF;
    if (addrEnd / 0x2000 != bank)
    {
        /*读取的数据跨了2个bank*/
        uint16_t len1 = (uint8_t)((addrEnd - 0x7FFF) * 4);
        uint16_t len0 = len - len1;
        readCodeMem(addr, bank, buf, len0);
        addrEnd <<= 2;
        addrEnd &= 0x7FFF;
        return readCodeMem(0, bank + 1, &buf[len0], len1);
    }
    else
    {

        return readCodeMem(addr, bank, buf, len);
    }
}

/*
flash addr 总共18位，这里提供最高的16位，flash一次必须写4位
len最少4
*/
void writeFlashMem(uint16_t addr, uint8_t *buf, uint16_t len)
{
    /*DMA configurations*/
    dma0Config.lenH = HIBYTE(len);
    dma0Config.lenL = LOBYTE(len);
    dma1Config.lenH = HIBYTE(len);
    dma1Config.lenL = LOBYTE(len);

    uint16_t daddr0 = DMA_BUF_SIZE;
    uint16_t daddr1 = daddr0 + sizeof(dma0Config);
    writeXDataMem(daddr0, (uint8_t *)&dma0Config, sizeof(dma0Config));
    writeXDataMem(daddr1, (uint8_t *)&dma1Config, sizeof(dma1Config));

    /*Point DMA controller to DMA configurations*/
    writeSFRMem(0xD5, HIBYTE(daddr0)); /*DMA0CFGH*/
    writeSFRMem(0xD4, LOBYTE(daddr0)); /*DMA0CFGL*/
    writeSFRMem(0xD3, HIBYTE(daddr1)); /*DMA1CFGH*/
    writeSFRMem(0xD2, LOBYTE(daddr1)); /*DMA1CFGL*/

    /*Arm DMA channel 0*/
    writeSFRMem(0xD6, 0x01); /*DMAARM*/

    /*Point Flash controller to start address */
    writeXDataMem(0x6271, (uint8_t *)&addr, 2); /*FADDRL FADDRH*/

    /*Transfer data using BURST_WRITE*/
    burstWrite(buf, len);

    /*Arm DMA channel 1*/
    writeSFRMem(0xD6, 0x02); /*DMAARM*/

    /*Trigger flash controller*/
    uint8_t fctl = 0x0A;
    writeXDataMem(0x6270, &fctl, 1);

    /*waiting for flash write complete*/
    do
    {
        if (!readXDataMem(0x6270, &fctl, 1))
            break;
    } while (fctl & 0x80);
}

void CC2541EnterDebugMode()
{
    delay(10);
    MYPIN_WRITE_MODE(PIN_RST);
    MYPIN_WRITE_MODE(PIN_DD);
    MYPIN_WRITE_MODE(PIN_DC);

    MYPIN_WRITE_HIGH(PIN_RST);
    DC_LOW;
    DD_LOW;

    /*enter debug mode*/
    MYPIN_WRITE_LOW(PIN_RST);
    DC_HIGH;
    delay(10);
    DC_LOW;
    delay(10);
    DC_HIGH;
    delay(10);
    DC_LOW;
    delay(10);
    MYPIN_WRITE_HIGH(PIN_RST);

    delay(10);
}

void CC2541ExitDebugMode()
{
    MYPIN_WRITE_MODE(PIN_RST);
    MYPIN_READ_MODE(PIN_DD);
    MYPIN_READ_MODE(PIN_DC);

    MYPIN_WRITE_LOW(PIN_DD);
    MYPIN_WRITE_LOW(PIN_DC);
    MYPIN_WRITE_LOW(PIN_RST);
    delay(10);

    MYPIN_WRITE_HIGH(PIN_RST);
    delay(10);
}
