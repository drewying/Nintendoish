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
			return parent.memory->chr[parent.ppu->PPUADDR.address];
		}
		if (parent.ppu->PPUADDR.address < 0x3000) {
			parent.ppu->PPUADDR.address += 0x1;
			return parent.memory->vram[parent.ppu->PPUADDR.address - 0x2001];
		}
		if (parent.ppu->PPUADDR.address < 0x3F00) {
			parent.ppu->PPUADDR.address += 0x1;
			return parent.memory->vram[parent.ppu->PPUADDR.address - 0x3001];
		}
		if (parent.ppu->PPUADDR.address < 0x4000) {
			return parent.memory->pal[parent.ppu->PPUADDR.address - 0x3F00];
		}
		break;
	default:
		exit(0);
		break;
	}
}

void NES::PPU::setPPURegister(unsigned short index, unsigned char value) {
	switch (index) {
	case 0x2000: //PPU Control Flags
		PPUCTRL.byte = value;
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
		if (PPUADDR.address < 0x3000) {
			PPUADDR.address++;
			parent.memory->vram[PPUADDR.address - 0x2001] = value;
		}
		if (PPUADDR.address < 0x3F00) {
			PPUADDR.address += 0x1;
			parent.memory->vram[parent.ppu->PPUADDR.address - 0x3001] = value;
		}
		if (PPUADDR.address < 0x4000) {
			parent.memory->pal[parent.ppu->PPUADDR.address - 0x3F00] = value;
		}
		break;
	case 0x4014: // OAM DMA High Address
		DMAINDEX = value;
		copyDMAMemory(DMAINDEX - 1);
		break;
	default:
		exit(0);
		break;
	}
	parent.ppu->PPUSTATUS.status.Address = value & 0x1F;
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
	parent.stallCycles = (parent.cpu->cycles - 1) % 2 == 1 ? 514 : 513;
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
/*	bool renderBackground = PPUMASK.status.ShowBackground;
	bool renderSprite = PPUMASK.status.ShowSprite;
	bool oddCycle = currentCycle & 2 != 0;

	static int currentSpriteIndex = 0;
	static int currentOamIndex = 0;
	static bool writeFlag = false;
	static Sprite* currentSprite;

	if (currentCycle == 0) {
		// Idle Cycle.
	}
	if (currentCycle >= 1 && currentCycle <= 64) {
		if (currentCycle == 1) {
			memset(&spriteList, 0xFF, 8);
			currentSpriteIndex = 0;
			currentOamIndex = 0;
		}
	}


	if (currentCycle >= 65 && currentCycle <= 256 && currentOamIndex < 256 && renderSprite == true) {
		// Render visible Scanline
		//if (renderBackground) {
		//	printf("Render Background\n");
		//}

		//printf("Render Sprite\n");
		//renderPatternTable();
		if (oddCycle == true) {
			// On odd cycles, data is read from (primary) OAM
			if (memory.oam[currentOamIndex] <= (currentScanline + 1) && (memory.oam[currentOamIndex] + 8) >= (currentScanline + 1)) {
				currentSprite = (Sprite*)&memory.oam[currentOamIndex];
				writeFlag = true;
			}

		} else if (writeFlag == true){
			// On even cycles, data is written to secondary OAM
			writeFlag = false;
			memcpy(&spriteList[currentSpriteIndex], currentSprite, 4);
			currentSpriteIndex += 1;
		}
		currentOamIndex += 4;

	}
	if (currentCycle >= 257 && currentCycle < 320) {
		OAMADDR = 0x0;
		//Tile Data
	} */
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
	/*for (int i = 0; i <= 8; i++) {
		char tileSliceA = memory.chr[tileIndex + i];
		char tileSliceB = memory.chr[tileIndex + i + 8];
		for (int j = 7; j >= 0; j--) {
			unsigned short colorIndex = (tileSliceB & 0x1) << 1 | (tileSliceA & 0x1);
			colorIndex = 0x3F10  + colorIndex;
			//colorIndex += 2;
			//unsigned char t = memory.vram[colorIndex];
			//unsigned char* color = colorTable[memory.vram[colorIndex]];
			unsigned char c = (((float)colorIndex / 3.0) * 255);
			unsigned char color[3] = { c, 0, 0 };

			unsigned int combinedColor = color[0] << 16 | color[1] << 8 | color[2];
			parent.graphics[x + j + ((y + i) * 256)] = combinedColor;
			tileSliceA = tileSliceA >> 1;
			tileSliceB = tileSliceB >> 1;
		}
	}*/
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
			vBlankEnd();
		}
	}

	bool renderingEnabled = (PPUMASK.status.ShowSprite || PPUMASK.status.ShowBackground);
	if (renderingEnabled == true && currentScanline >= 0 && currentScanline <= 239 && currentCycle >= 1 && currentCycle <= 340) {
		//skipCycle = true;
	}

	if (currentScanline == -1 && currentCycle == 340 && ODDFRAME == true && renderingEnabled == true) {
		//skipCycle = true;
	}


    // Render Visible Scanlines
    if (currentScanline >= 0 && currentScanline <= 239) {
		//renderScanline();
    }
    
    if (currentScanline == 240) {
        // Do Nothing. Idle Scanline
    }
   
	// VBlank Scanlines
    if (currentScanline == 241 && currentCycle == 1 && parent.cpu->cycles > 100) {
        vBlankStart();
    }
}

void NES::PPU::vBlankStart() {
    PPUSTATUS.status.VBlankStarted = true;
	if (PPUCTRL.status.NMI == true) {
		printf("\n\n\nNMI REQUESTED\n\n\n");
		parent.cpu->requestNMI = true;
	}
}

void NES::PPU::vBlankEnd() {
    PPUSTATUS.status.VBlankStarted = false;
}
