# include "usb.h"

#include <stdlib.h>
#include <string.h>
#include "ch552.h"

#define THIS_ENDP0_SIZE DEFAULT_ENDP0_SIZE

const uint8_c usbDevDesc[] = {
    0x12,               // 描述符长度(18字节)
    0x01,               // 描述符类型
    0x10, 0x01,         // 本设备所用USB版本(1.1)
//  0x20, 0x00,         //本设备所用USB版本(2.0)
    0x00,               // 类代码
    0x00,               // 子类代码
    0x00,               // 设备所用协议
    THIS_ENDP0_SIZE, // 端点0最大包长
    0x88, 0x80,         // 厂商ID
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
    0x07, 0x00,         // 产品ID
    0x01, 0x02,         // 设备版本号 (2.1)
    0x01,               // 描述厂商信息的字符串描述符的索引值
    0x02,               // 描述产品信息的字串描述符的索引值
    0x00,               // 描述设备序列号信息的字串描述符的索引值
    0x01                // 可能的配置数
};

const uint8_c usbCfgDesc[] = {
    0x09,        //   bLength
    0x02,        //   bDescriptorType (Configuration)
    0x52, 0x00,  //   wTotalLength 82
    0x02,        //   bNumInterfaces 2
    0x01,        //   bConfigurationValue
    0x00,        //   iConfiguration (String Index)
    0xA0,        //   bmAttributes Remote Wakeup
    0x64,        //   bMaxPower 200mA

/* -------------------------------- */
//  Keyboard
    0x09,        //   bLength
    0x04,        //   bDescriptorType (Interface)
    0x00,        //   bInterfaceNumber 0
    0x00,        //   bAlternateSetting
    0x01,        //   bNumEndpoints 1
    0x03,        //   bInterfaceClass
    0x01,        //   bInterfaceSubClass
    0x01,        //   bInterfaceProtocol
    0x00,        //   iInterface (String Index)

    0x09,        //   bLength
    0x21,        //   bDescriptorType (HID)
    0x11, 0x01,  //   bcdHID 1.11
    0x00,        //   bCountryCode
    0x01,        //   bNumDescriptors
    0x22,        //   bDescriptorType[0] (HID)
    0x3E, 0x00,  //   wDescriptorLength[0] 62

    0x07,        //   bLength
    0x05,        //   bDescriptorType (Endpoint)
    0x81,        //   bEndpointAddress (IN/D2H)
    0x03,        //   bmAttributes (Interrupt)
    0x08, 0x00,  //   wMaxPacketSize 8
    0x0A,        //   bInterval 10 (unit depends on device speed)

/* -------------------------------- */
//  Mouse
    0x09,        //   bLength
    0x04,        //   bDescriptorType (Interface)
    0x01,        //   bInterfaceNumber 1
    0x00,        //   bAlternateSetting
    0x01,        //   bNumEndpoints 1
    0x03,        //   bInterfaceClass
    0x01,        //   bInterfaceSubClass
    0x02,        //   bInterfaceProtocol
    0x00,        //   iInterface (String Index)

    0x09,        //   bLength
    0x21,        //   bDescriptorType (HID)
    0x10, 0x01,  //   bcdHID 1.10
    0x00,        //   bCountryCode
    0x01,        //   bNumDescriptors
    0x22,        //   bDescriptorType[0] (HID)
    0x34, 0x00,  //   wDescriptorLength[0] 52

    0x07,        //   bLength
    0x05,        //   bDescriptorType (Endpoint)
    0x82,        //   bEndpointAddress (IN/D2H)
    0x03,        //   bmAttributes (Interrupt)
    0x04, 0x00,  //   wMaxPacketSize 4
    0x0A,        //   bInterval 10 (unit depends on device speed)

/* -------------------------------- */
//  Vendor defined HID
    0x09,        //   bLength
    0x04,        //   bDescriptorType (Interface)
    0x02,        //   bInterfaceNumber 2
    0x00,        //   bAlternateSetting
    0x02,        //   bNumEndpoints 2
    0xFF,        //   bInterfaceClass
    0x80,        //   bInterfaceSubClass
    0x55,        //   bInterfaceProtocol
    0x00,        //   iInterface (String Index)

    0x07,        // bLength
    0x05,        // bDescriptorType (Endpoint)
    0x83,        // bEndpointAddress (IN/D2H)
    0x02,        // bmAttributes (Bulk)
    0x40, 0x00,  // wMaxPacketSize 64
    0x00,        // bInterval 0 (unit depends on device speed)

    0x07,        // bLength
    0x05,        // bDescriptorType (Endpoint)
    0x04,        // bEndpointAddress (OUT/H2D)
    0x03,        // bmAttributes (Interrupt)
    0x40, 0x00,  // wMaxPacketSize 64
    0x00,        // bInterval 0 (unit depends on device speed)
};

/*HID类报表描述符*/
const uint8_c KeyRepDesc[] =
{
    0x05, 0x01,        // Usage Page (Generic Desktop Ctrls)
    0x09, 0x06,        // Usage (Keyboard)
    0xA1, 0x01,        // Collection (Application)
    0x05, 0x07,        //   Usage Page (Kbrd/Keypad)
    0x19, 0xE0,        //   Usage Minimum (0xE0)
    0x29, 0xE7,        //   Usage Maximum (0xE7)
    0x15, 0x00,        //   Logical Minimum (0)
    0x25, 0x01,        //   Logical Maximum (1)
    0x75, 0x01,        //   Report Size (1)
    0x95, 0x08,        //   Report Count (8)
    0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x95, 0x01,        //   Report Count (1)
    0x75, 0x08,        //   Report Size (8)
    0x81, 0x01,        //   Input (Const,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x95, 0x03,        //   Report Count (3)
    0x75, 0x01,        //   Report Size (1)
    0x05, 0x08,        //   Usage Page (LEDs)
    0x19, 0x01,        //   Usage Minimum (Num Lock)
    0x29, 0x03,        //   Usage Maximum (Scroll Lock)
    0x91, 0x02,        //   Output (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
    0x95, 0x05,        //   Report Count (5)
    0x75, 0x01,        //   Report Size (1)
    0x91, 0x01,        //   Output (Const,Array,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
    0x95, 0x06,        //   Report Count (6)
    0x75, 0x08,        //   Report Size (8)
    0x26, 0xFF, 0x00,  //   Logical Maximum (255)
    0x05, 0x07,        //   Usage Page (Kbrd/Keypad)
    0x19, 0x00,        //   Usage Minimum (0x00)
    0x29, 0x91,        //   Usage Maximum (0x91)
    0x81, 0x00,        //   Input (Data,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0xC0,              // End Collection
};
const uint8_c MouseRepDesc[] =
{
    0x05, 0x01,        // Usage Page (Generic Desktop Ctrls)
    0x09, 0x02,        // Usage (Mouse)
    0xA1, 0x01,        // Collection (Application)
    0x09, 0x01,        //   Usage (Pointer)
    0xA1, 0x00,        //   Collection (Physical)
    0x05, 0x09,        //     Usage Page (Button)
    0x19, 0x01,        //     Usage Minimum (0x01)
    0x29, 0x03,        //     Usage Maximum (0x03)
    0x15, 0x00,        //     Logical Minimum (0)
    0x25, 0x01,        //     Logical Maximum (1)
    0x75, 0x01,        //     Report Size (1)
    0x95, 0x03,        //     Report Count (3)
    0x81, 0x02,        //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x75, 0x05,        //     Report Size (5)
    0x95, 0x01,        //     Report Count (1)
    0x81, 0x01,        //     Input (Const,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x05, 0x01,        //     Usage Page (Generic Desktop Ctrls)
    0x09, 0x30,        //     Usage (X)
    0x09, 0x31,        //     Usage (Y)
    0x09, 0x38,        //     Usage (Wheel)
    0x15, 0x81,        //     Logical Minimum (-127)
    0x25, 0x7F,        //     Logical Maximum (127)
    0x75, 0x08,        //     Report Size (8)
    0x95, 0x03,        //     Report Count (3)
    0x81, 0x06,        //     Input (Data,Var,Rel,No Wrap,Linear,Preferred State,No Null Position)
    0xC0,              //   End Collection
    0xC0,              // End Collection
};
/*鼠标数据*/
uint8_t HIDMouse[] = { 0x0, 0x0, 0x0, 0x0 };
/*键盘数据*/
uint8_t HIDKey[] = { 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 };

uint8_t	usbLangDesc[] = { 0x04, 0x03, 0x09, 0x04 };
uint8_t	usbManuDesc[] = { 0x0A, 0x03, 'N', 0, 'S', 0, 'D', 0, 'N', 0 };
uint8_t	usbProdDesc[] = { 0x0E, 0x03, 'S', 0, 'i', 0, 'm', 0, 'P', 0, 'a', 0, 'd', 0 };

uint8_x __at (0x0000) Ep0Buffer[8  > (THIS_ENDP0_SIZE + 2) ? 8  : (THIS_ENDP0_SIZE + 2)];   //端点0 OUT&IN缓冲区，必须是偶地址
uint8_x __at (0x000A) Ep1Buffer[64 > (MAX_PACKET_SIZE + 2) ? 64 : (MAX_PACKET_SIZE + 2)];   //端点1 IN缓冲区,必须是偶地址
uint8_x __at (0x004C) Ep2Buffer[64 > (MAX_PACKET_SIZE + 2) ? 64 : (MAX_PACKET_SIZE + 2)];   //端点2 IN缓冲区,必须是偶地址
uint8_x __at (0x008E) Ep3Buffer[2 * MAX_PACKET_SIZE];                                       //端点3 OUT&IN缓冲区,必须是偶地址
uint8_t             SetupReqCode, SetupLen, Ready, Count, FLAG, UsbConfig;
uint8_t*            pDescr;                                                                 //USB配置标志
USB_SETUP_REQ       SetupReqBuf;                                                            //暂存Setup包 
#define UsbSetupBuf ((PUSB_SETUP_REQ)Ep0Buffer)

void __usbDeviceInterrupt() __interrupt (INT_NO_USB) __using (1) {
    uint8_t i, len;
	if (UIF_TRANSFER) {                                                                     // USB传输完成
		switch (USB_INT_ST & (MASK_UIS_TOKEN | MASK_UIS_ENDP)) {                            // 分析操作令牌和端点号
				case UIS_TOKEN_OUT | 3:                                                     // endpoint 3# 中断端点下传
					if (U_TOG_OK) {                                                         // 不同步的数据包将丢弃
						len = USB_RX_LEN;
						for (i = 0; i < len; i++) {
							Ep3Buffer[MAX_PACKET_SIZE + i] = Ep3Buffer[i] ^ 0xFF;           // OUT数据取反到IN由计算机验证
						}
						UEP3_T_LEN = len;
						UEP3_CTRL = UEP3_CTRL & ~ MASK_UEP_T_RES | UEP_T_RES_ACK;           // 允许上传
					}
					break;
				case UIS_TOKEN_IN | 3:                                                      // endpoint 3# 批量端点上传
					UEP3_CTRL = UEP3_CTRL & ~ MASK_UEP_T_RES | UEP_T_RES_NAK;               // 暂停上传
					break;
                case UIS_TOKEN_IN | 2:                                                      //endpoint 2# 中断端点上传
                    UEP2_T_LEN = 0;                                                         //预使用发送长度一定要清空
                    UEP2_CTRL = UEP2_CTRL & ~ MASK_UEP_T_RES | UEP_T_RES_NAK;               //默认应答NAK
                    break;
                case UIS_TOKEN_IN | 1:                                                      //endpoint 1# 中断端点上传
                    UEP1_T_LEN = 0;                                                         //预使用发送长度一定要清空
                    UEP1_CTRL = UEP1_CTRL & ~ MASK_UEP_T_RES | UEP_T_RES_NAK;               //默认应答NAK
                    FLAG = 1;                                                               /*传输完成标志*/
                    break;
				case UIS_TOKEN_SETUP | 0:                                                   // endpoint 0# SETUP
					len = USB_RX_LEN;
					if (len == sizeof(USB_SETUP_REQ)) {                                 // SETUP包长度
						SetupLen = UsbSetupBuf->wLengthL;
						if (UsbSetupBuf->wLengthH || SetupLen > 0x7F) SetupLen = 0x7F;      // 限制总长度
						len = 0;                                                            // 默认为成功并且上传0长度
						SetupReqCode = UsbSetupBuf->bRequest;				
						if ((UsbSetupBuf->bRequestType & USB_REQ_TYP_MASK) != USB_REQ_TYP_STANDARD) { /* 非标准请求 */
                            len = 0xFF;					
						} else {                                                            // 标准请求
							switch (SetupReqCode) {                                          // 请求码
								case USB_GET_DESCRIPTOR:
									switch(UsbSetupBuf->wValueH) {
										case 1:                                             // 设备描述符
											pDescr = (uint8_t*) (&usbDevDesc[0]);
											len = sizeof(usbDevDesc);
											break;
										case 2:                                             // 配置描述符
											pDescr = (uint8_t*) (&usbCfgDesc[0]);
											len = sizeof(usbCfgDesc);
											break;
										case 3:                                             // 字符串描述符
											switch(UsbSetupBuf->wValueL) {
												case 1:
													pDescr = (uint8_t*) (&usbManuDesc[0]);
													len = sizeof(usbManuDesc);
													break;
												case 2:
													pDescr = (uint8_t*) (&usbProdDesc[0]);
													len = sizeof(usbProdDesc);
													break;
												case 0:
													pDescr = (uint8_t*) (&usbLangDesc[0]);
													len = sizeof(usbLangDesc);
													break;
												default:
													len = 0xFF;                             // 不支持的字符串描述符
													break;
											}
											break;
                                        case 0x22:                                          //报表描述符
                                            if (UsbSetupBuf->wIndexL == 0) {                //接口0报表描述符
                                                pDescr = KeyRepDesc;                        //数据准备上传
                                                len = sizeof(KeyRepDesc);
                                            } else if (UsbSetupBuf->wIndexL == 1) {         //接口1报表描述符
                                                pDescr = MouseRepDesc;                      //数据准备上传
                                                len = sizeof(MouseRepDesc);                                
                                            } else {
                                                len = 0xff;                                 //本程序只有2个接口，这句话正常不可能执行
                                            }
                                            break;
										default:
											len = 0xFF;                                     // 不支持的描述符类型
											break;
									}
									if (SetupLen > len) SetupLen = len;                     // 限制总长度
									len = SetupLen >= THIS_ENDP0_SIZE ? THIS_ENDP0_SIZE : SetupLen;  // 本次传输长度
									memcpy(Ep0Buffer, pDescr, len);                         /* 加载上传数据 */
									SetupLen -= len;
									pDescr += len;
									break;
								case USB_SET_ADDRESS:
									SetupLen = UsbSetupBuf->wValueL;                        // 暂存USB设备地址
									break;
								case USB_GET_CONFIGURATION:
									Ep0Buffer[0] = UsbConfig;
									if (SetupLen >= 1) len = 1;
									break;
								case USB_SET_CONFIGURATION:
									UsbConfig = UsbSetupBuf->wValueL;
									break;
								case USB_CLEAR_FEATURE:
									if ((UsbSetupBuf->bRequestType & USB_REQ_RECIP_MASK) == USB_REQ_RECIP_ENDP) {  // 端点
										switch (UsbSetupBuf->wIndexL) {
											case 0x81:
												UEP1_CTRL = UEP1_CTRL & ~ ( bUEP_T_TOG | MASK_UEP_T_RES ) | UEP_T_RES_NAK;
												break;
											case 0x82:
												UEP2_CTRL = UEP2_CTRL & ~ ( bUEP_T_TOG | MASK_UEP_T_RES ) | UEP_R_RES_ACK;
												break;
											case 0x83:
												UEP3_CTRL = UEP3_CTRL & ~ ( bUEP_T_TOG | MASK_UEP_T_RES ) | UEP_T_RES_NAK;
												break;
											case 0x01:
												UEP1_CTRL = UEP1_CTRL & ~ ( bUEP_R_TOG | MASK_UEP_R_RES ) | UEP_R_RES_ACK;
												break;
                                            case 0x04:
												UEP3_CTRL = UEP3_CTRL & ~ ( bUEP_R_TOG | MASK_UEP_R_RES ) | UEP_R_RES_ACK;
												break;
											default:
                                            len = 0xFF;                                                             // 不支持的端点
												break;
										}
									}
									if ((UsbSetupBuf->bRequestType & USB_REQ_RECIP_MASK) == USB_REQ_RECIP_DEVICE) { // 设备
                                        break;
                                    } else {
                                        len = 0xFF;                                                                 // 不是端点不支持
                                    }
									break;
                                case USB_SET_FEATURE:                                                               /* Set Feature */
                                    if ((UsbSetupBuf->bRequestType & 0x1F) == 0x00) {                               /* 设置设备 */
                                        if ((((uint16_t) UsbSetupBuf->wValueH << 8) | UsbSetupBuf->wValueL) == 0x01)  {
                                            if (usbCfgDesc[7] & 0x20) {
                                                /* 设置唤醒使能标志 */
                                            } else {
                                                len = 0xFF;                                                         /* 操作失败 */
                                            }
                                        } else {
                                            len = 0xFF;                                                             /* 操作失败 */
                                        }
                                    } else if ((UsbSetupBuf->bRequestType & 0x1F) == 0x02) {                        /* 设置端点 */
                                        if ((((uint16_t) UsbSetupBuf->wValueH << 8) | UsbSetupBuf->wValueL ) == 0x00) {
                                            switch (((uint16_t) UsbSetupBuf->wIndexH << 8 ) | UsbSetupBuf->wIndexL) {
                                            case 0x82:
                                                UEP2_CTRL = UEP2_CTRL & (~bUEP_T_TOG) | UEP_T_RES_STALL;            /* 设置端点2 IN STALL */
                                                break;
                                            case 0x02:
                                                UEP2_CTRL = UEP2_CTRL & (~bUEP_R_TOG) | UEP_R_RES_STALL;            /* 设置端点2 OUT Stall */
                                                break;
                                            case 0x81:
                                                UEP1_CTRL = UEP1_CTRL & (~bUEP_T_TOG) | UEP_T_RES_STALL;            /* 设置端点1 IN STALL */
                                                break;
                                            default:
                                                len = 0xFF;                                                         //操作失败
                                                break;
                                            }
                                        } else {
                                            len = 0xFF;                                                             //操作失败
                                        }
                                    } else {
                                        len = 0xFF;                                                                 //操作失败
                                    }
                                    break;
								case USB_GET_INTERFACE:
									Ep0Buffer[0] = 0x00;
									if ( SetupLen >= 1 ) len = 1;
									break;
								case USB_GET_STATUS:
									Ep0Buffer[0] = 0x00;
									Ep0Buffer[1] = 0x00;
									if ( SetupLen >= 2 ) len = 2;
									else len = SetupLen;
									break;
								default:
									len = 0xFF;                                                                     // 操作失败							
									break;
							}
						}
					} else {
						len = 0xFF;                                                                                 // SETUP包长度错误				
					}
					if (len == 0xFF) {                                                                              // 操作失败
						SetupReqCode = 0xFF;
						UEP0_CTRL = bUEP_R_TOG | bUEP_T_TOG | UEP_R_RES_STALL | UEP_T_RES_STALL;                    // STALL
					} else if (len <= THIS_ENDP0_SIZE) {                                                            // 上传数据或者状态阶段返回0长度包
						UEP0_T_LEN = len;
						UEP0_CTRL = bUEP_R_TOG | bUEP_T_TOG | UEP_R_RES_ACK | UEP_T_RES_ACK;                        // 默认数据包是DATA1
					} else {                                                                                        // 下传数据或其它
						UEP0_T_LEN = 0;                                                                             // 虽然尚未到状态阶段，但是提前预置上传0长度数据包以防主机提前进入状态阶段
						UEP0_CTRL = bUEP_R_TOG | bUEP_T_TOG | UEP_R_RES_ACK | UEP_T_RES_ACK;                        // 默认数据包是DATA1
					}
					break;
				case UIS_TOKEN_IN | 0:                                                                              // endpoint 0# IN
					switch (SetupReqCode) {
						case USB_GET_DESCRIPTOR:
							len = SetupLen >= THIS_ENDP0_SIZE ? THIS_ENDP0_SIZE : SetupLen;                         // 本次传输长度
							memcpy(Ep0Buffer, pDescr, len);                                                         /* 加载上传数据 */
							SetupLen -= len;
							pDescr += len;
							UEP0_T_LEN = len;
							UEP0_CTRL ^= bUEP_T_TOG;                                                                // 翻转
							break;
						case USB_SET_ADDRESS:
							USB_DEV_AD = USB_DEV_AD & bUDA_GP_BIT | SetupLen;
							UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
							break;
						default:
							UEP0_T_LEN = 0;                                                                         // 状态阶段完成中断或者是强制上传0长度数据包结束控制传输
							UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
							break;
					}
					break;
				case UIS_TOKEN_OUT | 0:                                                                             // endpoint 0# OUT
					switch (SetupReqCode) {
                        case 0x09:
                            if (Ep0Buffer[0])  {
                                //printf("Light on Num Lock LED!\n");
                            } else if (Ep0Buffer[0] == 0) {
                                //printf("Light off Num Lock LED!\n");
                            }	
                            break;
						default:
							UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;                                              // 准备下一控制传输
							break;
					}
                    UEP0_CTRL ^= bUEP_R_TOG;                                                                        //同步标志位翻转
					break;
				default:		
					break;
			}
		UIF_TRANSFER = 0;                                                                                           // 清中断标志
	} else if (UIF_BUS_RST) {                                                                                       // USB总线复位
		UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
		UEP1_CTRL = bUEP_AUTO_TOG | UEP_R_RES_ACK;
		UEP2_CTRL = bUEP_AUTO_TOG | UEP_R_RES_ACK | UEP_T_RES_NAK;
        UEP3_CTRL = bUEP_AUTO_TOG | UEP_R_RES_ACK | UEP_T_RES_NAK;
		USB_DEV_AD = 0x00;
		UIF_SUSPEND = 0;
		UIF_TRANSFER = 0;
		UIF_BUS_RST = 0;                                                                                            // 清中断标志
	} else if (UIF_SUSPEND) {                                                                                       // USB总线挂起/唤醒完成
		UIF_SUSPEND = 0;
		if (USB_MIS_ST & bUMS_SUSPEND) {                                                                          // 挂起		
			while (XBUS_AUX & bUART0_TX);                                                                         // 等待发送完成
			SAFE_MOD = 0x55;
			SAFE_MOD = 0xAA;
			WAKE_CTRL = bWAK_BY_USB | bWAK_RXD0_LO;                                                                 // USB或者RXD0有信号时可被唤醒
			PCON |= PD;                                                                                             // 睡眠
			SAFE_MOD = 0x55;
			SAFE_MOD = 0xAA;
			WAKE_CTRL = 0x00;
		} else {                                                                                                    // 唤醒

		}
	} else {                                                                                                        // 意外的中断,不可能发生的情况
		USB_INT_FG = 0xFF;                                                                                          // 清中断标志
	}
}

void usbDevInit() {
	IE_USB = 0;
	USB_CTRL = 0x00;                                                           // 先设定USB设备模式
	UDEV_CTRL = bUD_PD_DIS;                                                    // 禁止DP/DM下拉电阻
    UDEV_CTRL &= ~bUD_LOW_SPEED;                                               //选择全速12M模式，默认方式
    USB_CTRL &= ~bUC_LOW_SPEED;
	
    UEP0_DMA = 0x0000;                                                  //端点0数据传输地址
    UEP4_1_MOD &= ~(bUEP1_RX_EN | bUEP1_TX_EN);                                //端点0单64字节收发缓冲区
    UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;                                 //OUT事务返回ACK，IN事务返回NAK
    UEP1_DMA = 0x000A;                                                  //端点1数据传输地址
    UEP4_1_MOD = UEP4_1_MOD & ~bUEP1_BUF_MOD | bUEP1_TX_EN;                    //端点1发送使能 64字节缓冲区
    UEP1_CTRL = bUEP_AUTO_TOG | UEP_T_RES_NAK;                                 //端点1自动翻转同步标志位，IN事务返回NAK	
    UEP2_DMA = 0x004C;                                                  //端点2数据传输地址
    UEP2_3_MOD = UEP2_3_MOD & ~bUEP2_BUF_MOD | bUEP2_TX_EN;                    //端点2发送使能 64字节缓冲区
    UEP2_CTRL = bUEP_AUTO_TOG | UEP_T_RES_NAK;                                 //端点2自动翻转同步标志位，IN事务返回NAK   
    UEP3_DMA = 0x008E;                                                  //端点3数据传输地址
    UEP4_1_MOD = UEP4_1_MOD & ~bUEP3_BUF_MOD | bUEP3_TX_EN;                    //端点3发送使能 128字节缓冲区
    UEP3_CTRL = bUEP_AUTO_TOG | UEP_R_RES_ACK | UEP_T_RES_NAK;                 //端点3自动翻转同步标志位，IN事务返回NAK

	USB_DEV_AD = 0x00;
	USB_CTRL |= bUC_DEV_PU_EN | bUC_INT_BUSY | bUC_DMA_EN;                      // 启动USB设备及DMA，在中断期间中断标志未清除前自动返回NAK
	UDEV_CTRL |= bUD_PORT_EN;                                                   // 允许USB端口
	USB_INT_FG = 0xFF;                                                          // 清中断标志
	USB_INT_EN = bUIE_SUSPEND | bUIE_TRANSFER | bUIE_BUS_RST;
	IE_USB = 1;
}

void Enp1IntIn() {
    memcpy(Ep1Buffer, HIDKey, sizeof(HIDKey));                                  //加载上传数据
    UEP1_T_LEN = sizeof(HIDKey);                                                //上传数据长度
    UEP1_CTRL = UEP1_CTRL & ~ MASK_UEP_T_RES | UEP_T_RES_ACK;                   //有数据时上传数据并应答ACK
}

void Enp2IntIn( ) {
    memcpy(Ep2Buffer, HIDMouse, sizeof(HIDMouse));                              //加载上传数据
    UEP2_T_LEN = sizeof(HIDMouse);                                              //上传数据长度
    UEP2_CTRL = UEP2_CTRL & ~ MASK_UEP_T_RES | UEP_T_RES_ACK;                   //有数据时上传数据并应答ACK
}

void usbHIDTestSend(uint8_t i) {
    switch(i) {
//鼠标数据上传示例
    case 'L':                                                        //左键
        HIDMouse[0] = 0x01;
        Enp2IntIn();
        HIDMouse[0] = 0;
        break;
    case 'R':                                                        //右键
        HIDMouse[0] = 0x02;
        Enp2IntIn();
        HIDMouse[0] = 0;
        break;
//键盘数据上传示例
    case 'A':                                                         //A键
        FLAG = 0;
        HIDKey[2] = 0x1d;                                             //按键开始
        Enp1IntIn();
        HIDKey[2] = 0;                                                //按键结束
        while(FLAG == 0)
        {
            ;    /*等待上一包传输完成*/
        }
        Enp1IntIn();
        break;
    case 'P':                                                         //P键
        FLAG = 0;
        HIDKey[2] = 0x38;
        Enp1IntIn();
        HIDKey[2] = 0;                                                //按键结束
        while(FLAG == 0)
        {
            ;    /*等待上一包传输完成*/
        }
        Enp1IntIn();
        break;
    case 'Q':                                                         //Num Lock键
        FLAG = 0;
        HIDKey[2] = 0x39;
        Enp1IntIn();
        HIDKey[2] = 0;                                                //按键结束
        while(FLAG == 0)
        {
            ;    /*等待上一包传输完成*/
        }
        Enp1IntIn();
        break;
    default:                                                          //其他
        UEP1_CTRL = UEP1_CTRL & ~ MASK_UEP_T_RES | UEP_T_RES_NAK;     //默认应答NAK
        UEP2_CTRL = UEP2_CTRL & ~ MASK_UEP_T_RES | UEP_T_RES_NAK;     //默认应答NAK
        break;
    }
}
