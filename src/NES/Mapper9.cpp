#include "Mapper9.h"
#include <stdlib.h> 
#include <stdio.h>

using namespace NES;

Mapper9::Mapper9(Cartridge &cartridge) : Mapper(cartridge) {
    prgOffset1 = (cartridge.prgSize * 2) - 3;
}

uint8_t Mapper9::getTileData(uint16_t index) {
    uint8_t value;
    if (index < 0x1000) {
        if (latch0 == false) {
            // latch0 == $FD
            value = cartridge.chr[(chrOffset0FD * 0x1000) + index];
        } else {
            // latch0 == $FE
            value = cartridge.chr[(chrOffset0FE * 0x1000) + index];
        }
    } else {
        if (latch1 == false) {
            // latch1 == $FD
            value = cartridge.chr[(chrOffset1FD * 0x1000) + (index - 0x1000)];
        } else {
            // latch1 == $FE
            value = cartridge.chr[(chrOffset1FE * 0x1000) + (index - 0x1000)];
        }
    }
    
    if (index == 0x0FD8) {
        latch0 = false;
    }
    if (index == 0x0FE8) {
        latch0 = true;
    }
    if (index >= 0x1FD8 && index <= 0x1FDF) latch1 = false;
    if (index >= 0x1FE8 && index <= 0x1FEF) latch1 = true;
    
    return value;
}

void Mapper9::setTileData(uint16_t index, uint8_t value) { }

uint8_t Mapper9::getProgramData(uint16_t index) {
    if (index < 0x8000) {
        return prgRam[index - 0x6000];
    } else if (index < 0xA000) {
        return cartridge.prg[(prgOffset0 * 0x2000) + (index - 0x8000)];
    } else {
        return cartridge.prg[(prgOffset1 * 0x2000) + (index - 0xA000)];
    }
}

void Mapper9::setProgramData(uint16_t index, uint8_t value) {
    if (index < 0x8000) {
        prgRam[index - 0x6000] = value;
    } else if (index < 0xA000) {
        return;
    } else if (index < 0xB000) {
        prgOffset0 = value & 0xF;
    } else if (index < 0xC000) {
        chrOffset0FD = value & 0x1F;
    } else if (index < 0xD000) {
        chrOffset0FE = value & 0x1F;
    } else if (index < 0xE000) {
        chrOffset1FD = value & 0x1F;
    } else if (index < 0xF000) {
        chrOffset1FE = value & 0x1F;
    } else {
        if ((value & 0x1) == 0x1) {
            cartridge.currentMirroring = Cartridge::HorizontalMirror;
        } else {
            cartridge.currentMirroring = Cartridge::VerticalMirror;
        }
    }
}
