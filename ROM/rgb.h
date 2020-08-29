#ifndef __RGB_H
#define __RGB_H

#include "ch552.h"

#define LED_FADE_ADDR   0x0156  // LED渐变列表存储起始地址（xRAM），需确认USB占用后的结束地址
#define LED_FADE_LEN    8       // LED渐变列表最大长度

typedef enum { LEDNone = 0, LEDRGB = 1, LEDBGR = 2, LEDTRI = 3 } LEDMode;

typedef struct {
    uint32_t color;
    uint16_t length;
} FadeNode;

typedef struct {
    uint8_t trig;
    LEDMode mode;
    FadeNode nodes[LED_FADE_LEN];
    uint8_t length;
    uint8_t step;
    uint8_t count;
    uint32_t time;
} RGBConfig;

void rgbInit();
void rgbSet(uint8_t index, uint32_t value);
void rgbPush();

void rgbSetLed(uint16_t index, uint16_t value);
void rgbSetTime(uint16_t time);
void rgbAddFade(uint16_t index, uint16_t value);
void rgbSetTrig(uint16_t index, uint16_t value);
void rgbSetMode(uint16_t index, uint16_t value);
void rgbUpdate();

#endif