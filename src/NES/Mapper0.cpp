#include "Mapper0.h"
#include <stdlib.h> 
#include <stdio.h>

using namespace NES;

uint8_t Mapper0::getTileData(uint16_t index) {
	return cartridge.chr[index];
}

void Mapper0::setTileData(uint16_t index, uint8_t value) {
	cartridge.chr[index] = value;
}

uint8_t Mapper0::getProgramData(uint16_t index) {
	if (cartridge.prgSize == 1 && index > 0xC000) {
		index -= 0x4000;
	}
	return cartridge.prg[index - 0x8000];
}

void Mapper0::setProgramData(uint16_t index, uint8_t value) {
	if (cartridge.prgSize == 1 && index > 0xC000) {
		index -= 0x4000;
	}
	cartridge.prg[index - 0x8000] = value;
}
