#include "PPU.h"
#include <iostream>

using namespace std;


unsigned char NES::PPU::getPPURegister(unsigned short index) {
	char status;
	switch (index) {
	case 0x2002: //PPU Status Flags
		status = PPUSTATUS.byte;
		PPUSTATUS.status.VBlankStarted = false;
		PPUADDRLATCH = false;
		return status;
		break;
	case 0x2004: //OAM data Read/Write
		return parent.memory->oam[parent.ppu->OAMADDR];
		break;
	case 0x2007: //PPU Data Read/Write
		if (parent.ppu->PPUADDR.address < 0x2000) {
			return parent.memory->chr[PPUADDR.address];
		}
		if (parent.ppu->PPUADDR.address < 0x3000) {
			return parent.memory->vram[PPUADDR.address - 0x2001];
		}
		if (parent.ppu->PPUADDR.address < 0x3F00) {
			return parent.memory->vram[PPUADDR.address - 0x3001];
		}
		if (parent.ppu->PPUADDR.address < 0x4000) {
			return parent.memory->pal[PPUADDR.address - 0x3F00];
		}
		PPUADDR.address += PPUCTRL.status.VRAMAddress ? 32 : 1;
		exit(0);
		break;
	default:
		exit(0);
		break;
	}
}

void NES::PPU::setPPURegister(unsigned short index, unsigned char value) {
	switch (index) {
	case 0x2000: //PPU Control Flags
		if (((PPUCTRL.byte >> 0x7) == 0x0) && ((value >> 0x7) == 0x1) && PPUSTATUS.status.VBlankStarted == true) {
			parent.cpu->requestNMI = true;
		}
		PPUCTRL.byte = value;
		if (PPUCTRL.status.NMI == true && PPUSTATUS.status.VBlankStarted == true) {
			//printf("\n\n\nNMI REQUESTED\n\n\n");
			parent.cpu->requestNMI = true;
		}

		if (PPUCTRL.status.NMI == true) {
			//printf("\n\n\nNMI ENABLED\n\n\n", PPUCTRL.byte, value);
		} else {
			//printf("\n\n\nNMI DISABLED\n\n\n", PPUCTRL.byte, value);
		}
		break;
	case 0x2001: //PPU Render Flags
		PPUMASK.byte = value;
		break;
	case 0x2003: //OAM Read/Write Address
		OAMADDR = value;
		break;
	case 0x2004: //OAM data Read/Write
		parent.memory->oam[OAMADDR] = value;
		OAMADDR += 1;
		break;
	case 0x2005:
		//TODO PPU Scroll
		PPUSCROLL = value;
		break;
	case 0x2006: //PPU Read/Write Address
		if (PPUADDRLATCH == false) {
			PPUADDRLATCH = true;
			PPUADDR.byte.lo = value;
		}
		else {
			PPUADDRLATCH = false;
			PPUADDR.byte.hi = value;
		}
		break;
	case 0x2007: //PPU Data Read/Write
		if (PPUADDR.address < 0x2000) {
			exit(0); //Shouldn't write to CHR data.
		} else if (PPUADDR.address < 0x3000) {
			parent.memory->vram[PPUADDR.address - 0x2001] = value;
		} else if (PPUADDR.address < 0x3F00) { 
			parent.memory->vram[PPUADDR.address - 0x3001] = value;
		} else if (PPUADDR.address < 0x4000) {
			parent.memory->pal[PPUADDR.address - 0x3F00] = value;
		} else {
			exit(0);
		}
		PPUADDR.address += PPUCTRL.status.VRAMAddress ? 32 : 1;
		break;
	case 0x4014: // OAM DMA High Address
		DMAINDEX = value;
		copyDMAMemory(DMAINDEX - 1);
		break;
	default:
		exit(0);
		break;
	}
	if (index != 0x4014) {
		PPUSTATUS.status.Address = value & 0x1F;
	}
}

/*
void NES::PPU::processWrite() {
	if (lastWrite != 0x0) {
		if (parent.cpu->cycles < 29658 &&
			(lastWrite == 0x2000 ||
				lastWrite == 0x2001 ||
				lastWrite == 0x2005 ||
				lastWrite == 0x2006)
			) {
			memory[lastWrite] = lastValue;
		}
		else {
			if (lastWrite == 0x2000 ||
				lastWrite == 0x2001 ||
				lastWrite == 0x2003 ||
				(lastWrite == 0x2004 && memory[lastWrite] != lastValue) ||
				lastWrite == 0x2005 ||
				lastWrite == 0x2006 ||
				(lastWrite == 0x2007 && memory[lastWrite] != lastValue)) {
				PPUSTATUS.status.Address = memory[lastWrite] & 0x1F;
			}
			switch (lastWrite) {
			case 0x2000:
				if (lastValue != PPUCTRL.byte) {
					// When turning on the NMI flag in bit 7, if the PPU is currently in vertical blank and the 
					// PPUSTATUS ($2002) vblank flag is set, an NMI will be generated immediately
					if (((lastValue >> 0x7) == 0x0) && ((PPUCTRL.byte >> 0x7) == 0x1) && PPUSTATUS.status.VBlankStarted == true) {
						parent.cpu->requestNMI = true;
					}
				}
				break;
			case 0x2002:
				PPUSTATUS.status.VBlankStarted = false;
				PPUADDRLATCH = false;
				break;
			case 0x2004:
				if (lastValue != memory.oam[OAMADDR]) {
					OAMADDR += 1;
				}
				break;
			case 0x4014:
				copyDMAMemory(DMAINDEX);
				break;
			}
		}

		lastWrite = 0x0;
		lastValue = 0x0;
	}
}

*/

void NES::PPU::copyDMAMemory(unsigned char index) {
	parent.cpu->stallCycles = (parent.cpu->cycles) % 2 == 1 ? 513 : 514;
	for (int i = 0x0; i < 0xFF; i++) {
		parent.memory->oam[i] = parent.memory->get((index * 0x100) + i);
	}
	//memcpy(memory.oam, &parent.memory[index * 0x100], 0xFF);
}

void NES::PPU::reset() {
    PPUCTRL.byte = 0x0;
    PPUMASK.byte = 0x0;
    PPUSTATUS.byte = 0x10;
    OAMADDR = 0x0;
	PPUADDR.address = 0x0;
    PPUADDRLATCH = false;
    PPUSCROLL = 0x0;
	DMAINDEX = 0xFF;
    ODDFRAME = false;
}


void NES::PPU::renderScanline() {
	bool renderBackground = PPUMASK.status.ShowBackground;
	bool renderSprite = PPUMASK.status.ShowSprite;
	if (renderBackground && currentCycle < 256) {
		unsigned int tileX = currentCycle / 8;
		unsigned int tileY = currentScanline / 8;
		unsigned int paletteX = currentCycle / 32;
		unsigned int paletteY = currentCycle / 30;
		
		unsigned int attributeIndex = paletteX + (paletteY * 8);
		unsigned int nametableIndex = tileX + (tileY * 32);

		unsigned int tileIndex = (parent.memory->vram[nametableIndex] + 0x100) * 0x10;
		tileIndex += currentScanline % 8;

		unsigned int paletteIndex = parent.memory->vram[attributeIndex + 0x3C0];

		unsigned char tileSliceA = parent.memory->chr[tileIndex];
		unsigned char tileSliceB = parent.memory->chr[tileIndex + 8];
		
		tileSliceA = tileSliceA >> (7 - currentCycle % 8);
		tileSliceB = tileSliceB >> (7 - currentCycle % 8);
		
		unsigned short colorIndex = (tileSliceB & 0x1) << 1 | (tileSliceA & 0x1);
		unsigned char* backgroundColor = colorTable[parent.memory->pal[0]];
		unsigned char* color = colorTable[parent.memory->pal[colorIndex]];
		if (colorIndex == 0) color = backgroundColor;
		unsigned int combinedColor = color[0] << 16 | color[1] << 8 | color[2];
		parent.graphics[currentCycle + (currentScanline * 256)] = combinedColor;
	}
} 

void NES::PPU::renderPatternTable() {
	int tileIndex = 0x0000;
	
	for (int y = 0; y < 128; y += 8) {
		for (int x = 0; x < 128; x += 8) {
			renderTile(x, y, tileIndex);
			tileIndex += 0x10;
		}
	}

	for (int y = 0; y < 128; y += 8) {
		for (int x = 128; x < 256; x += 8) {
			renderTile(x, y, tileIndex);
			tileIndex += 0x10;
		}
	}
}

 
void NES::PPU::renderTile(int x, int y, int tileIndex) {
	for (int i = 0; i <= 8; i++) {
		char tileSliceA = parent.memory->chr[tileIndex + i];
		char tileSliceB = parent.memory->chr[tileIndex + i + 8];
		for (int j = 7; j >= 0; j--) {
			unsigned short colorIndex = (tileSliceB & 0x1) << 1 | (tileSliceA & 0x1);

			unsigned char* backgroundColor = colorTable[parent.memory->pal[0]];
			unsigned char* color = colorTable[parent.memory->pal[colorIndex + 6]];

			if (colorIndex == 0) color = backgroundColor;

			unsigned int combinedColor = color[0] << 16 | color[1] << 8 | color[2];
			parent.graphics[x + j + ((y + i) * 256)] = combinedColor;
			tileSliceA = tileSliceA >> 1;
			tileSliceB = tileSliceB >> 1;
		}
	}
}

void NES::PPU::step() {


	if (skipCycle) {
		skipCycle = false;
		return;
	}

	cycles += 1;
	currentCycle += 1;

	if (currentCycle == 341) { // Next Scanline
		currentCycle = 0;
		currentScanline++;
		if (currentScanline == 261) { // Next Frame
			frameCount++;
			currentScanline = -1;
			ODDFRAME = !ODDFRAME;
			parent.updateGraphics = true;
		}
	}

	bool renderingEnabled = (PPUMASK.status.ShowSprite || PPUMASK.status.ShowBackground);
	if (renderingEnabled == true && currentScanline >= 0 && currentScanline <= 239 && currentCycle >= 1 && currentCycle <= 340) {
		//renderScanline();
		//printf("RENDER");
		//renderPatternTable();
		//skipCycle = true;
	}

	if (currentScanline == -1 && currentCycle == 340 && ODDFRAME == true && renderingEnabled == true) {
		//skipCycle = true;
	}


    // Render Visible Scanlines
    if (currentScanline >= 0 && currentScanline <= 239) {
		renderScanline();
    }
    
    if (currentScanline == 240) {
        // Do Nothing. Idle Scanline
    }

	// VBlank Scanlines
	if (currentScanline == -1 && currentCycle == 1 && parent.cpu->cycles > 100) {
		vBlankEnd();
	}

	if (currentScanline == 241 && currentCycle == 1 && parent.cpu->cycles > 100) {
		vBlankStart();
	}




   



}

void NES::PPU::vBlankStart() {
	//printf("\n\n\nVBLANK\n\n\n");
    PPUSTATUS.status.VBlankStarted = true;
	if (PPUCTRL.status.NMI == true) {
		//printf("\n\n\nNMI REQUESTED\n\n\n");
		parent.cpu->requestNMI = true;
	}
}

void NES::PPU::vBlankEnd() {
    PPUSTATUS.status.VBlankStarted = false;
}
