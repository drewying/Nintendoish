#include "PPU.h"
#include <iostream>

using namespace std;


unsigned char NES::PPU::getPPURegister(unsigned short index) {
	char status;
	short address;
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
		address = PPUADDR.address;
		PPUADDR.address += PPUCTRL.status.VRAMAddress ? 32 : 1;
		if (address < 0x2000) {
			return parent.memory->chr[address];
		}
		if (address < 0x3000) {
			return parent.memory->vram[address - 0x2000];
		}
		if (address < 0x3F00) {
			return parent.memory->vram[address - 0x3000];
		}
		if (address < 0x4000) {
			return parent.memory->pal[address - 0x3F00];
		}
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
			parent.memory->vram[PPUADDR.address - 0x2000] = value;
		} else if (PPUADDR.address < 0x3F00) { 
			parent.memory->vram[PPUADDR.address - 0x3000] = value;
		} else if (PPUADDR.address < 0x4000) {
			parent.memory->pal[PPUADDR.address - 0x3F00] = value;
		} else {
			exit(0);
		}
		PPUADDR.address += PPUCTRL.status.VRAMAddress ? 32 : 1;
		break;
	case 0x4014: // OAM DMA High Address
		copyDMAMemory(value);
		break;
	default:
		exit(0);
		break;
	}
	if (index != 0x4014) {
		PPUSTATUS.status.Address = value & 0x1F;
	}
}

void NES::PPU::copyDMAMemory(unsigned char index) {
	parent.cpu->stallCycles = (parent.cpu->cycles) % 2 == 1 ? 513 : 514;
	for (int i = 0x0; i < 0x100; i++) {
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
	ODDFRAME = false;
}


void NES::PPU::prepareSprites() {
	Sprite* spriteList = (Sprite*)parent.memory->oam;
	int spriteCount = 0;
	int oamIndex = 0;
	size_t t = sizeof(Sprite);
	memset(spriteBuffer, 0x0, sizeof(Sprite) * 8);
	while (spriteCount < 8 && oamIndex < 64) {
		if (spriteList->yPosition != 0 &&
			currentScanline >= spriteList->yPosition + 1 &&
			currentScanline < spriteList->yPosition + 1 + 8)
		{
			spriteBuffer[spriteCount] = *spriteList;
			spriteCount++;
		}
		spriteList++;
		oamIndex++;
	}
}

void NES::PPU::renderPixel(int x, int y) {
	if (x > 256) return;

	unsigned char* defaultColor = colorTable[parent.memory->pal[0]]; //Default Background Color
	unsigned char* finalColor = defaultColor;

	if (PPUMASK.status.ShowBackground) {
		unsigned int tileX = x / 8;
		unsigned int tileY = y / 8;
		unsigned int tileIndex = tileX + (tileY * 32);

		//Palette
		unsigned char paletteX = x / 32;
		unsigned char paletteY = y / 32;
		unsigned char paletteIndex = paletteX + (paletteY * 8);
		unsigned char paletteInfo = parent.memory->vram[0x3C0 + paletteIndex];

		paletteX = (x & 0x1F) >> 4;
		paletteY = (y & 0x1F) >> 4;

		if (paletteX == 0 && paletteY == 0) paletteInfo = (paletteInfo &  0x3);      // Top Left 00
		if (paletteX == 1 && paletteY == 0) paletteInfo = (paletteInfo &  0xF) >> 2; // Top Right 10
		if (paletteX == 0 && paletteY == 1) paletteInfo = (paletteInfo & 0x3F) >> 4; // Bottom Left 01
		if (paletteX == 1 && paletteY == 1) paletteInfo =  paletteInfo >> 6;         // Bottom Right 11

		unsigned char* backgroundColor =  getTileColor(
			parent.memory->vram[tileIndex] + (PPUCTRL.status.BackgroundTableAddress ? 0x100 : 0x0),  //Left or Right?
			x % 8, 
			y % 8, 
			paletteInfo,
			false, 
			false
		);
		finalColor = backgroundColor;
	} 

	if (PPUMASK.status.ShowSprite == true) {
		for (int i = 0; i < 8; i++) { 
			Sprite sprite = spriteBuffer[i];
			unsigned char spriteY = sprite.yPosition + 1;
			unsigned char spriteX = sprite.xPosition;
			
			if (sprite.xPosition != 0 &&
				x >= spriteX &&
				x < spriteX + 8) 
			{
					spriteX = x - spriteX;
					spriteY = y - spriteY;
					
					unsigned char* spriteColor = getTileColor(
						sprite.tileIndex + (PPUCTRL.status.SpriteTableAddress ? 0x100 : 0x0),
						spriteX, 
						spriteY, 
						sprite.attributes.palette + 0x4,
						sprite.attributes.horizontalFlip, 
						sprite.attributes.verticalFlip
					);
					if (spriteColor != defaultColor) {
						if (sprite.attributes.priority == 0 || finalColor == defaultColor) finalColor = spriteColor;
						break;
					}
			}
		}
	}

	unsigned int combinedColor = finalColor[0] << 16 | finalColor[1] << 8 | finalColor[2];
	parent.graphics[x + (y * 256)] = combinedColor;

} 

unsigned char* NES::PPU::getTileColor(
	unsigned int tileIndex,
	unsigned int tileX, 
	unsigned int tileY, 
	unsigned int paletteIndex, 
	unsigned int flipHorizontal, 
	unsigned int flipVertical) {
	tileIndex *= 0x10;
	tileIndex += flipVertical ? 8 - tileY : tileY;

	unsigned char tileSliceA = parent.memory->chr[tileIndex];
	unsigned char tileSliceB = parent.memory->chr[tileIndex + 8];

	tileSliceA = tileSliceA >> (flipHorizontal ? tileX : 7 - tileX);
	tileSliceB = tileSliceB >> (flipHorizontal ? tileX : 7 - tileX);

	unsigned short colorIndex = (tileSliceB & 0x1) << 1 | (tileSliceA & 0x1);
	if (colorIndex == 0) return colorTable[parent.memory->pal[0]];
	return colorTable[parent.memory->pal[(paletteIndex * 4) + colorIndex]];
}

void NES::PPU::renderPatternTable() {
	int tileIndex = 0x0000;
	
	for (int y = 0; y < 128; y += 8) {
		for (int x = 0; x < 128; x += 8) {
			renderTile(x, y, tileIndex);
			tileIndex++;
		}
	}

	for (int y = 0; y < 128; y += 8) {
		for (int x = 128; x < 256; x += 8) {
			renderTile(x, y, tileIndex);
			tileIndex;
		}
	}
}

 
void NES::PPU::renderTile(int x, int y, int tileIndex) {
	tileIndex *= 0x10;
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

	if (currentCycle == 0 && currentScanline < 240) prepareSprites();
	
	//Render Visible Scanlines
	bool renderingEnabled = (PPUMASK.status.ShowSprite || PPUMASK.status.ShowBackground);
	if (renderingEnabled == true && currentScanline >= 0 && currentScanline < 240 && currentCycle >= 0 && currentCycle < 256) {
		renderPixel(currentCycle, currentScanline);
	}

	// VBlank Scanlines
	if (currentScanline == 241 && currentCycle == 1 && parent.cpu->cycles > 100) {
		vBlankStart();
	}

	if (currentScanline == -1 && currentCycle == 1 && parent.cpu->cycles > 100) {
		vBlankEnd();
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
