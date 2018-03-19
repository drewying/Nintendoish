#include "Mapper4.h"
#include <stdlib.h> 
#include <stdio.h>

using namespace NES;

uint8_t Mapper4::getTileData(uint16_t index) {
    return cartridge.chr[index];
}

void Mapper4::setTileData(uint16_t index, uint8_t value) {
    cartridge.chr[index] = value;
}

uint8_t Mapper4::getProgramData(uint16_t index) {
    return cartridge.prg[index - 0x8000];
}

void Mapper4::setProgramData(uint16_t index, uint8_t value) {
    cartridge.prg[index - 0x8000] = value;
}
