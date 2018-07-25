#include "Mapper1.h"
#include <stdlib.h> 
#include <stdio.h>

using namespace NES;

Mapper1::Mapper1(Cartridge &cartridge) : Mapper(cartridge) {
    prgOffset0 = 0x0;
    prgOffset1 = cartridge.prgSize - 1;

    chrOffset0 = 0x0;
    chrOffset1 = 0x0;
}

uint8_t Mapper1::getTileData(uint16_t index) {
    if (index < 0x1000) {
        return cartridge.chr[index + (chrOffset0 * 0x1000)];
    } else {
        return cartridge.chr[(index - 0x1000) + (chrOffset1 * 0x1000)];
    }
}

void Mapper1::setTileData(uint16_t index, uint8_t value) {
    if (index < 0x1000) {
        cartridge.chr[index + (chrOffset0 * 0x1000)] = value;
    } else {
        cartridge.chr[(index - 0x1000) + (chrOffset1 * 0x1000)] = value;
    }
}

uint8_t Mapper1::getProgramData(uint16_t index) {
    if (index >= 0xC000) {
        return cartridge.prg[(index - 0xC000) + (prgOffset1 * 0x4000)]; // Calculation done inline to prevent 16 bit overflow
    } else if (index >= 0x8000) {
        return cartridge.prg[(index - 0x8000) + (prgOffset0 * 0x4000)]; // Calculation done inline to prevent 16 bit overflow
    } else if (index >= 0x6000) {
        return prgRAM[(index - 0x6000)];
    }
    return 0x0;
}

void Mapper1::setProgramData(uint16_t index, uint8_t value) {
    if (index < 0x8000) {
        prgRAM[(index - 0x6000)] = value;
        return;
    }

    if ((value & 0x80) == 0x80) {
        clearLoadRegister();
        controlRegister = controlRegister | 0xC;
        loadControlRegister();
        return;
    } 

    bool copyData = ((loadRegister & 0x1) == 0x1); //Shift register is full. Copy t0 control.
    
    //Shift load register.
    loadRegister >>= 1;
    loadRegister |= ((value & 0x1) << 4);
    if (copyData) { 
        if (index < 0xA000) {
            controlRegister = loadRegister;
            loadControlRegister();
        } else if (index < 0xC000) {
            //  Select 4 KB or 8 KB CHR bank at PPU $0000 (low bit ignored in 8 KB mode)
            chrValue0 = loadRegister;
        } else if (index < 0xE000) {
            // Select 4 KB CHR bank at PPU $1000 (ignored in 8 KB mode)
            chrValue1 = loadRegister;
        } else if (index <= 0xFFFF) {
            prgValue0 = loadRegister;
        }
        updateOffsets();
        clearLoadRegister();
    }
}

void Mapper1::updateOffsets() {    
    if (chrBankMode == 0x0) {
        chrOffset0 = chrValue0 & 0xE;
        chrOffset1 = chrValue0 | 0x1;
    } else if (chrBankMode == 0x1) {
        chrOffset0 = chrValue0;
        chrOffset1 = chrValue0 + 1;
    }

    /*
    if (chrBankMode == 0x1) {
        chrOffset0 = (loadRegister & 0x1);
    }
    ramOffset0 = ((loadRegister >> 0x2) & 0x3);
    prgOffset0 = (loadRegister >> 0x3) * 0x10;

    if (chrBankMode == 0x1) {
        chrOffset0 = (loadRegister & 0x1);
        ramOffset0 = ((loadRegister >> 0x2) & 0x3);
        prgOffset0 = (loadRegister >> 0x3) * 0x10;
    }
    */


    if (prgBankMode == 0x0 || prgBankMode == 0x1) {
        // 0, 1: switch 32 KB at $8000, ignoring low bit of bank number;
        prgOffset0 = prgValue0 & 0xE;
        prgOffset1 = prgValue0 | 0x1;
    }
    else if (prgBankMode == 0x2) {
        // 2: fix first bank at $8000 and switch 16 KB bank at $C000;
        prgOffset0 = 0x0;
        prgOffset1 = prgValue0 & 0xF;
    }
    else if (prgBankMode == 0x3) {
        // 3: fix last bank at $C000 and switch 16 KB bank at $8000
        prgOffset0 = prgValue0 & 0xF;
        prgOffset1 = cartridge.prgSize - 1;
    }
}

void Mapper1::loadControlRegister() {
    //Load Control
    chrBankMode = (controlRegister & 0x10); // 0: switch 8 KB at a time; 1: switch two separate 4 KB banks
    prgBankMode = (controlRegister & 0xC) >> 2;
    switch (controlRegister & 0x3) {
    case 0: // Lower Bank Single Screen
        cartridge.currentMirroring = Cartridge::SingleScreenA;
    case 1: // Upper Bank Single Screen
        cartridge.currentMirroring = Cartridge::SingleScreenB;
        break;
    case 2: // Vertical Mirroring
        cartridge.currentMirroring = Cartridge::VerticalMirror;
        break;
    case 3: // Horizontal Mirroring
        cartridge.currentMirroring = Cartridge::HorizontalMirror;
        break;
    }
    updateOffsets();
}

void Mapper1::clearLoadRegister() {
    loadRegister = 0x10;
}


