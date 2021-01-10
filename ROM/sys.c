#include "sys.h"

#include "ch552.h"
#include "bsp.h"
#include "rom.h"

#include <string.h>

volatile uint32_i sysTickCount = 0; // 系统刻计数器
static __xdata SysConfig sysConfig; // 系统配置存储

/*
 * 系统时钟配置
 */
void sysClockConfig() {
    SAFE_MOD = 0x55;
    SAFE_MOD = 0xAA;
    CLOCK_CFG |= bOSC_EN_INT;

#if OSC_EN_XT
    SAFE_MOD = 0x55;
    SAFE_MOD = 0xAA;
    CLOCK_CFG |= bOSC_EN_XT;                            // 使能外部晶振
    CLOCK_CFG &= ~bOSC_EN_INT;                          // 关闭内部晶振
#endif

    SAFE_MOD = 0x55;
    SAFE_MOD = 0xAA;
    CLOCK_CFG &= ~MASK_SYS_CK_SEL;

    SAFE_MOD = 0x55;
    SAFE_MOD = 0xAA;
#if FREQ_SYS == 24000000
    CLOCK_CFG |= 0x06;                                  // 24MHz
#endif
#if FREQ_SYS == 16000000
    CLOCK_CFG |= 0x05;                                  // 16MHz
#endif
#if FREQ_SYS == 12000000
    CLOCK_CFG |= 0x04;                                  // 12MHz
#endif
#if FREQ_SYS == 6000000
    CLOCK_CFG |= 0x03;                                  // 6MHz
#endif
#if FREQ_SYS == 3000000
    CLOCK_CFG |= 0x02;                                  // 3MHz
#endif
#if FREQ_SYS == 750000
    CLOCK_CFG |= 0x01;                                  // 750KHz
#endif
#if FREQ_SYS == 187500
    CLOCK_CFG |= 0x00;                                  // 187.5KHz
#endif

    SAFE_MOD = 0x00;
}

/*
 * 微秒延时
 */
void delay_us(uint16_t n) {
#ifdef	FREQ_SYS
#if		FREQ_SYS <= 6000000
    n >>= 2;
#endif
#if		FREQ_SYS <= 3000000
    n >>= 2;
#endif
#if		FREQ_SYS <= 750000
    n >>= 4;
#endif
#endif
	while (n) {  // total = 12~13 Fsys cycles, 1uS @Fsys=12MHz
		++ SAFE_MOD;  // 2 Fsys cycles, for higher Fsys, add operation here
    #ifdef	FREQ_SYS
    #if		FREQ_SYS >= 14000000
        ++ SAFE_MOD;
    #endif
    #if		FREQ_SYS >= 16000000
        ++ SAFE_MOD;
    #endif
    #if		FREQ_SYS >= 18000000
        ++ SAFE_MOD;
    #endif
    #if		FREQ_SYS >= 20000000
        ++ SAFE_MOD;
    #endif
    #if		FREQ_SYS >= 22000000
        ++ SAFE_MOD;
    #endif
    #if		FREQ_SYS >= 24000000
        ++ SAFE_MOD;
    #endif
    #endif
            -- n;
	}
}

/*
 * 毫秒延时
 */
void delay(uint16_t n) {
	while (n) {
        delay_us(1000);
		-- n;
	}
}

/*
 * 刻计数器更新中断
 * 需要注意的是，非总线LED驱动也使用此中断，这种情况下刻计数器更新在 rgb.c 内实现
 */
#if !defined(SIMPAD_V2)
void __tim2Interrupt() __interrupt (INT_NO_TMR2) __using (2) {
    if (TF2) {
        TF2 = 0;
        sysTickCount += 1;
    }
}
#endif

/*
 * 系统刻计数初始化
 */
void sysTickConfig() {
#if !defined(SIMPAD_V2)
    T2MOD &= ~bT2_CLK; C_T2 = 0;
    RCAP2 = T2COUNT = 0xFFFF - (uint16_t) (FREQ_SYS / 12L / 1000L);   // 1000Hz
    TR2 = 1;
    ET2 = 1;
#endif
}

/*
 * 获取当前刻
 */
uint32_t sysGetTickCount() {
    return sysTickCount;
}

/*
 * 通过RGB565数据和一个字节的扩展数据得到完整的RGB888
 */
uint32_t sysGetRGB(uint16_t color, uint8_t extend) {
    uint32_t val = 0;
    val = ((uint32_t) ((color & 0xF800) | ((extend & 0xE0) << 3))) << 8;
    val |= ((uint32_t) ((color & 0x07E0) | (extend & 0x18))) << 5;
    val |= ((color & 0x001F) << 3) | (extend & 0x07);
    return val;
}

#define KEY_CFG(i)  (sysConfig.keyConfig[i])
#define LED_CFG(i)  (sysConfig.ledConfig[i])

/*
 * 加载系统配置
 */
void sysLoadConfig() {
    memset(&sysConfig, 0x00, sizeof(SysConfig));

    uint16_t btc = 0, tmp = 0, addr = 0;
    btc = romRead16i(0x00);                     // BTC
    for (uint8_t i = 0; i < KEY_COUNT; i++) {
        KEY_CFG(i).mode = (KeyMode) ((btc >> (2 * i)) & 0x3);
        tmp = romRead16i(0x02 + i * 4);         // BTx
        KEY_CFG(i).marco = (tmp & 0x8000) != 0;
        if (!KEY_CFG(i).marco) {
            KEY_CFG(i).code = tmp & 0xFF;
        } else {
            addr = tmp & 0x7FFF;
            tmp = romRead16i(0x04 + i * 4);     // BTxL
            for (uint16_t j = 0; j < tmp; j++) {
                if (addr + j < FLASH_SIZE)
                    KEY_CFG(i).program[j] = romRead8i(addr + j);
                else
                    KEY_CFG(i).program[j] = romRead8e(addr + j);
            }
            KEY_CFG(i).length = tmp;
        }
    }

    for (uint8_t i = 0; i < LED_COUNT; i++) {
        addr = romRead16i(0x16 + i * 4);        // LEDx
        tmp = romRead16i(0x18 + i * 4);         // LEDxL
        LED_CFG(i).marco = (addr != 0xFFFF);
        if (!LED_CFG(i).marco) {
            addr = romRead8i(0x26 + i * 1);     // LEDxEX
            LED_CFG(i).color = sysGetRGB(tmp, addr);
        } else {
            for (uint16_t j = 0; j < tmp; j++) {
                if (addr + j < FLASH_SIZE)
                    LED_CFG(i).program[j] = romRead8i(addr + j);
                else
                    LED_CFG(i).program[j] = romRead8e(addr + j);
            }
            LED_CFG(i).length = tmp;
        }
    }
}

/*
 * 获取系统配置指针
 */
SysConfig* sysGetConfig() {
    return &sysConfig;
}
