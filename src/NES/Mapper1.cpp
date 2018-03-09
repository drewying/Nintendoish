#include "Mapper1.h"
#include <stdlib.h> 
#include <stdio.h>

using namespace NES;

Mapper1::Mapper1(Cartridge &cartridge) : Mapper(cartridge) {
    prgOffset1 = 0x0;
    prgOffset2 = cartridge.prgSize - 1;
}

uint8_t Mapper1::getTileData(uint16_t index) {
    if (index < 0x1000) {
        return cartridge.chr[index + (chrOffset1 * 0x1000)];
    } else {
        return cartridge.chr[index + (chrOffset2 * 0x1000)];
    }
}

void Mapper1::setTileData(uint16_t index, uint8_t value) {
    if (index < 0x1000) {
        cartridge.chr[index + (chrOffset1 * 0x1000)] = value;
    } else {
        cartridge.chr[index + (chrOffset2 * 0x1000)] = value;
    }
}

uint8_t Mapper1::getProgramData(uint16_t index) {
    if (index >= 0xC000) {
        return cartridge.prg[(index - 0xC000) + prgOffset2 * 0x4000]; // Calculation done inline to prevent 16 bit overflow
    }
    else if (index >= 0x8000) {
        return cartridge.prg[(index - 0x8000) + (prgOffset1 * 0x4000)]; // Calculation done inline to prevent 16 bit overflow
    }
    else {
        printf("Ruh Roh. SVRAM not implemented.");
    }
    return 0x0;
}

void Mapper1::setProgramData(uint16_t index, uint8_t value) {
    if ((value & 0x80) == 0x80) {
        // Clear load register to initial state
        loadRegister = 0x10;
        return;
    } 

    bool copyData = ((loadRegister & 0x1) == 0x1); //Shift register is full. Copy t0 control.
    
    //Shift load register.
    loadRegister >>= 1;
    loadRegister |= ((value & 0x1) << 4);
    if (copyData) { 
        if (index < 0xA000) {
            //Load Control
            chrBankMode = (value & 0x10) == 0x10;
            prgBankMode = ((value & 0xC) >> 2);
            switch (value & 0x3) {
            case 0:
            case 1:
                cartridge.horizontalMirroring = true;
                cartridge.verticalMirroring = true;
                break;
            case 2:
                cartridge.horizontalMirroring = false;
                cartridge.verticalMirroring = true;
            case 3:
                cartridge.horizontalMirroring = true;
                cartridge.verticalMirroring = false;
            }
        } else if (index < 0xC000) {
            chrOffset1 = value;
            if (chrBankMode == 0x0) chrOffset1 *= 2;
        } else if (index < 0xE000) {
            chrOffset2 = value;
            if (chrBankMode == 0x0) chrOffset2 *= 2;
        } else if (index <= 0xFFFF) {
            if (prgBankMode == 0x0) {
                prgOffset1 = (value & 0xE) * 0x2;
            } else if (prgBankMode == 0x1) {
                prgOffset1 = 0x0;
                prgOffset2 = (value & 0xF);
            } else if (prgBankMode == 0x3) {
                prgOffset1 = (value & 0xF);
                prgOffset2 = cartridge.prgSize - 1;
            }
        }
    }
}




