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
		return console.ppu->getPPURegister(index);
	}

	if (index == 0x4014) {
		//OAM DMA Access
		return console.ppu->getPPURegister(index);
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

	if (index >= 0x8000) {
		return console.cartridge->getProgramData(index);
	}

	return 0x0;
}

void NES::Memory::set(unsigned short index, unsigned char value) {
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
	} else if (index == 0x4016) {
		//Controller 1
		console.controllerOne->startPoll();
	} else if (index == 0x4017) {
		//Controller 2
	} else if (index >= 0x8000) {
		console.cartridge->setProgramData(index, value);
	}
}

unsigned short NES::Memory::resetVector() {
    unsigned char lo = (*this).get(0xFFFC);
    unsigned char hi = (*this).get(0xFFFD);
	return hi << 8 | lo;
}
