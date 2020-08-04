#include "ch552.h"
#include "sys.h"
#include "usb.h"
#include "bsp.h"

#include "nsvm.h"

void __usbDeviceInterrupt() __interrupt (INT_NO_USB) __using (1);
extern uint8_t FLAG;

volatile __bit control = 0;
volatile uint8_t prevKey = 0;

void main() {
    sysClockConfig();
    delay(5);

    usbDevInit();
    EA = 1;
    UEP1_T_LEN = 0;
    UEP2_T_LEN = 0;
    UEP3_T_LEN = 0;

    FLAG = 1;

    delay(500);
    usbReleaseAll();
    usbPushKeydata();
    requestHIDData();

    while (1) {
        // Standard keyboard
        usbReleaseAll();
        usbSetKeycode(0, 1);                    // Report ID 1
        usbSetKeycode(1, 0);                    // NO CONTROL
        usbSetKeycode(9, 0);                    // NO ESCAPE

        uint8_t val = 0;
        BT1 = 1; BT2 = 1;
        delay_us(10);
        val |= (BT1 != 0 ? 0 : 0x01);
        val |= (BT2 != 0 ? 0 : 0x02);
    #if defined(SIMPAD_V2)
        usbSetKeycode(2, BT1 != 0 ? 0 : 13);    // KEY_J
        usbSetKeycode(3, BT2 != 0 ? 0 : 14);    // KEY_K
    #elif defined(SIMPAD_NANO)
        usbSetKeycode(2, BT1 != 0 ? 0 : 7);     // KEY_D
        usbSetKeycode(3, BT2 != 0 ? 0 : 9);     // KEY_F
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
    #if defined(SIMPAD_V2)
        BT3 = 1; BT4 = 1; BT5 = 1;
        delay_us(10);
             if (BT3 == 0) val = 0xB6;          // KEY_PREV
        else if (BT4 == 0) val = 0xCD;          // KEY_PLAY
        else if (BT5 == 0) val = 0xB5;          // KEY_NEXT
    #elif defined(SIMPAD_NANO)
        BT3 = 1;
        delay_us(10);
        if (BT3 == 0) val = 0xCD;               // KEY_PLAY
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
