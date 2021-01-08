#include "fir.h"

#include "sys.h"

static __xdata fir_type firBuffer[FIR_SIZE];
static __xdata uint8_t firResult[FIR_SIZE];
uint8_t i = 0;

#define __set_pin(k) do { BT ## k = 1; for (i = 0; i < 10; i++); } while (0)
#define __fifo_in(i, k) do { firBuffer[i] <<= 1; firBuffer[i] |= !BT ## k; } while (0)

void firUpdate() {
    fir_type buf = 0;
    for (uint8_t i = 0; i < FIR_SIZE; i++) {
        firResult[i] = 0;
        buf = firBuffer[i];
        while (buf != 0) {
            buf = buf & (buf - 1);
            firResult[i] += 1;
        }
    }

#if defined(SIM_KEY)
    __set_pin(1);
    __fifo_in(0, 1);
#else
    __set_pin(1);
    __set_pin(2);
    __fifo_in(0, 1);
    __fifo_in(1, 2);
#if (defined(SIMPAD_V2_AE) || defined(SIMPAD_V2))
    __set_pin(3);
    __set_pin(4);
    __set_pin(5);
    __fifo_in(2, 3);
    __fifo_in(3, 4);
    __fifo_in(4, 5);
#elif (defined(SIMPAD_NANO_AE) || defined(SIMPAD_NANO))
    __set_pin(3);
    __fifo_in(2, 3);
#endif
#endif
}

__bit fir(uint8_t i) {
    return (firResult[i] > sizeof(fir_type) * 4) &&
           (firBuffer[i] & 0x1 != 0);
}