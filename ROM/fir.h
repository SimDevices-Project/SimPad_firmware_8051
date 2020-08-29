#ifndef __FIR_H
#define __FIR_H

#include "ch552.h"
#include "bsp.h"

typedef uint16_t fir_type;
#define FIR_SIZE KEY_COUNT

void firUpdate();
__bit fir(uint8_t i);

#endif