#ifndef __RGB_H
#define __RGB_H

#include "ch552.h"

void rgbInit();
void rgbSet(uint8_t index, uint32_t value);
void rgbPush();

#endif