#include "Mapper4.h"
#include <stdlib.h> 
#include <stdio.h>

using namespace NES;

Mapper4::Mapper4(Cartridge &cartridge) : Mapper(cartridge) {
    prgOffset0 = 0x0;
    prgOffset1 = 0x1;
    prgOffset2 = (cartridge.prgSize * 2) - 2;
    prgOffset3 = (cartridge.prgSize * 2) - 1;
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
        if (enableRam) {
            return prgRAM[index - 0x6000];
        } else {
            return 0xFF; //TODO This should be open bus.
        }
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
    bool isOdd = (value & 0x1);
    
    if (index < 0x8000) {
        if (enableRamWrites && enableRam) {
            prgRAM[index - 0x6000] = value;
        }
    } else if (index < 0xA000 && isOdd == false) {
        //Bank Select
        bankSelect = value;
    } else if (index < 0xA000 && isOdd == true) {
        //Bank Data
        uint8_t prgMode = (bankSelect & 0x40) >> 6;
        uint8_t chrMode = (bankSelect & 0x80) >> 7;
        uint8_t bankSelection = (bankSelect & 0x7);
        
        switch (bankSelection) {
            case 0:
                if (chrMode == 0) {
                    chrOffset0 = value;
                    chrOffset1 = value + 1;
                }
                if (chrMode == 1) {
                    chrOffset4 = value;
                    chrOffset5 = value + 1;
                }
                break;
            case 1:
                if (chrMode == 0) {
                    chrOffset2 = value;
                    chrOffset3 = value + 1;
                }
                if (chrMode == 1) {
                    chrOffset6 = value;
                    chrOffset7 = value + 1;
                }
                break;
            case 2:
                if (chrMode == 0) chrOffset4 = value;
                if (chrMode == 1) chrOffset0 = value;
                break;
            case 3:
                if (chrMode == 0) chrOffset5 = value;
                if (chrMode == 1) chrOffset1 = value;
                break;
            case 4:
                if (chrMode == 0) chrOffset6 = value;
                if (chrMode == 1) chrOffset2 = value;
                break;
            case 5:
                if (chrMode == 0) chrOffset7 = value;
                if (chrMode == 1) chrOffset3 = value;
                break;
            case 6:
                if (prgMode == 0) {
                    prgOffset0 = value;
                    prgOffset2 = (cartridge.prgSize * 2) - 2;
                } else {
                    prgOffset2 = value;
                    prgOffset0 = (cartridge.prgSize * 2) - 2;
                }
                break;
            case 7:
                prgOffset1 = value;
                break;
            default:
                break;
        }
    } else if (index < 0xC000 && isOdd == false) {
        //Mirroring Select
        if (cartridge.currentMirroring != Cartridge::FourScreen) {
            if (value & 0x1) {
                cartridge.currentMirroring = Cartridge::HorizontalMirror;
            } else {
                cartridge.currentMirroring = Cartridge::VerticalMirror;
            }
        }
    } else if (index < 0xC000 && isOdd == true) {
        enableRam = ((value & 0x80) == 0x80);
        enableRamWrites = ((value & 0x40) != 0x40);
    } else if (index < 0xE000 && isOdd == false) {
        irqReload = value;
    } else if (index < 0xE000 && isOdd == true) {
        reloadIRQ = true;
    } else if (isOdd == false) {
        enableIRQ = false;
    } else {
        enableIRQ = true;
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
