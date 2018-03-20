#include "Mapper4.h"
#include <stdlib.h> 
#include <stdio.h>

using namespace NES;

uint8_t Mapper4::getTileData(uint16_t index) {
    if (index < 0x400) {
        return cartridge.chr[(chrOffset0 * 0x400) + index];
    } else if (index < 0x800) {
        return cartridge.chr[(chrOffset1 * 0x400) + (index - 0x400)];
    } else if (index < 0xC00) {
        return cartridge.chr[(chrOffset2 * 0x400) + (index - 0x800)];
    } else if (index < 0x1000) {
        return cartridge.chr[(chrOffset3 * 0x400) + (index - 0xC00)];
    } else if (index < 0x1400) {
        return cartridge.chr[(chrOffset4 * 0x400) + (index - 0x1000)];
    } else if (index < 0x1800) {
        return cartridge.chr[(chrOffset5 * 0x400) + (index - 0x1400)];
    } else if (index < 0x1C00) {
        return cartridge.chr[(chrOffset6 * 0x400) + (index - 0x1800)];
    } else {
        return cartridge.chr[(chrOffset7 * 0x400) + (index - 0x1C00)];
    }
    
    return 0x0;
}

void Mapper4::setTileData(uint16_t index, uint8_t value) {
    cartridge.chr[index] = value;
}

uint8_t Mapper4::getProgramData(uint16_t index) {
    if (index < 0x8000) {
        return prgRAM[index - 0x6000];
    } else if (index < 0xA000) {
        return cartridge.prg[(prgOffset0 * 0x2000) + (index - 0x8000)];
    } else if (index < 0xC000) {
        return cartridge.prg[(prgOffset1 * 0x2000) + (index - 0xA000)];
    } else if (index < 0xE000) {
        return cartridge.prg[(prgOffset2 * 0x2000) + (index - 0xC000)];
    } else {
        return cartridge.prg[(prgOffset3 * 0x2000) + (index - 0xE000)];
    }
    return 0x0;
}

void Mapper4::setProgramData(uint16_t index, uint8_t value) {
    if (index < 0x8000) {
        prgRAM[index - 0x6000] = value;
    } else if (index < 0xA000) {
        //Bank Select
    }
    cartridge.prg[index - 0x8000] = value;
}
