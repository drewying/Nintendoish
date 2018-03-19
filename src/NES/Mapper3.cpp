#include "Mapper3.h"
#include <stdlib.h> 
#include <stdio.h>

using namespace NES;

Mapper3::Mapper3(Cartridge &cartridge) : Mapper(cartridge) {
}

uint8_t Mapper3::getTileData(uint16_t index) {
    return cartridge.chr[index + (chrOffset0 * 0x2000)];
}

void Mapper3::setTileData(uint16_t index, uint8_t value) {
    cartridge.chr[index + (chrOffset0 * 0x2000)] = value;
}

uint8_t Mapper3::getProgramData(uint16_t index) {
    if (index >= 0x8000) return cartridge.prg[index - 0x8000];
    return 0x0;
}

void Mapper3::setProgramData(uint16_t index, uint8_t value) {
    if (index >= 0x8000) {
        chrOffset0 = value & 0x3;
    }
}
