#include "fir.h"

#include "sys.h"

static __xdata fir_type firBuffer[FIR_SIZE];
static __xdata uint8_t firResult[FIR_SIZE];

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

    BT1 = 1; BT2 = 1;
    delay_us(10);
    firBuffer[0] <<= 1; firBuffer[0] |= !BT1;
    firBuffer[1] <<= 1; firBuffer[1] |= !BT2;
#if (defined(SIMPAD_V2_AE) || defined(SIMPAD_V2))
    BT3 = 1; BT4 = 1; BT5 = 1;
    delay_us(10);
    firBuffer[2] <<= 1; firBuffer[2] |= !BT3;
    firBuffer[3] <<= 1; firBuffer[3] |= !BT4;
    firBuffer[4] <<= 1; firBuffer[4] |= !BT5;
#elif (defined(SIMPAD_NANO_AE) || defined(SIMPAD_NANO))
    BT3 = 1;
    delay_us(10);
    firBuffer[2] <<= 1; firBuffer[2] |= !BT3;
#endif
}

__bit fir(uint8_t i) {
    return (firResult[i] > sizeof(fir_type) * 4) &&
           (firBuffer[i] & 0x1 != 0);
}