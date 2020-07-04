/*--------------------------------------------------------------------------
UsbDescriptor.H
Header file for SimPad Nano Anniversary Edition pinmap.
****************************************
**  Copyright (C)  Handle 2018-2020   **
**  Web:       https://sim.bysb.net   **
****************************************
--------------------------------------------------------------------------*/

#include "Public/Core/CH552.H"

#ifndef __USB_DESCRIPTOR__
#define __USB_DESCRIPTOR__

#ifdef __cplusplus
extern "C" {
#endif

/*设备描述符*/
//低字节在前
UINT8C DevDesc[18] = {0x12,           // 描述符长度(18字节)
                      0x01,           // 描述符类型
                      0x10,0x01,      // 本设备所用USB版本(1.1)
                      // 0x20,0x00,   //本设备所用USB版本(2.0)
                      0x00,           // 类代码
                      0x00,           // 子类代码
                      0x00,           // 设备所用协议
                      THIS_ENDP0_SIZE,// 端点0最大包长
                      0x88,0x80,      // 厂商ID
                      /*
                      * 00 01 SimPad v2
                      * 00 02 SimPad v2 - Extra
                      * 00 03 SimPad v2 - Lite
                      * 00 04 SimPad Nano
	                    * 00 05 SimPad 3
                      * 00 06 SimPad v2 - Year Edition
	                    * 00 07 SimPad Nano - Year Edition
                      * 00 FF SimPad Boot
                      */

                      0x07,0x00,      // 产品ID
                      0x01, 0x02,     // 设备版本号 (2.1)
                      0x01,           // 描述厂商信息的字符串描述符的索引值
                      0x02,           // 描述产品信息的字串描述符的索引值
                      0x00,           // 描述设备序列号信息的字串描述符的索引值
                      0x01            // 可能的配置数
                     };

#ifdef __cplusplus
}
#endif

#endif