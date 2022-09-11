#ifndef __MY_ATMEGA328_H
#define __MY_ATMEGA328_H
#include <Arduino.h>

#define SIZE_BYTES_PER_PAGE 128
#define SIZE_WORDS_PER_PAGE (SIZE_BYTES_PER_PAGE / 2)
#define SIZE_BYTES_PER_EEPROM_PAGE 4

#ifdef __cplusplus
extern "C"
{
#endif
    bool isFlashBusy();
    uint8_t readLockBits();
    bool writeLockBits(uint8_t data);
    uint8_t readSignatureByte(uint8_t a);
    uint8_t readFuseLowBits();
    bool writeFuseLowBits(uint8_t data);
    uint8_t readFuseHighBits();
    bool writeFuseHighBits(uint8_t data);
    uint8_t readFuseExtBits();
    bool writeFuseExtBits(uint8_t data);
    uint8_t readCalibrationByte();
    void loadExtendedAddrByte(uint8_t addr);
    uint8_t readProgramMemoryLowByte(uint16_t addr);
    uint8_t readProgramMemoryHighByte(uint16_t addr);
    // void loadProgramMemPageLowByte(uint8_t addr, uint8_t lByte);
    // void loadProgramMemPageHighByte(uint8_t addr, uint8_t hByte);
    // void loadProgramMemPageOneWord(uint8_t addr, uint8_t lByte, uint8_t hByte);
    // void loadProgramMemPage(uint8_t *bs);
    bool writeProgramMemPage(uint16_t addr, uint8_t *bs128);
    uint8_t readEEPROMMemory(uint16_t addr);
    bool writeEEPROMMemByte(uint16_t addr, uint8_t data);
    bool writeEEPROMMemPage(uint16_t addr, uint8_t *bs4);
    bool eraseChip();
    bool ATmega328EnterDebugMode();
    void ATmega328ExitDebugMode();
#ifdef __cplusplus
}
#endif

#endif