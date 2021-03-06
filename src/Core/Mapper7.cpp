#include "Mapper7.h"
#include <stdlib.h> 
#include <stdio.h>

using namespace NES;

Mapper7::Mapper7(Cartridge &cartridge) : Mapper(cartridge) {
    cartridge.currentMirroring = Cartridge::SingleScreenA;
}

uint8_t Mapper7::getTileData(uint16_t index) {
    return cartridge.chr[index];
}

void Mapper7::setTileData(uint16_t index, uint8_t value) {
    cartridge.chr[index] = value;
}

uint8_t Mapper7::getProgramData(uint16_t index) {
    return cartridge.prg[(prgOffset0 * 0x8000) + (index - 0x8000)];
}

void Mapper7::setProgramData(uint16_t index, uint8_t value) {
    if (index < 0x8000) {
        return;
    }
    
    if ((value & 0x10) == 0x0) {
        cartridge.currentMirroring = Cartridge::SingleScreenA;
    } else {
        cartridge.currentMirroring = Cartridge::SingleScreenB;
    }
    prgOffset0 = value & 0x7;
}
