/*没写完*/

#include <Arduino.h>
#include <avr/boot.h>
const volatile uint8_t program[] = {
    0x0C, 0x94, 0x34, 0x00, 0x0C, 0x94, 0x3E, 0x00, 0x0C, 0x94, 0x3E, 0x00, 0x0C, 0x94, 0x3E, 0x00,
    0x0C, 0x94, 0x3E, 0x00, 0x0C, 0x94, 0x3E, 0x00, 0x0C, 0x94, 0x3E, 0x00, 0x0C, 0x94, 0x3E, 0x00,
    0x0C, 0x94, 0x3E, 0x00, 0x0C, 0x94, 0x3E, 0x00, 0x0C, 0x94, 0x3E, 0x00, 0x0C, 0x94, 0x3E, 0x00,
    0x0C, 0x94, 0x3E, 0x00, 0x0C, 0x94, 0x3E, 0x00, 0x0C, 0x94, 0x3E, 0x00, 0x0C, 0x94, 0x3E, 0x00,
    0x0C, 0x94, 0x3E, 0x00, 0x0C, 0x94, 0x3E, 0x00, 0x0C, 0x94, 0x3E, 0x00, 0x0C, 0x94, 0x3E, 0x00,
    0x0C, 0x94, 0x3E, 0x00, 0x0C, 0x94, 0x3E, 0x00, 0x0C, 0x94, 0x3E, 0x00, 0x0C, 0x94, 0x3E, 0x00,
    0x0C, 0x94, 0x3E, 0x00, 0x0C, 0x94, 0x3E, 0x00, 0x11, 0x24, 0x1F, 0xBE, 0xCF, 0xEF, 0xD8, 0xE0,
    0xDE, 0xBF, 0xCD, 0xBF, 0x0E, 0x94, 0x40, 0x00, 0x0C, 0x94, 0x4C, 0x00, 0x0C, 0x94, 0x00, 0x00,
    0x25, 0x9A, 0x1D, 0x9A, 0x81, 0xE0, 0x94, 0xE0, 0x01, 0x97, 0xD9, 0xF3, 0x21, 0xE0, 0x34, 0xE0,
    0x21, 0x50, 0x31, 0x09, 0xE9, 0xF7, 0xF8, 0xCF, 0xF8, 0x94, 0xFF, 0xCF, 0xFF, 0xFF, 0xFF, 0xFF};

const uint16_t delay_count = 16000000 / 4000;

void _delay(uint8_t ms)
{
    uint16_t cnt;
    asm volatile(
        "\n"
        "L_dl1%=:"
        "\n\t"
        "mov %A0, %A2"
        "\n\t"
        "mov %B0, %B2"
        "\n\t"
        "L_dl2%=:"
        "\n\t"
        "sbiw %A0, 1"
        "\n\t"
        "brne L_dl2%="
        "\n\t"
        "dec %1"
        "\n\t"
        "brne L_dl1%="
        "\n\t"
        : "=&w"(cnt)
        : "r"(ms), "r"(delay_count));
}

/*1个page32个字节*/
void doSPM(uint16_t pageAddr, uint8_t spmcsr)
{
    // uint8_t spmcsr = 0x03;
    uint8_t temp, temp2;
    asm volatile(
        "Wait_spm%=:\n\t" /*check for previous SPM complete*/
        "in %[temp], %[portSPMCSR]\n\t"
        "sbrc %[temp], %[selfprgen]\n\t"
        "rjmp Wait_spm%=\n\t"

        "in %[temp2], %[portSREG]\n\t"
        "cli\n\t"
        "Wait_ee%=:\n\t" /*check that no EEPROM write access is present*/
        "sbic %[portEECR], %[eepe]\n\t"
        "rjmp Wait_ee%=\n\t"

        "movw r30, %A[pageAddr]\n\t"
        "out %[portSPMCSR], %[spmcsr]\n\t"
        "spm\n\t"
        "out %[portSREG],%[temp2]\n\t"

        : [temp] "=&r"(temp), [temp2] "=&r"(temp2)
        : [pageAddr] "r"(pageAddr),
          [portSPMCSR] "I"(_SFR_IO_ADDR(SPMCSR)), [spmcsr] "r"(spmcsr), [selfprgen] "I"(SELFPRGEN),
          [portSREG] "I"(_SFR_IO_ADDR(SREG)),
          [portEECR] "I"(_SFR_IO_ADDR(EECR)), [eepe] "I"(EEPE));
}

uint8_t _LPM(uint8_t spmcsr, uint16_t z)
{
    uint8_t temp;

    asm volatile("movw r30, %A[z]\n\t"
                 "out %[portSPMCSR], %[spmcsr]\n\t"
                 "lpm %[temp], z\n\t"
                 : [temp] "=&r"(temp)
                 : [z] "r"(z),
                   [portSPMCSR] "I"(_SFR_IO_ADDR(SPMCSR)), [spmcsr] "r"(spmcsr));
    return temp;
}

/* z取值：0x0001 (lockbit), 0x0000 (fuse low), 0x0003 (fuse high), 0x0002 (fuse extended)*/
uint8_t readFuseOrLockBit(uint16_t z)
{
    return _LPM(_BV(BLBSET) | _BV(SELFPRGEN), z);
}

/* z取值：(好像没效果)
    0x0001 (RC oscillator calibration byte) 
    0x0003 (TSGAIN - temp sensor gain) 
    0x0002 (TSOFFSET - temp sensor offset)??
    0x0000 (Device signature byte 1) 
    0x0002 (Device signature byte 2)??
    0x0004 (Device signature byte 3)
    */
uint8_t readSignatureRow(uint16_t z)
{
    return _LPM(_BV(SIGRD) | _BV(SPMEN), z);
}

void writePage()
{
    /*page erase*/
    doSPM(0x00, _BV(PGERS) | _BV(SELFPRGEN));
}
uint8_t volatile ret;

void boot_program_page(uint32_t page, uint8_t *buf)
{
    uint16_t i;
    uint8_t sreg;

    // Disable interrupts.

    sreg = SREG;
    cli();

    eeprom_busy_wait();

    boot_page_erase(page);
    boot_spm_busy_wait(); // Wait until the memory is erased.

    for (i = 0; i < SPM_PAGESIZE; i += 2)
    {
        // Set up little-endian word.

        uint16_t w = *buf++;
        w += (*buf++) << 8;

        boot_page_fill(page + i, w);
    }

    boot_page_write(page); // Store buffer in flash page.
    boot_spm_busy_wait();  // Wait until the memory is written.

    // Reenable RWW-section again. We need this if we want to jump back
    // to the application after bootloading.

    boot_rww_enable();

    // Re-enable interrupts (if they were ever enabled).

    SREG = sreg;
}