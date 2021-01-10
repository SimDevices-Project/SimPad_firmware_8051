#ifndef __RGB_H
#define __RGB_H

#include "ch552.h"

#define LED_FADE_LEN    4       // LED渐变列表最大长度

//             默认模式      正向渐变     反向渐变     触发模式
typedef enum { LEDNone = 0, LEDRGB = 1, LEDBGR = 2, LEDTRI = 3 } LEDMode;

// 渐变节点结构体
typedef struct {
    uint32_t color;
    uint16_t length;
} FadeNode;

// LED配置结构体
typedef struct {
    uint8_t trig;
    LEDMode mode;
    FadeNode nodes[LED_FADE_LEN];
    uint8_t length;
    uint8_t step;
    uint8_t count;
    uint32_t time;
    uint8_t r, g, b;
    uint32_t c;
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