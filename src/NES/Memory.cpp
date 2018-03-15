#include "Memory.h"
#include "PPU.h"
#include "APU.h"

#include <iostream>

using namespace std;

uint8_t NES::Memory::get(uint16_t index) {
    if (index < 0x2000) {
        //RAM Access
        return ram[index % 0x800];
    }

    if (index < 0x4000) {
        //PPU Access. Mirrors of $2000-2007 (repeats every 8 bytes)
        index = 0x2000 + (index % 0x8);
        return console.ppu->getPPURegister(index);
    }

    if (index == 0x4014) {
        //OAM DMA Access
        return console.ppu->getPPURegister(index);
    }


    if (index == 0x4015) {
        //APU Access
        return console.apu->getAPURegister(index);
    }


    if (index == 0x4016) {
        //Controller 1
        return console.controllerOne->pollController();
    }

    if (index == 0x4017) {
        //Controller 2
        //parent.controllerOne = 0x40;
        return 0x40;
    }

    if (index >= 0x6000) {
        return console.cartridge->getProgramData(index);
    }

    return 0x0;
}

void NES::Memory::set(uint16_t index, uint8_t value) {
    if (index < 0x2000) {
        //RAM Access
        ram[index % 0x800] = value;
    } else if (index < 0x4000) {
        //PPU Access. Mirrors of $2000-2007 (repeats every 8 bytes)
        index = 0x2000 + (index % 0x8);
        console.ppu->setPPURegister(index, value);
    } else if (index == 0x4014) {
        //OAM DMA Access
        console.ppu->setPPURegister(index, value);
    } else if (index == 0x4015) {
        //APU Access
        console.apu->setAPURegister(index, value);
    } else if (index == 0x4016) {
        //Controller 1
        console.controllerOne->startPoll();
    } else if (index == 0x4017) {
        //Controller 2
    } else if (index >= 0x6000) {
        console.cartridge->setProgramData(index, value);
    }
}

uint16_t NES::Memory::resetVector() {
    uint8_t lo = (*this).get(0xFFFC);
    uint8_t hi = (*this).get(0xFFFD);
    return hi << 8 | lo;
}
