#include "Memory.h"
#include "PPU.h"

#include <iostream>

using namespace std;

unsigned char NES::Memory::get(unsigned short index) {
	if (index < 0x2000) {
		//RAM Access
		return ram[index % 0x800];
	}

	if (index < 0x4000) {
		//PPU Access. Mirrors of $2000-2007 (repeats every 8 bytes)
		index = 0x2000 + (index % 0x8);
		return parent.ppu->getPPURegister(index);
	}

	if (index == 0x4014) {
		//OAM DMA Access
		return parent.ppu->getPPURegister(index);
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

void NES::Memory::set(unsigned short index, unsigned char value) {
	if (index < 0x2000) {
		//RAM Access
		ram[index % 0x800] = value;
	}

	if (index < 0x4000) {
		//PPU Access. Mirrors of $2000-2007 (repeats every 8 bytes)
		index = 0x2000 + (index % 0x8);
		parent.ppu->setPPURegister(index, value);
	}

	if (index == 0x4014) {
		//OAM DMA Access
		parent.ppu->setPPURegister(index, value);
	}

	if (index == 0x4016) {
		//Controller 1
		parent.controllerOne = 0x40;
		parent.controllerOne = value;
	}

	if (index == 0x4017) {
		//Controller 2
		parent.controllerOne = 0x40;
		parent.controllerOne = value;
	}

	if (index >= 0xC000) {
		prg[index - 0xC000] = value;
	}

	if (index >= 0x8000) {
		//NROM Mapper access
		if (extended == false && index > 0x4000) {
			index -= 0x4000;
		}
		prg[index - 0x8000] = value;
	}
}

unsigned short NES::Memory::resetVector() {
    unsigned char lo = (*this).get(0xFFFC);
    unsigned char hi = (*this).get(0xFFFD);
    return hi << 8 | lo;
}
