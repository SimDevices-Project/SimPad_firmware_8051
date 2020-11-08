#ifndef __ROM_H
#define __ROM_H

#include "ch552.h"

void romInit();
uint8_t romRead8i(uint8_t addr);
void romWrite8i(uint8_t addr, uint8_t data);
uint16_t romRead16i(uint8_t addr);
void romWrite16i(uint8_t addr, uint16_t data);
uint8_t romRead8e(uint16_t addr);
void romWrite8e(uint16_t addr, uint8_t data);
uint16_t romRead16e(uint16_t addr);
void romWrite16e(uint16_t addr, uint16_t data);

#endif