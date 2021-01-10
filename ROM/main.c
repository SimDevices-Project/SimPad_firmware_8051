#include "ch552.h"
#include "sys.h"
#include "rom.h"
#include "usb.h"
#include "rgb.h"
#include "fir.h"
#include "bsp.h"

#include "cvm.h"

void __usbDeviceInterrupt() __interrupt (INT_NO_USB) __using (1);       // USB中断定义
extern uint8_t FLAG;

void __tim2Interrupt() __interrupt (INT_NO_TMR2) __using (2);           // TIM2中断定义

volatile __bit control = 0;
volatile uint8_t prevKey = 0;

volatile uint8_t runKey = 0xFF;
cvm_ret cvmWDTCallback() {                                              // 解释器看门狗回调定义
    if (runKey != 0xFF) {
        if ((runKey & 0x80) != 0) {
            if (fir(runKey & 0x7F) == 0)
                cvm_end();
        } else {
            if ((prevKey & (0x01 << runKey)) != (fir(runKey) << runKey))
                cvm_end();
        }
    }
    return CVM_RET_OK;
}

#ifdef PERFORMANCE_MODE
__bit __get_pin(uint8_t i) {
    switch (i) {
#if (KEY_COUNT >= 1)
    case 1:
        return BT1;
#endif
#if (KEY_COUNT >= 2)
    case 2:
        return BT2;
#endif
#if (KEY_COUNT >= 3)
    case 3:
        return BT3;
#endif
#if (KEY_COUNT >= 4)
    case 4:
        return BT4;
#endif
#if (KEY_COUNT == 5)
    case 5:
        return BT5;
#endif
    default:
        break;
    }
    return 0;
}

__bit __test(uint8_t v, uint8_t i) {
    return (v & (1 << i));
}

void main() {
    uint8_t i;
    sysClockConfig();
    delay(5);

    romInit();
    delay(500);

    sysLoadConfig();
    SysConfig* cfg = sysGetConfig();

    sysTickConfig();
    usbDevInit();
    rgbInit();
    EA = 1;

    for (i = 0; i < LED_COUNT; i++) {
        if (cfg->ledConfig[i].marco)
            cvm_run(cfg->ledConfig[i].program, cfg->ledConfig[i].length);
        else
            rgbSetLed(i, cfg->ledConfig[i].color);
    }
    rgbUpdate();
    rgbPush();

    delay(500);
    usbReleaseAll();
    usbPushKeydata();
    requestHIDData();

    cvm_wdt(&cvmWDTCallback);

    uint8_t state = 0, trig = 0;
    while (1) {
        firUpdate();

        uint8_t nowKey = 0;
        for (i = 0; i < KEY_COUNT; i++) {
            nowKey <<= 1;
            nowKey |= fir(i);
        }

        for (i = 0; i < KEY_COUNT; i++) {
            if (cfg->keyConfig[i].mode == KeyNone) {
                usbSetKeycode(0, 1);
                usbSetKeycode(1, 0);
                if (!cfg->keyConfig[i].marco && cfg->keyConfig[i].code < 0xE0) {
                    if (__test(state, i) != 0 && __get_pin(i) == 0) {
                        // negedge
                        usbSetKeycode(i + 2, cfg->keyConfig[i].code);
                        usbPushKeydata();
                    } else if (fir(i)) {
                        trig |= (1 << i);
                    } else if (!fir(i) && __test(trig, i) != 0) {
                        trig &= !(1 << i);
                        usbSetKeycode(i + 2, 0x00);
                    } 
                }
                if (prevKey != nowKey)
                    usbPushKeydata();
            }
        }

        state = 0;
        for (i = 0; i < KEY_COUNT; i++) {
            state <<= 1;
            state |= __get_pin(i);
        }

        if (hasHIDData()) {
            cvm_wdt(NULL);
            cvm_run(fetchHIDData(), HID_BUF);
            requestHIDData();
            cvm_wdt(&cvmWDTCallback);
        }

        if (prevKey != nowKey)
            prevKey = nowKey;
    }
    
}
#else
void main() {
    uint8_t i;
    sysClockConfig();
    delay(5);

    romInit();
    delay(500);

    sysLoadConfig();
    SysConfig* cfg = sysGetConfig();

    sysTickConfig();
    usbDevInit();
    rgbInit();
    EA = 1;

    for (i = 0; i < LED_COUNT; i++) {
        if (cfg->ledConfig[i].marco)
            cvm_run(cfg->ledConfig[i].program, cfg->ledConfig[i].length);
        else
            rgbSetLed(i, cfg->ledConfig[i].color);
    }

    delay(500);
    usbReleaseAll();
    usbPushKeydata();
    requestHIDData();

    cvm_wdt(&cvmWDTCallback);

    while (1) {
        firUpdate();
        rgbUpdate();
        rgbPush();

        uint8_t nowKey = 0;
        for (i = 0; i < KEY_COUNT; i++) {
            nowKey <<= 1;
            nowKey |= fir(i);
        }

        uint8_t ctrlKey = 0;
        __bit hasCtrlKey = 0;
        for (i = 0; i < KEY_COUNT; i++) {
            if (cfg->keyConfig[i].mode == KeyNone) {
                usbSetKeycode(0, 1);
                usbSetKeycode(1, 0);
                if (!cfg->keyConfig[i].marco) {
                    if (cfg->keyConfig[i].code >= 0xE0) {
                        hasCtrlKey = 1;
                        ctrlKey |= (fir(i) ? 0x01 << (cfg->keyConfig[i].code - 0xE0) : 0x00);
                    } else
                        usbSetKeycode(i + 2, fir(i) ? cfg->keyConfig[i].code : 0x00);
                }
                if (prevKey != nowKey)
                    usbPushKeydata();
            } else {
                usbReleaseAll();
                usbSetKeycode(0, 1);
                usbPushKeydata();
                usbSetKeycode(0, 2);
                usbPushKeydata();
                usbReleaseAll();
                if (cfg->keyConfig[i].marco) {
                    if (cfg->keyConfig[i].mode == KeyPress) {
                        if (fir(i)) {
                            runKey = i | 0x80;
                            cvm_run(cfg->keyConfig[i].program, cfg->keyConfig[i].length);
                            runKey = 0xFF;
                        }
                    } else {
                        __bit trig = 0;
                        if (cfg->keyConfig[i].mode == KeyDown)
                            trig = (prevKey & (0x01 << i)) == 0 && (nowKey & (0x01 << i)) != 0;
                        else
                            trig = (prevKey & (0x01 << i)) != 0 && (nowKey & (0x01 << i)) == 0;
                        if (trig) {
                            runKey = i;
                            cvm_run(cfg->keyConfig[i].program, cfg->keyConfig[i].length);
                            runKey = 0xFF;
                        }
                    }
                }
            }
        }

        if (prevKey != nowKey && hasCtrlKey) {
            usbSetKeycode(0, 1);
            usbSetKeycode(1, ctrlKey);
            usbPushKeydata();
        }

        if (hasHIDData()) {
            cvm_wdt(NULL);
            cvm_run(fetchHIDData(), HID_BUF);
            requestHIDData();
            cvm_wdt(&cvmWDTCallback);
        }

        if (prevKey != nowKey)
            prevKey = nowKey;
    }
    
}
#endif
