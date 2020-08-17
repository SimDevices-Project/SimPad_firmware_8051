#include "ch552.h"
#include "sys.h"
#include "usb.h"
#include "rgb.h"
#include "bsp.h"

#include "cvm.h"

void __usbDeviceInterrupt() __interrupt (INT_NO_USB) __using (1);
extern uint8_t FLAG;

void __rgbTim2Interrupt() __interrupt (INT_NO_TMR2) __using (2);

volatile __bit control = 0;
volatile uint8_t prevKey = 0;

typedef uint16_t fir_type;
#define FIR_SIZE 5
static __idata fir_type firBuffer[FIR_SIZE];
static __idata uint8_t firResult[FIR_SIZE];

void updateFir() {
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
#elif defined(SIMPAD_NANO)
    BT3 = 1;
    delay_us(10);
    firBuffer[2] <<= 1; firBuffer[2] |= !BT3;
#endif
}

__bit fir(uint8_t index) {
    return (firResult[index % FIR_SIZE] > sizeof(fir_type) * 4) &&
           (firBuffer[index % FIR_SIZE] & 0x1 != 0);
}

void main() {
    sysClockConfig();
    delay(5);

    usbDevInit();
    rgbInit();
    EA = 1;

    delay(500);
    usbReleaseAll();
    usbPushKeydata();
    requestHIDData();

    while (1) {
        updateFir();

        // Standard keyboard
        usbReleaseAll();
        usbSetKeycode(0, 1);                    // Report ID 1
        usbSetKeycode(1, 0);                    // NO CONTROL
        usbSetKeycode(9, 0);                    // NO ESCAPE

        uint8_t val = 0;
        val |= (fir(0) ? 0x01 : 0);
        val |= (fir(1) ? 0x02 : 0);
    #if (defined(SIMPAD_V2_AE) || defined(SIMPAD_V2))
        usbSetKeycode(2, fir(0) ? 13 : 0);      // KEY_J
        usbSetKeycode(3, fir(1) ? 14 : 0);      // KEY_K
    #elif defined(SIMPAD_NANO)
        usbSetKeycode(2, fir(0) ? 7 : 0);       // KEY_D
        usbSetKeycode(3, fir(1) ? 9 : 0);       // KEY_F
    #endif

        if (val != prevKey) {
            prevKey = val;
            usbPushKeydata();
        }
        
        // Multimedia keypad
        usbReleaseAll();
        usbSetKeycode(0, 2);                    // Report ID 2
        usbSetKeycode(2, 0);
        val = 0;
    #if (defined(SIMPAD_V2_AE) || defined(SIMPAD_V2))
             if (fir(2)) val = 0xB6;            // KEY_PREV
        else if (fir(3)) val = 0xCD;            // KEY_PLAY
        else if (fir(4)) val = 0xB5;            // KEY_NEXT
    #elif defined(SIMPAD_NANO)
        if (fir(2)) val = 0xCD;                 // KEY_PLAY
    #endif
        
        if (val > 0) {
            usbSetKeycode(1, val);
            usbPushKeydata();
            delay(100);
            usbSetKeycode(1, 0);
            usbPushKeydata();
            delay(100);
        }

        if (hasHIDData()) {
            for (uint8_t i = 0; i < 32; i++)
                setHIDData(i, getHIDData(i));
            pushHIDData();
            requestHIDData();
        }
    }
    
}
