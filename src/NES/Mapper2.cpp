#include "Mapper2.h"
#include <stdlib.h> 
#include <stdio.h>

using namespace NES;

uint8_t Mapper2::getTileData(uint16_t index) {
	return cartridge.chr[index];
}

void Mapper2::setTileData(uint16_t index, uint8_t value) {
	cartridge.chr[index] = value;
}

uint8_t Mapper2::getProgramData(uint16_t index) {
	if (index >= 0xC000) {
		return cartridge.prg[(index - 0xC000) + (cartridge.prgSize - 1) * 0x4000]; // Calculation done inline to prevent 16 bit overflow
	} else if (index >= 0x8000) {
		return cartridge.prg[(index - 0x8000) + (offset * 0x4000)]; // Calculation done inline to prevent 16 bit overflow
	} else {
		printf("Ruh Roh. SVRAM not implemented.");
	}
	return 0x0;
}

void Mapper2::setProgramData(uint16_t index, uint8_t value) {
	if (index >= 0x8000) {
		offset = (value & 0x7);
	} else if (index >= 0x6000){
		printf("Ruh Roh. SVRAM not implemented.");
	}
}
