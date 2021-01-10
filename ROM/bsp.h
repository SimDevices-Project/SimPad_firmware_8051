#ifndef __BSP_H
#define __BSP_H

#ifndef __MULTI_COMPILE__
/* 编译目标设置 开始 */
#define SIMPAD_V2_AE
//#define SIMPAD_NANO_AE
//#define SIM_KEY
//#define SIMPAD_V2
//#define SIMPAD_NANO
/* 编译目标设置 结束 */
#endif

/* 切换至性能模式 */
//#define PERFORMANCE_MODE

/* 使能字符串输入模拟指令 */
//#define INSTR_ENB_STRP

/* 使能灯光渐变列表 */
#define RGB_USE_GRAD_LIST
/* 使能简化灯光渐变列表，CH552受存储空间限制，必须打开 */
#define RGB_GRAD_LIST_LITE

__sfr   __at (0x90) P1;
__sbit  __at (0x90) P10;
__sbit  __at (0x91) P11;
__sbit  __at (0x92) P12;
__sbit  __at (0x93) P13;
__sbit  __at (0x94) P14;
__sbit  __at (0x95) P15;
__sbit  __at (0x96) P16;
__sbit  __at (0x97) P17;

__sfr   __at (0xB0) P3;
__sbit  __at (0xB0) P30;
__sbit  __at (0xB1) P31;
__sbit  __at (0xB2) P32;
__sbit  __at (0xB3) P33;
__sbit  __at (0xB4) P34;
__sbit  __at (0xB5) P35;
__sbit  __at (0xB6) P36;
__sbit  __at (0xB7) P37;

/* 芯片内部存储空间大小 */
#define FLASH_SIZE 0x80

/* 板级接口定义，需要注意的是，按键需定义为BTx */
#if defined(SIMPAD_V2_AE)
    #define BT1 P33
    #define BT2 P32
    #define BT3 P34
    #define BT4 P31
    #define BT5 P30
    #define KEY_COUNT 5
    #define LED P12
    #define LED_COUNT 4
#elif defined(SIMPAD_NANO_AE)
    #define BT1 P15
    #define BT2 P17
    #define BT3 P16
    #define KEY_COUNT 3
    #define LED P14
    #define LED_COUNT 2
    #define HAS_ROM
    #define ROM_SIZE 0x800
    #define ROM_SDA P11
    #define ROM_SCL P10
    #define ROM_WP P33
#elif defined(SIM_KEY)
    #define BT1 P17
    #define KEY_COUNT 1
    #define LED P14
    #define LED_COUNT 1
    #define HAS_ROM
    #define ROM_SIZE 0x800
    #define ROM_SDA P11
    #define ROM_SCL P10
    #define ROM_WP P33
#elif defined(SIMPAD_V2)
    #define BT1 P32
    #define BT2 P33
    #define BT3 P11
    #define BT4 P10
    #define BT5 P34
    #define KEY_COUNT 5
    #define G1R P15
    #define G1G P16
    #define G1B P14
    #define G2R P13
    #define G2G P12
    #define G2B P17
    #define LED_COUNT 2
#elif defined(SIMPAD_NANO)
    #define BT1 P17
    #define BT2 P15
    #define BT3 P16
    #define KEY_COUNT 3
    #define LED P14
    #define LED_COUNT 2
#else
    #error "No board defined!"
#endif

#endif