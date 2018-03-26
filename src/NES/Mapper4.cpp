#include "Mapper4.h"
#include <stdlib.h> 
#include <stdio.h>

using namespace NES;

Mapper4::Mapper4(Cartridge &cartridge) : Mapper(cartridge) {
    updateOffsets();
}

uint8_t Mapper4::getTileData(uint16_t index) {
    tickScanlineCounter(index);
    
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
        return enableRam ? prgRAM[index - 0x6000] : 0xFF; //TODO This should be open bus
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

void Mapper4::updateOffsets() {
    if (chrMode == 0) {
        chrOffset0 = (registers[0] & 0xFE);
        chrOffset1 = (registers[0] | 0x1);
        chrOffset2 = (registers[1] & 0xFE);
        chrOffset3 = (registers[1] | 0x1);
        chrOffset4 = registers[2];
        chrOffset5 = registers[3];
        chrOffset6 = registers[4];
        chrOffset7 = registers[5];
    } else {
        chrOffset0 = registers[2];
        chrOffset1 = registers[3];
        chrOffset2 = registers[4];
        chrOffset3 = registers[5];
        chrOffset4 = (registers[0] & 0xFE);
        chrOffset5 = (registers[0] | 0x1);
        chrOffset6 = (registers[1] & 0xFE);
        chrOffset7 = (registers[1] | 0x1);
    }
    
    if (prgMode == 0) {
        prgOffset0 = (registers[6] & 0x3F);
        prgOffset1 = (registers[7] & 0x3F);
        prgOffset2 = (cartridge.prgSize * 2) - 2;
        prgOffset3 = (cartridge.prgSize * 2) - 1;
    } else {
        prgOffset0 = (cartridge.prgSize * 2) - 2;
        prgOffset1 = (registers[7] & 0x3F);
        prgOffset2 = (registers[6] & 0x3F);
        prgOffset3 = (cartridge.prgSize * 2) - 1;
    }
}

void Mapper4::setProgramData(uint16_t index, uint8_t value) {
    bool isOdd = (index & 0x1);
    
    if (index < 0x8000) {
        if (enableRamWrites && enableRam) {
            prgRAM[index - 0x6000] = value;
        }
    } else if (index < 0xA000) {
        if (isOdd == false) {
            //Bank Select
            prgMode = (value & 0x40);
            chrMode = (value & 0x80);
            selectedRegister = (value & 0x7);
            updateOffsets();
        } else {
            //Bank Data
            registers[selectedRegister] = value;
            updateOffsets();
        }
    } else if (index < 0xC000) {
        if (isOdd == false) {
            //Mirroring Select
            if (cartridge.currentMirroring != Cartridge::FourScreen) {
                if (value & 0x1) {
                    cartridge.currentMirroring = Cartridge::HorizontalMirror;
                } else {
                    cartridge.currentMirroring = Cartridge::VerticalMirror;
                }
            }
        } else {
            enableRam = ((value & 0x80) == 0x80);
            enableRamWrites = ((value & 0x40) != 0x40);
        }
    } else if (index < 0xE000) {
        if (isOdd == false) {
            irqReload = value;
        } else {
            reloadIRQ = true;
        }
    } else {
        if (isOdd == false) {
            enableIRQ = false;
        } else {
            enableIRQ = true;
        }
    }
}

void Mapper4::tickScanlineCounter(uint16_t index){
    if (index < 0x1000) {
        scanlineLatch = false;
    }
    
    if (index >= 0x1000 && scanlineLatch == false) {
        scanlineLatch = true;
        if (counter == 0 || reloadIRQ) {
            reloadIRQ = false;
            counter = irqReload;
        } else {
            counter--;
            if (counter == 0 && enableIRQ) {
                cartridge.console.cpu->requestIRQ = true;
            }
        }
    }
}
