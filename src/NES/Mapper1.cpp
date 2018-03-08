#include "Mapper1.h"
#include <stdlib.h> 
#include <stdio.h>

using namespace NES;

unsigned char Mapper1::getTileData(unsigned short index) {
	return cartridge.chr[index];
}

void Mapper1::setTileData(unsigned short index, unsigned char value) {
	cartridge.chr[index] = value;
}

unsigned char Mapper1::getProgramData(unsigned short index) {
	if (cartridge.prgSize == 1 && index > 0xC000) {
		index -= 0x4000;
	}
	return cartridge.prg[index - 0x8000];
}

void Mapper1::setProgramData(unsigned short index, unsigned char value) {
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
