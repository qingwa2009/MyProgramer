#include <Arduino.h>
#include "MyPWM.h"
void timerPause()
{
    GTCCR |= _BV(TSM);
}

void timerResume()
{
    GTCCR &= ~(_BV(TSM));
}

void timer2SetClockSource(uint8_t Timer_Clock_Source)
{
    TCCR2B &= ~(_BV(CS22) | _BV(CS21) | _BV(CS20));
    TCCR2B |= Timer_Clock_Source;
}

void timer2SetCountType(uint8_t Timer_Count_Type)
{
    TCCR2A &= ~(_BV(WGM21) | _BV(WGM20));
    TCCR2A |= Timer_Count_Type & (_BV(WGM21) | _BV(WGM20));

    TCCR2B &= ~(_BV(WGM22));
    TCCR2B |= (Timer_Count_Type & 0b00000100) << 1;
}

/*注：对应引脚必须手动设置output才会生效*/
void timer2SetAOnCompMatchOutType(uint8_t Timer_OCnx_Type)
{
    TCCR2A &= ~(_BV(COM2A1) | _BV(COM2A0));
    TCCR2A |= Timer_OCnx_Type << 6;
}

/*注：对应引脚必须手动设置output才会生效*/
void timer2SetBOnCompMatchOutType(uint8_t Timer_OCnx_Type)
{
    TCCR2A &= ~(_BV(COM2B1) | _BV(COM2B0));
    TCCR2A |= Timer_OCnx_Type << 4;
}

inline uint8_t timer2GetCountValue()
{
    return TCNT2;
}

inline void timer2SetCountValue(uint8_t v)
{
    TCNT2 = v;
}
inline uint8_t timer2GetACompMatchValue()
{
    return OCR2A;
}
inline void timer2SetACompMatchValue(uint8_t v)
{
    OCR2A = v;
}

inline uint8_t timer2GetBCompMatchValue()
{
    return OCR2B;
}

inline void timer2SetBCompMatchValue(uint8_t v)
{
    OCR2B = v;
}

/*for non-PWM only, will not generate any interrupt*/
void timer2ForceAMatch()
{
    TCCR2B |= _BV(FOC2A);
}
/*for non-PWM only, will not generate any interrupt*/
void timer2ForceBMatch()
{
    TCCR2B |= _BV(FOC2B);
}
void timer2EnableAMatchInterrupt()
{
    TIMSK2 |= _BV(OCIE2A);
}
void timer2EnableBMatchInterrupt()
{
    TIMSK2 |= _BV(OCIE2B);
}
void timer2EnableOverflowInterrupt()
{
    TIMSK2 |= _BV(TOIE2);
}

void timer2DisableAMatchInterrupt()
{
    TIMSK2 &= ~(_BV(OCIE2A));
}

void timer2DisableBMatchInterrupt()
{
    TIMSK2 &= ~(_BV(OCIE2B));
}

void timer2DisableOverflowInterrupt()
{
    TIMSK2 &= ~(_BV(TOIE2));
}

void timer1SetClockSource(uint8_t Timer_Clock_Source)
{
    TCCR1B &= ~(_BV(CS12) | _BV(CS11) | _BV(CS10));
    TCCR1B |= Timer_Clock_Source;
}

void timer1SetCountType(uint8_t Timer_Count_Type)
{
    TCCR1A &= ~(_BV(WGM11) | _BV(WGM10));
    TCCR1A |= Timer_Count_Type & (_BV(WGM11) | _BV(WGM10));

    TCCR1B &= ~(_BV(WGM13) | _BV(WGM12));
    TCCR1B |= (Timer_Count_Type & 0b00001100) << 1;
}
void timer1SetCountValue(uint16_t v)
{
    uint8_t sreg = SREG; /* Save global interrupt flag */
    cli();               /* Disable interrupts */
    TCNT1 = v;
    SREG = sreg; /* Restore global interrupt flag */
}
uint16_t timer1GetCountValue()
{
    uint16_t v;
    uint8_t sreg = SREG; /* Save global interrupt flag */
    cli();               /* Disable interrupts */
    v = TCNT1;
    SREG = sreg; /* Restore global interrupt flag */
    return v;
}
/*注：对应引脚必须手动设置output才会生效*/
void timer1SetAOnCompMatchOutType(uint8_t Timer_OCnx_Type)
{
    TCCR1A &= ~(_BV(COM1A1) | _BV(COM1A0));
    TCCR1A |= Timer_OCnx_Type << 6;
}
void timer1SetACompMatchValue(uint16_t v)
{
    uint8_t sreg = SREG; /* Save global interrupt flag */
    cli();               /* Disable interrupts */
    OCR1A = v;
    SREG = sreg; /* Restore global interrupt flag */
}
uint16_t timer1GetACompMatchValue()
{
    uint16_t v;
    uint8_t sreg = SREG; /* Save global interrupt flag */
    cli();               /* Disable interrupts */
    v = OCR1A;
    SREG = sreg; /* Restore global interrupt flag */
    return v;
}
/*注：对应引脚必须手动设置output才会生效*/
void timer1SetBOnCompMatchOutType(uint8_t Timer_OCnx_Type)
{
    TCCR1A &= ~(_BV(COM1B1) | _BV(COM1B0));
    TCCR1A |= Timer_OCnx_Type << 4;
}
void timer1SetBCompMatchValue(uint16_t v)
{
    uint8_t sreg = SREG; /* Save global interrupt flag */
    cli();               /* Disable interrupts */
    OCR1B = v;
    SREG = sreg; /* Restore global interrupt flag */
}
uint16_t timer1GetBCompMatchValue()
{
    uint16_t v;
    uint8_t sreg = SREG; /* Save global interrupt flag */
    cli();               /* Disable interrupts */
    v = OCR1B;
    SREG = sreg; /* Restore global interrupt flag */
    return v;
}

/*
The ICR1 register can only be written when using a waveform generation mode 
that utilizes the ICR1 register for defining the counter’s TOP value. In these 
cases the waveform generation mode (WGM13:0) bits must be set before the 
TOP value can be written to the ICR1 registe
*/
void timer1SetICRValue(uint16_t v)
{
    uint8_t sreg = SREG; /* Save global interrupt flag */
    cli();               /* Disable interrupts */
    ICR1 = v;
    SREG = sreg; /* Restore global interrupt flag */
}
uint16_t timer1GetICRValue()
{
    uint16_t v;
    uint8_t sreg = SREG; /* Save global interrupt flag */
    cli();               /* Disable interrupts */
    v = ICR1;
    SREG = sreg; /* Restore global interrupt flag */
    return v;
}
/*for non-PWM only, will not generate any interrupt*/
void timer1ForceAMatch()
{
    TCCR1C |= _BV(FOC1A);
}
/*for non-PWM only, will not generate any interrupt*/
void timer1ForceBMatch()
{
    TCCR1C |= _BV(FOC1B);
}
void timer1EnableAMatchInterrupt()
{
    TIMSK1 |= _BV(OCIE1A);
}
void timer1EnableBMatchInterrupt()
{
    TIMSK1 |= _BV(OCIE1B);
}
void timer1EnableOverflowInterrupt()
{
    TIMSK1 |= _BV(TOIE1);
}
void timer1DisableAMatchInterrupt()
{
    TIMSK1 &= ~(_BV(OCIE1A));
}
void timer1DisableBMatchInterrupt()
{
    TIMSK1 &= ~(_BV(OCIE1B));
}
void timer1DisableOverflowInterrupt()
{
    TIMSK1 &= ~(_BV(TOIE1));
}
void timer1EnableInputCaptureInterrupt()
{
    TIMSK1 |= _BV(ICIE1);
}
void timer1DisableInputCaptureInterrupt()
{
    TIMSK1 &= ~(_BV(ICIE1));
}