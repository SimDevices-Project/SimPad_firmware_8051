#ifndef __BSP_H
#define __BSP_H

//#define SIMPAD_V2_AE
//#define SIMPAD_NANO
#define SIMPAD_V2

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

#define NP ((void*)(-1))

#if defined(SIMPAD_V2_AE)
    #define BT1 P33
    #define BT2 P32
    #define BT3 P34
    #define BT4 P31
    #define BT5 P30
#elif defined(SIMPAD_NANO)
    #define BT1 P15
    #define BT2 P17
    #define BT3 P16
    #define BT4 NP
    #define BT5 NP
#elif defined(SIMPAD_V2)
    #define BT1 P32
    #define BT2 P33
    #define BT3 P11
    #define BT4 P10
    #define BT5 P34
    #define G1R P15
    #define G1G P16
    #define G1B P14
    #define G2R P13
    #define G2G P12
    #define G2B P17
#else
    #error "No board defined!"
#endif

#endif