#ifndef __MY_PWM_H
#define __MY_PWM_H
/*timer prescale*/
// #define Timer0_Clock_Stop 0
// #define Timer0_Clock_1 1
// #define Timer0_Clock_1_8 2
// #define Timer0_Clock_1_64 3
// #define Timer0_Clock_1_256 4
// #define Timer0_Clock_1_1024 5
// #define Timer0_Clock_T0_Falling 6
// #define Timer0_Clock_T0_Rising 7

#define Timer1_Clock_Stop 0
#define Timer1_Clock_1 1
#define Timer1_Clock_1_8 2
#define Timer1_Clock_1_64 3
#define Timer1_Clock_1_256 4
#define Timer1_Clock_1_1024 5
#define Timer1_Clock_T1_Falling 6
#define Timer1_Clock_T1_Rising 7

#define Timer2_Clock_Stop 0
#define Timer2_Clock_1 1
#define Timer2_Clock_1_8 2
#define Timer2_Clock_1_32 3
#define Timer2_Clock_1_64 4
#define Timer2_Clock_1_128 5
#define Timer2_Clock_1_256 6
#define Timer2_Clock_1_1024 7

/*timer wave generation mode*/
#define Timer2_Count_Normal 0
#define Timer2_Count_PPWM_0_FF_0 1
#define Timer2_Count_CTC_0_OCRA 2
#define Timer2_Count_FPWM_0_FF 3
#define Timer2_Count_PPWM_0_OCRA_0 5
#define Timer2_Count_FPWM_0_OCRA 7

#define Timer1_Count_Normal 0
#define Timer1_Count_PPWM_0_00FF_0 1
#define Timer1_Count_PPWM_0_01FF_0 2
#define Timer1_Count_PPWM_0_03FF_0 3
#define Timer1_Count_CTC_0_OCRA 4
#define Timer1_Count_FPWM_0_00FF 5
#define Timer1_Count_FPWM_0_01FF 6
#define Timer1_Count_FPWM_0_03FF 7
#define Timer1_Count_PPWMFC_0_ICR_0 8
#define Timer1_Count_PPWMFC_0_OCRA_0 9
#define Timer1_Count_PPWM_0_ICR_0 10
#define Timer1_Count_PPWM_0_OCRA_0 11
#define Timer1_Count_CTC_0_ICR 12
#define Timer1_Count_FPWM_0_ICR 14
#define Timer1_Count_FPWM_0_OCRA 15

/*timer compare output mode*/
#define Timer2_OCx_Disconnected 0
#define Timer2_OCx_NonPWM_Toggle 1
#define Timer2_OCx_NonPWM_Out0 2
#define Timer2_OCx_NonPWM_Out1 3
#define Timer2_OCA_FPWM_0_FF_Disconnected 1
#define Timer2_OCA_FPWM_0_OCRA_Toggle 1
#define Timer2_OCx_FPWM_Out0_On_Match_Out1_at_Bottom 2
#define Timer2_OCx_FPWM_Out1_On_Match_Out0_at_Bottom 3
#define Timer2_OCA_PPWM_0_FF_0_Disconnected 1
#define Timer2_OCA_PPWM_0_OCRA_0_Toggle 1
#define Timer2_OCx_PPWM_Out0_On_UpMatch_Out1_On_DownMatch 2
#define Timer2_OCx_PPWM_Out1_On_UpMatch_Out0_On_DownMatch 3

#define Timer1_OCx_Disconnected 0
#define Timer1_OCx_NonPWM_Toggle 1
#define Timer1_OCx_NonPWM_Out0 2
#define Timer1_OCx_NonPWM_Out1 3
#define Timer1_OCA_FPWM_0_OCRA_Toggle 1
#define Timer1_OCA_FPWM_0_ICR_Toggle 1
#define Timer1_OCx_FPWM_Out0_On_Match_Out1_at_Bottom 2
#define Timer1_OCx_FPWM_Out1_On_Match_Out0_at_Bottom 3
#define Timer1_OCA_PPWM_0_OCRA_0_Toggle 1
#define Timer1_OCA_PPWMFC_0_OCRA_0_Toggle 1
#define Timer1_OCx_PPWM_Out0_On_UpMatch_Out1_On_DownMatch 2
#define Timer1_OCx_PPWMFC_Out0_On_UpMatch_Out1_On_DownMatch 2
#define Timer1_OCx_PPWM_Out1_On_UpMatch_Out0_On_DownMatch 3
#define Timer1_OCx_PPWMFC_Out1_On_UpMatch_Out0_On_DownMatch 3

#ifdef __cplusplus
extern "C"
{
#endif
    void timerPause();
    void timerResume();
    void timer2SetClockSource(uint8_t Timer_Clock_Source);
    void timer2SetCountType(uint8_t Timer_Count_Type);
    void timer2SetCountValue(uint8_t v);
    uint8_t timer2GetCountValue();
    void timer2SetAOnCompMatchOutType(uint8_t Timer_OCnx_Type);
    void timer2SetACompMatchValue(uint8_t v);
    uint8_t timer2GetACompMatchValue();
    void timer2SetBOnCompMatchOutType(uint8_t Timer_OCnx_Type);
    void timer2SetBCompMatchValue(uint8_t v);
    uint8_t timer2GetBCompMatchValue();
    void timer2EnableAMatchInterrupt();
    void timer2EnableBMatchInterrupt();
    void timer2EnableOverflowInterrupt();
    void timer2DisableAMatchInterrupt();
    void timer2DisableBMatchInterrupt();
    void timer2DisableOverflowInterrupt();
    void timer2ForceAMatch();
    void timer2ForceBMatch();

    void timer1SetClockSource(uint8_t Timer_Clock_Source);
    void timer1SetCountType(uint8_t Timer_Count_Type);
    void timer1SetCountValue(uint16_t v);
    uint16_t timer1GetCountValue();
    void timer1SetAOnCompMatchOutType(uint8_t Timer_OCnx_Type);
    void timer1SetACompMatchValue(uint16_t v);
    uint16_t timer1GetACompMatchValue();
    void timer1SetBOnCompMatchOutType(uint8_t Timer_OCnx_Type);
    void timer1SetBCompMatchValue(uint16_t v);
    uint16_t timer1GetBCompMatchValue();
    void timer1SetICRValue(uint16_t v);
    uint16_t timer1GetICRValue();
    void timer1EnableAMatchInterrupt();
    void timer1EnableBMatchInterrupt();
    void timer1EnableOverflowInterrupt();
    void timer1DisableAMatchInterrupt();
    void timer1DisableBMatchInterrupt();
    void timer1DisableOverflowInterrupt();
    void timer1EnableInputCaptureInterrupt();
    void timer1DisableInputCaptureInterrupt();
    void timer1ForceAMatch();
    void timer1ForceBMatch();
#ifdef __cplusplus
}
#endif

#endif