#include "rgb.h"

#include "bsp.h"
#include "sys.h"

// [index][count: 0, control: 1][red: 0, green: 1, blue: 2]
volatile uint8_i RGBS[2][2][3];
volatile uint16_i __count = 0;
volatile uint16_i __hue = 0;

uint32_t hsv2rgb(uint16_t h, uint8_t s, uint8_t v) {
    uint8_i i;
	__idata float min, max;
	max = v * 2.55f;
	min = max * (100 - s) / 100.0f;

	i = h / 60;
	uint8_i difs = h % 60; // factorial part of h

	// RGB adjustment amount by hue 
	__idata float adj = (max - min) * difs / 60.0f;

    uint8_t r, g, b;
	switch (i) {
	case 0:
		r = max;
		g = min + adj;
		b = min;
		break;
	case 1:
		r = max - adj;
		g = max;
		b = min;
		break;
	case 2:
		r = min;
		g = max;
		b = min + adj;
		break;
	case 3:
		r = min;
		g = max - adj;
		b = max;
		break;
	case 4:
		r = min + adj;
		g = min;
		b = max;
		break;
	default:		// case 5:
		r = max;
		g = min;
		b = max - adj;
		break;
	}

    return ((uint32_t) r << 16) || ((uint32_t) g << 8) || (uint32_t) b;
}

void __rgbTim2Interrupt() __interrupt (INT_NO_TMR2) __using (2) {
#if defined(SIMPAD_V2)
    RGBS[0][0][0] += 1; RGBS[0][0][1] += 1; RGBS[0][0][2] += 1;
    RGBS[1][0][0] += 1; RGBS[1][0][1] += 1; RGBS[1][0][2] += 1;
    G1R = RGBS[0][0][0] > RGBS[0][1][0];
    G1G = RGBS[0][0][0] > RGBS[0][1][1];
    G1B = RGBS[0][0][0] > RGBS[0][1][2];
    G2R = RGBS[1][0][0] > RGBS[1][1][0];
    G2G = RGBS[1][0][0] > RGBS[1][1][1];
    G2B = RGBS[1][0][0] > RGBS[1][1][2];
#endif
    if (__count < 1024) __count += 1;
    else {
        __count = 0;
        __hue += 1;
        __hue %= 360;
        rgbSet(0, hsv2rgb(__hue, 100, 100));
        rgbSet(1, hsv2rgb(360 - __hue, 100, 100));
    }
}

void rgbInit() {
#if defined(SIMPAD_V2)
    T2MOD |= (bTMR_CLK | bT2_CLK);
    RCAP2 = T2COUNT = 0xFFFF - (390);   // 240fps
    TR2 = 1;
    ET2 = 1;
#endif
}

void rgbSet(uint8_t index, uint32_t value) {
#if defined(SIMPAD_V2)
    RGBS[index % 2][1][0] = (value >> 16) & 0xFF;
    RGBS[index % 2][1][1] = (value >> 8 ) & 0xFF;
    RGBS[index % 2][1][2] = (value      ) & 0xFF;
#endif
}
