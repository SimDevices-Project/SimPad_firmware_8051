#ifndef __USB_H
#define __USB_H

#include <stdint.h>

void usbDevInit();

void usbSetKeycode(uint8_t i, uint8_t key);
void usbReleaseAll();
void usbPushKeydata();

uint8_t getHIDData(uint8_t index);
void setHIDData(uint8_t index, uint8_t data);
__bit hasHIDData();
void requestHIDData();
void pushHIDData();


#endif
