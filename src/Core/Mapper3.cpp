#include "Mapper3.h"
#include <stdlib.h> 
#include <stdio.h>

using namespace NES;

Mapper3::Mapper3(Cartridge &cartridge) : Mapper(cartridge) { }

uint8_t Mapper3::getTileData(uint16_t index) {
    return cartridge.chr[(chrOffset0 * 0x2000) + index];
}

void Mapper3::setTileData(uint16_t index, uint8_t value) {
    cartridge.chr[(chrOffset0 * 0x2000) + index] = value;
}

uint8_t Mapper3::getProgramData(uint16_t index) {
    if (index < 0x8000) {
        return 0x0;
    }
    return cartridge.prg[index - 0x8000];
}

void Mapper3::setProgramData(uint16_t index, uint8_t value) {
    if (index < 0x8000) {
        return;
    }
    chrOffset0 = value & 0x3;
}
