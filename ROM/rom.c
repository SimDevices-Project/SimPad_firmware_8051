#include "rom.h"

#include "bsp.h"

#if defined(HAS_ROM)
void __eeprom_reset() {
    for (uint8_t i = 0; i < 9; i++) {
        ROM_SCL = 1;
        if (ROM_SDA == 1) break;
        ROM_SCL = 0;
    }
}

void __eeprom_start() {
    ROM_SCL = 1;
    ROM_SDA = 1;
    ROM_SDA = 0;
}

void __eeprom_stop() {
    ROM_SCL = 1;
    ROM_SDA = 0;
    ROM_SDA = 1;
}

void __eeprom_ack() {
    ROM_SCL = 0;
    ROM_SCL = 1;
    while (ROM_SDA == 1);
    ROM_SCL = 0;
}

void __eeprom_nak() {
    ROM_SCL = 0;
    ROM_SDA = 1;
    ROM_SCL = 1;
    ROM_SCL = 0;
}

void __eeprom_wr(uint8_t data) {
    ROM_SCL = 0;
    for (uint8_t i = 0; i < 8; i++) {
        ROM_SDA = (data << i) & 0x80;
        ROM_SCL = 1;
        ROM_SCL = 0;
    }
}

uint8_t __eeprom_rd() {
    uint8_t data = 0;
    ROM_SCL = 0;
    for (uint8_t i = 0; i < 8; i++) {
        ROM_SCL = 1;
        data |= ROM_SDA << (7 - i);
        ROM_SCL = 0;
    }
    return data;
}

void __eeprom_write(uint16_t addr, uint8_t data) {
    ROM_WP = 0;
    addr &= ROM_SIZE;
    uint8_t devAddr = 0xA0 | (((addr >> 8) & 0x07) << 1);
    uint8_t wordAddr = addr & 0xFF;
    __eeprom_start();
    __eeprom_wr(devAddr & 0xFE);
    __eeprom_ack();
    __eeprom_wr(wordAddr);
    __eeprom_ack();
    __eeprom_wr(data);
    __eeprom_ack();
    __eeprom_stop();
    ROM_WP = 1;
}

uint8_t __eeprom_read(uint16_t addr) {
    addr &= (ROM_SIZE - 1);
    uint8_t devAddr = 0xA0 | (((addr >> 8) & 0x07) << 1);
    uint8_t wordAddr = addr & 0xFF;
    uint8_t data = 0;
    __eeprom_start();
    __eeprom_wr(devAddr & 0xFE);
    __eeprom_ack();
    __eeprom_wr(wordAddr);
    __eeprom_ack();
    __eeprom_start();
    __eeprom_wr(devAddr | 0x01);
    __eeprom_ack();
    data = __eeprom_rd();
    __eeprom_nak();
    __eeprom_stop();
    return data;
}
#endif

void __flash_write(uint8_t addr, uint8_t data) {
    addr &= (FLASH_SIZE - 1);
    uint8_t offset = addr << 1;
    SAFE_MOD = 0x55;
    SAFE_MOD = 0xAA;
    GLOBAL_CFG |= bDATA_WE;
    ROM_ADDR = DATA_FLASH_ADDR + offset;
    ROM_DATA_L = data;
    ROM_CTRL = 0x9A;
    SAFE_MOD = 0x55;
    SAFE_MOD = 0xAA;
    GLOBAL_CFG &= ~bDATA_WE;
}

uint8_t __flash_read(uint8_t addr) {
    addr &= (FLASH_SIZE - 1);
    uint8_t offset = addr << 1;
    ROM_ADDR = DATA_FLASH_ADDR + offset;
    ROM_CTRL = 0x8E;
    return ROM_DATA_L;
}

void romInit() {
#if defined(HAS_ROM)
    ROM_WP = 0;
    __eeprom_reset();
    ROM_WP = 1;
#endif
}

uint8_t romRead8i(uint8_t addr) {
    return __flash_read(addr);
}

void romWrite8i(uint8_t addr, uint8_t data) {
    __flash_write(addr, data);
}

uint16_t romRead16i(uint8_t addr) {
    return ((uint16_t) __flash_read(addr)) | (((uint16_t) __flash_read(addr + 1)) << 8);
}

void romWrite16i(uint8_t addr, uint16_t data) {
    __flash_write(addr, data & 0xFF);
    __flash_write(addr + 1, (data >> 8) & 0xFF);
}

uint8_t romRead8e(uint16_t addr) {
#if defined(HAS_ROM)
    return __eeprom_read(addr);
#else
    return 0x00;
#endif
}

void romWrite8e(uint16_t addr, uint8_t data) {
#if defined(HAS_ROM)
    __eeprom_write(addr, data);
#else
    return;
#endif
}

uint16_t romRead16e(uint16_t addr) {
#if defined(HAS_ROM)
    return ((uint16_t) __eeprom_read(addr)) | (((uint16_t) __eeprom_read(addr + 1)) << 8);
#else
    return 0x0000;
#endif
}

void romWrite16e(uint16_t addr, uint16_t data) {
#if defined(HAS_ROM)
    __eeprom_write(addr, data & 0xFF);
    __eeprom_write(addr + 1, (data >> 8) & 0xFF);
#else
    return;
#endif
}
