#include "rgb.h"

#include "bsp.h"
#include "sys.h"

#if defined(SIMPAD_V2)
// [index][count: 0, control: 1][red: 0, green: 1, blue: 2]
static uint8_i RGBS[2][2][3];
#else
static uint32_i rgbGRBData[LED_COUNT];
#endif

#if defined(SIMPAD_V2)
extern volatile uint32_t sysTickCount;
volatile uint8_t __tick_cnt = 0;
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
    RGBS[index][1][0] = (value >> 16) & 0xFF;
    RGBS[index][1][1] = (value >> 8 ) & 0xFF;
    RGBS[index][1][2] = (value      ) & 0xFF;
#else
    rgbGRBData[index] = ((value & 0x00FF00) << 8) | ((value & 0xFF0000) >> 8) | (value & 0x0000FF);
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

static __xdata RGBConfig fadeConfig[LED_COUNT];
volatile uint16_i fadeLength = 0;

void rgbSetLed(uint16_t index, uint16_t value) {
    uint8_t i = index & 0xFF;
    fadeConfig[i].mode = LEDNone;
    fadeConfig[i].step = 0;
    fadeConfig[i].length = 0;
    fadeConfig[i].nodes[0].color = sysGetRGB(value, (index >> 8) & 0xFF);
    fadeConfig[i].nodes[0].length = 0;
    rgbSet(i, fadeConfig[i].nodes[0].color);
}

void rgbSetTime(uint16_t time) {
    fadeLength = time;
}

#define CFG(i) (fadeConfig[i])

void rgbAddFade(uint16_t index, uint16_t value) {
    uint8_t i = index & 0xFF;
    uint8_t len = CFG(i).length;
    CFG(i).nodes[len].color = sysGetRGB(value, (index >> 8) & 0xFF);
    CFG(i).nodes[len].length = fadeLength;
    CFG(i).length += 1;
    if (CFG(i).length == LED_FADE_LEN)
        CFG(i).length = 0;
}

void rgbSetTrig(uint16_t index, uint16_t value) {
    uint8_t i = index & 0xFF;
    CFG(i).trig = value & 0xFF;
}

void rgbSetMode(uint16_t index, uint16_t value) {
    uint8_t i = index & 0xFF;
    CFG(i).mode = (LEDMode) (value & 0xFF);
}

uint32_t __rgb__rainbow(uint8_t step, uint8_t count) {
    switch (step) {
    case 0:
        return 0xFF0000 | ((uint32_t) count << 8);
    case 1:
        return 0x00FF00 | ((uint32_t) (0xFF - count) << 16);
    case 2:
        return 0x00FF00 | count;
    case 3:
        return 0x0000FF | ((uint32_t) (0xFF - count) << 8);
    case 4:
        return 0x0000FF | ((uint32_t) count << 16);
    case 5:
        return 0xFF0000 | (0xFF - count);
    default:
        return 0;
    }
}

__bit __rgb_key(uint8_t i) {
    switch (i) {
#if (KEY_COUNT == 3)
    case 1:
        return BT1;
    case 2:
        return BT2;
    case 3:
        return BT3;
#elif (KEY_COUNT == 5)
    case 4:
        return BT4;
    case 5:
        return BT5;
#endif
    default:
        break;
    }
    return 0;
}

__bit __rgb__trig(uint8_t index) {
    for (uint8_t i = 0; i < KEY_COUNT; i++) {
        if (CFG(index).trig & (0x01 << i)) {
            if (__rgb_key(i) == 0)
                return 1;
        }
    }
    return 0;
}

volatile uint8_i trigCtl = 0, trigSte = 0;
volatile uint32_i prevTickCount = 0;

#define NODE(i, j) (CFG(i).nodes[CFG(i).step + (j)])

#if defined(RGB_USE_GRAD_LIST) && !defined(RGB_GRAD_LIST_LITE)
uint8_t __rgb__inter(uint8_t index, uint8_t input, int16_t total, uint16_t time) {
    int8_t sign = total > 0 ? 1 : -1;
    uint8_t d = (uint16_t) (sign * total) / time;
    uint32_t now = sysGetTickCount();
    if (d == 0) {
        d = time / (uint16_t) (sign * total);
        if ((now - CFG(index).time) % d == 0)
            input += sign * 1;
    } else if (prevTickCount != now) {
        prevTickCount = now;
        input += sign * d;
    }
    return input;
}
#endif

void __rgb__perform_rgb(uint8_t i) {
    uint32_t c = __rgb__rainbow(CFG(i).step, CFG(i).count);
    rgbSet(i, c);
    CFG(i).step += (CFG(i).count == 0 ? 1 : 0);
    CFG(i).step = CFG(i).step > 5 ? 0 : CFG(i).step;
    CFG(i).count += 1;
}

void __rgb__perform_bgr(uint8_t i) {
    uint32_t c = __rgb__rainbow(CFG(i).step, CFG(i).count);
    rgbSet(i, c);
    CFG(i).step -= (CFG(i).count == 0 ? 1 : 0);
    CFG(i).step = CFG(i).step > 5 ? 5 : CFG(i).step;
    CFG(i).count -= 1;
}

void __rgb__perform_trig(uint8_t trig, uint8_t pos, uint8_t v) {
    if (trig && (trigSte & pos) == 0) {
        trigSte |= pos;
        if ((trigCtl & pos) == 0 || (v != 0))
            trigCtl |= pos;
        else
            trigCtl &= ~pos;
    } else if (!trig && (trigSte & pos) == 1) {
        trigSte &= ~pos;
    }
}

void rgbUpdate() {
    uint32_t c = 0, cn = 0;
    uint8_t tmp, trig;
    for (uint8_t i = 0; i < LED_COUNT; i++) {
        trig = __rgb__trig(i);
        switch (CFG(i).mode) {
        case LEDNone:
        #ifdef RGB_USE_GRAD_LIST
            tmp = (0x01 << i);
            __rgb__perform_trig(trig, tmp, 1);

        #ifndef RGB_GRAD_LIST_LITE
            if ((trigCtl & tmp) != 0) {
                trigCtl &= ~tmp;
                CFG(i).step = 0;
                CFG(i).time = 0;
                CFG(i).r = CFG(i).g = CFG(i).b = 0;
            }

            if (CFG(i).time == 0) {
                CFG(i).time = sysGetTickCount();
                c = NODE(i, 0).color;
                CFG(i).r = (c >> 16) & 0xFF;
                CFG(i).g = (c >> 8 ) & 0xFF;
                CFG(i).b = (c >> 0 ) & 0xFF;
            }

            if (NODE(i, 0).length != 0) {
                uint8_x rt = 0, gt = 0, bt = 0;
                cn = NODE(i, 1).color;
                rt = (cn >> 16) & 0xFF - (c >> 16) & 0xFF;
                gt = (cn >> 8 ) & 0xFF - (c >> 8 ) & 0xFF;
                bt = (cn      ) & 0xFF - (c      ) & 0xFF;
                CFG(i).r = __rgb__inter(i, CFG(i).r, rt, NODE(i).length);
                CFG(i).g = __rgb__inter(i, CFG(i).g, gt, NODE(i).length);
                CFG(i).b = __rgb__inter(i, CFG(i).b, bt, NODE(i).length);
                c =  ((uint32_t) CFG(i).r) << 16;
                c |= ((uint32_t) CFG(i).g) << 8;
                c |= CFG(i).b;
                rgbSet(i, c);

                if (sysGetTickCount() - CFG(i).time > NODE(i, 0).length) {
                    CFG(i).step += 1;
                    CFG(i).time = 0;
                    CFG(i).r = CFG(i).g = CFG(i).b = 0;
                    if (CFG(i).step == CFG(i).length)
                        CFG(i).step = 0;
                }
            } else
                rgbSet(i, NODE(i, 0).color);
        #else
            CFG(i).step = 0;
            if ((trigCtl & tmp) != 0) {
                trigCtl &= ~tmp;
                CFG(i).b = 0;
                CFG(i).time = 0;
                CFG(i).c = NODE(i, 0).color;
            }

            cn = NODE(i, 0).length;
            if (cn != 0) {
                if (CFG(i).b == 0) {
                    CFG(i).c -= 0x010101;
                    CFG(i).time += 1;
                } else
                    CFG(i).c = 0;
                rgbSet(i, CFG(i).c);

                if ((CFG(i).time >> (cn & 0x7)) > (cn & 0xFFF8))
                    CFG(i).b = 1;
            } else {
                rgbSet(i, NODE(i, 0).color);
            }
        #endif

        #else
            rgbSet(i, NODE(i, 0).color);
        #endif
            break;
        case LEDRGB:
            __rgb__perform_rgb(i);
            break;
        case LEDBGR:
            __rgb__perform_bgr(i);
            break;
        case LEDTRI:
            tmp = (0x01 << i);
            __rgb__perform_trig(trig, tmp, 0);

            if ((trigCtl & tmp) != 0)
                __rgb__perform_rgb(i);
            else
                __rgb__perform_bgr(i);
            break;
        default:
            break;
        }
    }
}
