#include "PPUMemory.h"

unsigned char NES::PPUMemory::get(unsigned short index) {
	if (index < 0x2000) {
		return chr[index];
	}
	if (index < 0x3000) {
		//TODO Proper Mirroring
		if (index < 0x2800) {
			return vram[index - 0x2000];
		}
		else {
			return vram[index - 0x2800];
		}
	}
	if (index < 0x3F00) { 
		return vram[index - 0x3000];
	}
	if (index < 0x4000) {
		if (index >= 0x3F10 && (index - 0x3F10) % 4 == 0) index -= 0x10; // Pallette Mirroring
		return pal[index - 0x3F00];
	}
}

void  NES::PPUMemory::set(unsigned short index, unsigned char value) {
	if (index < 0x2000) {
		chr[index] = value;
	} else if (index < 0x3000) {
		//TODO Proper Mirroring
		if (index < 0x2800) {
			vram[index - 0x2000] = value;
		}
		else {
			vram[index - 0x2800] = value;
		}
	} else if (index < 0x3F00) {
		vram[index - 0x3000] = value;
	}else if (index < 0x4000) {
		if (index >= 0x3F10 && (index - 0x3F10) % 0x4 == 0) index -= 0x10; // Pallette Mirroring
		pal[index - 0x3F00] = value;
	}
}

NES::PPUMemory::PPUMemory()
{
}
