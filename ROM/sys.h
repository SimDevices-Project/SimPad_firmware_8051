#ifndef __SYS_H
#define __SYS_H

#include <stdint.h>
#include <stdbool.h>

#include "bsp.h"

#define	 FREQ_SYS       24000000	        // 系统主频24MHz
#define  OSC_EN_XT      0                   // 外部晶振使能，默认开启内部晶振
#define  MARCO_MAX      48                  // 单个宏指令大小

void	sysClockConfig();                   // 时钟选择和配置
void	delay_us(uint16_t n);               // 以uS为单位延时
void	delay(uint16_t n);                  // 以mS为单位延时

void    sysTickConfig();
uint32_t sysGetTickCount();

//             无宏功能      按下触发宏    抬起触发宏   按住触发宏
typedef enum { KeyNone = 0, KeyDown = 1, KeyUp = 2, KeyPress = 3 } KeyMode;

typedef struct { // 按键配置结构体
    KeyMode mode;
    bool marco;
    uint8_t code;
    uint8_t program[MARCO_MAX];
    uint16_t length;
} KeyConfig;

typedef struct { // LED配置结构体
    bool marco;
    uint32_t color;
    uint8_t program[MARCO_MAX];
    uint16_t length;
} LEDConfig;

typedef struct { // 系统配置结构体
    KeyConfig keyConfig[KEY_COUNT];
    LEDConfig ledConfig[LED_COUNT];
} SysConfig;

uint32_t sysGetRGB(uint16_t color, uint8_t extend);
void sysLoadConfig();
SysConfig* sysGetConfig();

#endif
