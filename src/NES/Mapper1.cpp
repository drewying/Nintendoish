#include "Mapper1.h"
#include <stdlib.h> 
#include <stdio.h>

using namespace NES;

uint8_t Mapper1::getTileData(uint16_t index) {
    return cartridge.chr[index];
}

void Mapper1::setTileData(uint16_t index, uint8_t value) {
    cartridge.chr[index] = value;
}

uint8_t Mapper1::getProgramData(uint16_t index) {
    if (cartridge.prgSize == 1 && index > 0xC000) {
        index -= 0x4000;
    }
    return cartridge.prg[index - 0x8000];
}

void Mapper1::setProgramData(uint16_t index, uint8_t value) {
    if ((value & 0x80) == 0x80) {
        // Clear load register to initial state
        loadRegister = 0x10;
    } 
    else {
        bool copyData = ((loadRegister & 0x1) == 0x1); //Shift register is full. Copy tp control.
        //Shift load register.
        loadRegister >>= 1;
        loadRegister |= (value & 0x1) << 5;
    }
}
