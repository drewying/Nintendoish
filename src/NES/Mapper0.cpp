#include "Mapper0.h"
#include <stdlib.h> 
#include <stdio.h>

using namespace NES;

unsigned char Mapper0::getTileData(unsigned short index) {
	return cartridge.chr[index];
}

void Mapper0::setTileData(unsigned short index, unsigned char value) {
	cartridge.chr[index] = value;
}

unsigned char Mapper0::getProgramData(unsigned short index) {
	if (cartridge.prgSize == 1 && index > 0xC000) {
		index -= 0x4000;
	}
	return cartridge.prg[index - 0x8000];
}

void Mapper0::setProgramData(unsigned short index, unsigned char value) {
	if (cartridge.prgSize == 1 && index > 0xC000) {
		index -= 0x4000;
	}
	cartridge.prg[index - 0x8000] = value;
}
