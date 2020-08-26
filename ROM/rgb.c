#include "rgb.h"

#include "bsp.h"
#include "sys.h"

#if defined(SIMPAD_V2)
// [index][count: 0, control: 1][red: 0, green: 1, blue: 2]
volatile uint8_i RGBS[2][2][3];
#else
volatile uint32_i rgbGRBData[LED_COUNT];
#endif

#if defined(SIMPAD_V2)
extern volatile uint32_t sysTickCount;
volatile uint8_i __tick_cnt = 0;
void __tim2Interrupt() __interrupt (INT_NO_TMR2) __using (2) {
    if (TF2) {
        TF2 = 0;

        RGBS[0][0][0] += 1; RGBS[0][0][1] += 1; RGBS[0][0][2] += 1;
        RGBS[1][0][0] += 1; RGBS[1][0][1] += 1; RGBS[1][0][2] += 1;
        G1R = RGBS[0][0][0] > RGBS[0][1][0];
        G1G = RGBS[0][0][1] > RGBS[0][1][1];
        G1B = RGBS[0][0][2] > RGBS[0][1][2];
        G2R = RGBS[1][0][0] > RGBS[1][1][0];
        G2G = RGBS[1][0][1] > RGBS[1][1][1];
        G2B = RGBS[1][0][2] > RGBS[1][1][2];

        if (__tick_cnt < 61) __tick_cnt += 1;
        else {
            __tick_cnt = 0;
            sysTickCount += 1;
        }
    }
}
#endif

void rgbInit() {
#if defined(SIMPAD_V2)
    T2MOD |= (bTMR_CLK | bT2_CLK);
    RCAP2 = T2COUNT = 0xFFFF - (uint16_t) (FREQ_SYS / (256L * 240L));   // 240fps, 61400Hz
    TR2 = 1;
    ET2 = 1;
#else
    for (uint8_t i = 0; i < LED_COUNT; i++)
        rgbGRBData[i] = 0xFFFFFF;
#endif
}

void rgbSet(uint8_t index, uint32_t value) {
#if defined(SIMPAD_V2)
    RGBS[index % 2][1][0] = (value >> 16) & 0xFF;
    RGBS[index % 2][1][1] = (value >> 8 ) & 0xFF;
    RGBS[index % 2][1][2] = (value      ) & 0xFF;
#else
    rgbGRBData[index % LED_COUNT] = ((value & 0x00FF00) << 8) | ((value & 0xFF0000) >> 8) | (value & 0x0000FF);
#endif
}

#if !defined(SIMPAD_V2)
void __ws_rst() {
    LED = 0;
    delay_us(500);
    LED = 1;
}

void __ws_send(uint32_t value) {
    uint8_t i, j;
    for (i = 0; i < 24; i++) {
        if (value & 0x800000) {
            LED = 1;
            for (j = 4; j > 0; j--)
                __asm__("nop");
            LED = 0;
            for (j = 1; j > 0; j--)
                __asm__("nop");
        } else {
            LED = 1;
            for (j = 1; j > 0; j--)
                __asm__("nop");
            LED = 0;
            for (j = 4; j > 0; j--)
                __asm__("nop");
        }
        value <<= 1;
    }
}
#endif

void rgbPush() {
#if !defined(SIMPAD_V2)
    E_DIS = 1;
    __ws_rst();
    uint8_t i;
    for (i = 0; i < LED_COUNT; i++)
        __ws_send(rgbGRBData[i]);
    E_DIS = 0;
#endif
}
