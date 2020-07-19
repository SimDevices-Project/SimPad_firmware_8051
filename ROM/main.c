#include "ch552.h"
#include "sys.h"
#include "usb.h"

extern uint8_t FLAG, Ready;

void main() {
    sysClockConfig();
    delay(5);

    usbDevInit();
    EA = 1;
    UEP1_T_LEN = 0;
    UEP2_T_LEN = 0;
    UEP3_T_LEN = 0;
    
    FLAG = 0;
    Ready = 0;

    while (1) {
        if (Ready) {
            usbHIDTestSend('Q');
            delay(500);
            usbHIDTestSend('A');
            delay(500);
            usbHIDTestSend('Q');
            delay(500);
        }
        delay(100);
    }
    
}
