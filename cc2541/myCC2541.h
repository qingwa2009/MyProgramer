#ifndef __MY_CC2541_H
#define __MY_CC2541_H

#include <Arduino.h>

#define DMA_BUF_SIZE (512 + 16)

#define HIBYTE(x) ((uint8_t)(((uint16_t)(x) >> 8) & 0xFF))
#define LOBYTE(x) ((uint8_t)(x))

#define CMD_CHIP_ERASE 0x10    /*in:0 out:1*/
#define CMD_WR_CONFIG 0x18     /*in:1 out:1*/
#define CMD_RD_CONFIG 0x20     /*in:0 out:1*/
#define CMD_GET_PC 0x28        /*in:0 out:2*/
#define CMD_READ_STATUS 0x30   /*in:0 out:1*/
#define CMD_SET_HW_BRKPNT 0x38 /*in:3 out:1*/
#define CMD_HALT 0x40          /*in:0 out:1*/
#define CMD_RESUME 0x48        /*in:0 out:1*/
#define CMD_DEBUG_INSTR 0x50   /*in:1-3 out:1 0101 0Xyy*/
#define CMD_STEP_INSTR 0x58    /*in:0 out:1*/
#define CMD_BM 0x60            /*in:0 out:1*/
#define CMD_GET_CHIP_ID 0x68   /*in:0 out:2*/
#define CMD_BURST_WRITE 0x80   /*in:2-2049 out:1 1000 0kkk  kkkk kkkk*/

struct debugStatus
{
    uint8_t STACK_OVERFLOW : 1;    /*1: Stack overflow*/
    uint8_t OSCILLATOR_STABLE : 1; /*1: Oscillators stable*/
    uint8_t DEBUG_LOCKED : 1;      /*1: Debug interface is locked*/
    uint8_t HALT_STATUS : 1;       /*0: CPU was halted by HALT debug command, 1: CPU was halted by hardware breakpoint*/
    uint8_t PM_ACTIVE : 1;         /*0: Chip is in normal operation with CPU running (if not halted)*/
    uint8_t CPU_HALTED : 1;        /*1: CPU was halted from a breakpoint or from a HALT debug command*/
    uint8_t PCON_IDLE : 1;         /*1: CPU is not running*/
    uint8_t CHIP_ERASE_BUSY : 1;   /*1: Chip erase in progress*/
};

struct debugConfig
{
    uint8_t Reserved0 : 1;
    uint8_t TIMER_SUSPEND : 1; /*1: Suspend timers*/
    uint8_t DMA_PAUSE : 1;     /*1: Pause all DMA transfers*/
    uint8_t TIMERS_OFF : 1;    /*1: Disable timers*/
    uint8_t Reserved1 : 1;
    uint8_t SOFT_POWER_MODE : 1; /*0: the debug interface is reset during PM2 and PM3*/
    uint8_t Reserved2 : 1;
    uint8_t Reserved3 : 1;
};

struct ChipInfo
{
    uint8_t nouse0 : 3;
    uint8_t hasUSB : 1;
    uint8_t flashSize : 3; /*001 – 32 KB, 010 – 64 KB, 011 – 128 KB (for CC2533: 011 – 96 KB),100 – 256 KB*/
    uint8_t nouse1 : 1;
    uint8_t sramSize : 3; /*SRAM = (sramSize + 1)kb*/
    uint8_t nouse2 : 5;
};

struct hardBreakPointProp
{
    uint8_t memoryBank : 3;
    uint8_t enable : 1;
    uint8_t breakPointNum : 2;
    uint8_t unused : 2;
};

struct DMAConfigParam
{
    uint8_t srcAddrH;
    uint8_t srcAddrL;
    uint8_t destAddrH;
    uint8_t destAddrL;
    uint8_t lenH : 5;
    uint8_t vlen : 3;
    uint8_t lenL : 8;
    uint8_t trig : 5;
    uint8_t TMODE : 2;
    uint8_t wordSize : 1;
    uint8_t priority : 2;
    uint8_t m8 : 1;
    uint8_t irqMask : 1;
    uint8_t destInc : 2;
    uint8_t srcInc : 2;
};

extern bool readFailed;

#ifdef __cplusplus
extern "C"
{
#endif
    bool clockInit();
    void CC2541EnterDebugMode();
    void CC2541ExitDebugMode();
    uint16_t getChipIdAndVer();
    bool getChipInfo(struct ChipInfo *info);
    uint16_t getPC();
    uint8_t getCurrMemBank();
    struct debugStatus readStatus();
    uint8_t debugStepNext();
    struct debugStatus debugHalt();
    struct debugStatus debugResume();
    uint8_t debugInstr(uint8_t *instr, uint8_t len);
    struct debugConfig readDebugConfig();
    struct debugStatus writeDebugConfig();
    struct debugStatus burstWrite(uint8_t *bs, uint16_t len);
    struct debugStatus chipErase();
    struct debugStatus setHardwareBreakPoint(struct hardBreakPointProp *prop, uint8_t addrH, uint8_t addrL);
    bool readCodeMem(uint16_t addr, uint8_t bank, uint8_t *buf, uint16_t len);
    bool readCodeMem2(uint16_t addr, uint8_t *buf, uint16_t len);
    bool readXDataMem(uint16_t addr, uint8_t *buf, uint8_t len);
    void writeXDataMem(uint16_t addr, uint8_t *bs, uint8_t len);
    void writeSFRMem(uint8_t addr, uint8_t v);
    bool enableDMA();
    void writeFlashMem(uint16_t addr, uint8_t *buf, uint16_t len);
    void setPC(uint16_t addr);
#ifdef __cplusplus
}
#endif

#endif