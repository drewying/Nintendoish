#include "Memory.h"
#include "PPU.h"

#include <iostream>

using namespace std;

unsigned char &NES::Memory::operator [](unsigned short index) {
    if (index < 0x2000) {
        //RAM Access
        return ram[index % 0x800];
    }
    
    if (index < 0x4000) {
        //PPU Access. Mirrors of $2000-2007 (repeats every 8 bytes)
        return parent.ppu->memory[0x2000 + (index % 0x8)];
    }
    
    if (index == 0x4014) {
        //OAM DMA Access
        return parent.ppu->memory[index];
    }

	if (index == 0x4016) {
		//Controller 1
		parent.controllerOne = 0x40;
		return parent.controllerOne;
	}

	if (index == 0x4017) {
		//Controller 2
		parent.controllerOne = 0x40;
		return parent.controllerOne;
	}
    
    if (index >= 0xC000) {
        return prg[index - 0xC000];
    }
    
    if (index >= 0x8000) {
        //NROM Mapper access
        if (extended == false && index > 0x4000) {
            index -= 0x4000;
        }
        return prg[index - 0x8000];
    }
    
    return prg[index];
}

unsigned short NES::Memory::resetVector() {
    unsigned char lo = (*this)[0xFFFC];
    unsigned char hi = (*this)[0xFFFD];
    return hi << 8 | lo;
}
