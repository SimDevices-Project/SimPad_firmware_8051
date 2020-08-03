#include "sys.h"

#include "ch552.h"

void sysClockConfig() {
    SAFE_MOD = 0x55;
    SAFE_MOD = 0xAA;
    CLOCK_CFG |= bOSC_EN_INT;

#if OSC_EN_XT	
    SAFE_MOD = 0x55;
    SAFE_MOD = 0xAA;
    CLOCK_CFG |= bOSC_EN_XT;                            //使能外部晶振
    CLOCK_CFG &= ~bOSC_EN_INT;                          //关闭内部晶振 
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

void delay(uint16_t n) {
	while (n) {
        delay_us(1000);
		-- n;
	}
}                  
