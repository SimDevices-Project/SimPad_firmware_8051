/*--------------------------------------------------------------------------
PinMap.H
Header file for SimPad Nano pinmap.
****************************************
**  Copyright (C)  Handle 2018-2020   **
**  Web:       https://sim.bysb.net   **
****************************************
--------------------------------------------------------------------------*/

#include "Public/CH552.H"

#ifndef __PIN_MAP__
#define __PIN_MAP__

#ifdef __cplusplus
extern "C" {
#endif

sbit ws2812_data = P1^4; // WS2812 信号引脚

sbit BT1 = P1^7; // 默认设置 Z
sbit BT2 = P1^5; // 默认设置 X

sbit BT3 = P1^6; // 默认设置 Ctrl+R

// sbit BT4 = P1^6;//默认设置F1
// sbit BT5 = P1^6;//默认设置F2

#ifdef __cplusplus
}
#endif

#endif