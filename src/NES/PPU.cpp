#include "PPU.h"
#include <iostream>

using namespace std;


unsigned char NES::PPU::getPPURegister(unsigned short index) {
	char status;
	short address;
	switch (index) {
	case 0x2002: //PPU Status Flags
		status = reg.status.byte;
		reg.status.flags.VBlankStarted = false;
		reg.address.writeLatch = false;
		return status;
		break;
	case 0x2004: //OAM data Read/Write
		return oam[reg.oamAddress];
		break;
	case 0x2007: //PPU Data Read/Write
		address = reg.address.current.address;
		reg.address.current.address += reg.control.flags.VRAMAddress ? 32 : 1;

		if (address < 0x2000) {
			return parent.memory->chr[address];
		}
		if (address < 0x3000) {
			return vram[address - 0x2000];
		}
		if (address < 0x3F00) {
			return vram[address - 0x3000];
		}
		if (address < 0x4000) {
			return pal[address - 0x3F00];
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
		reg.control.byte = value;
		reg.address.temp.scroll.nameTableX = (value & 0x1); //t: ...BA.. ........ = d: ......BA
		reg.address.temp.scroll.nameTableY = ((value & 0x2) >> 1);

		if (reg.control.flags.NMI == true && reg.status.flags.VBlankStarted == true) {
			parent.cpu->requestNMI = true;
		}
		break;
	case 0x2001: //PPU Render Flags
		reg.mask.byte = value;
		break;
	case 0x2003: //OAM Read/Write Address
		reg.oamAddress = value;
		break;
	case 0x2004: //OAM data Read/Write
		oam[reg.oamAddress] = value;
		reg.oamAddress += 1;
		break;
	case 0x2005: //Scroll
		if (reg.address.writeLatch == false) {
			reg.address.fineXScroll = value & 0x7;						   // x:              CBA = d: .....CBA
			reg.address.temp.scroll.coarseXScroll = ((value & 0xF8) >> 3); // t: ....... ...HGFED = d : HGFED...
			reg.address.writeLatch = true;
		} else {
			// t: CBA..HG FED..... = d: HGFEDCBA
			reg.address.temp.scroll.fineYScroll = (value & 0x7);
			reg.address.temp.scroll.coarseYScroll = ((value & 0xF8) >> 3);
			reg.address.writeLatch = false;
		}
		break;
	case 0x2006: //PPU Read/Write Address
		if (reg.address.writeLatch == false) {
			reg.address.temp.byte.hi = value;
			reg.address.writeLatch = true;
		}
		else {
			reg.address.temp.byte.lo = value;
			reg.address.current.address = reg.address.temp.address;
			reg.address.writeLatch = false;
		}
		break;
	case 0x2007: { //PPU Data Read/Write
			short address = reg.address.current.address;
			if (address < 0x2000) {
				parent.memory->chr[address] = value;
			}
			else if (address < 0x3000) {
				vram[address - 0x2000] = value;
			}
			else if (address < 0x3F00) {
				vram[address - 0x3000] = value;
			}
			else if (address < 0x4000) {
				pal[address - 0x3F00] = value;
			}
			else {
				//exit(0);
			}
			reg.address.current.address += reg.control.flags.VRAMAddress ? 32 : 1;
		}
		break;
	case 0x4014: // OAM DMA High Address
		copyDMAMemory(value);
		break;
	default:
		exit(0);
		break;
	}
	if (index != 0x4014) {
		reg.status.flags.Address = value & 0x1F;
	}
}

void NES::PPU::copyDMAMemory(unsigned char index) {
	parent.cpu->stallCycles = (parent.cpu->cycles) % 2 == 1 ? 513 : 514;
	for (int i = 0x0; i < 0x100; i++) {
		oam[i] = parent.memory->get((index * 0x100) + i);
	}
	//memcpy(memory.oam, &parent.memory[index * 0x100], 0xFF);
}

void NES::PPU::reset() {
	reg.control.byte = 0x0;
	reg.mask.byte = 0x0;
	reg.status.byte = 0x10;
	reg.oamAddress = 0x0;
	reg.address.current.address = 0x0; 
	reg.address.temp.address = 0x0;
	scrollOffsetX = 0x0;
	scrollOffsetY = 0x0;
	oddFrame = false;
	reg.address.writeLatch = false;
}


void NES::PPU::prepareSprites() {
	Sprite* spriteList = (Sprite*)oam;
	int spriteCount = 0;
	int oamIndex = 0;
	size_t t = sizeof(Sprite);
	memset(spr, 0x0, sizeof(Sprite) * 8);
	while (spriteCount < 8 && oamIndex < 64) {
		if (spriteList->yPosition != 0 &&
			currentScanline >= spriteList->yPosition + 1 &&
			currentScanline < spriteList->yPosition + 1 + 8)
		{
			spr[spriteCount] = *spriteList;
			spriteCount++;
		}
		spriteList++;
		oamIndex++;
	}
}

void NES::PPU::renderPixel() {

	bool renderBackground = reg.mask.flags.ShowBackground == true && (currentCycle > 7 || reg.mask.flags.LeftBackground == true);
	bool renderSprites = reg.mask.flags.ShowSprite == true && (currentCycle > 7 || reg.mask.flags.LeftSprite == true);

	unsigned char* defaultColor = colorTable[pal[0]]; //Default Background Color
	unsigned char* backgroundColor = defaultColor;
	unsigned char* finalColor = defaultColor;

	if (renderBackground == true) {

		unsigned short address = reg.address.current.address;
		unsigned short tileAddress = address & 0x0FFF;
		unsigned short attributeAddress = 0x3C0 | (address & 0x0C00) | ((address >> 4) & 0x38) | ((address >> 2) & 0x07);

		//Palette
		unsigned char paletteInfo = vram[attributeAddress];

		int paletteX = 0; // (backgroundX & 0x1F) >> 4;
		int paletteY = 0; // (backgroundY & 0x1F) >> 4;

		if (paletteX == 0 && paletteY == 0) paletteInfo = (paletteInfo &  0x3);      // Top Left 00
		if (paletteX == 1 && paletteY == 0) paletteInfo = (paletteInfo &  0xF) >> 2; // Top Right 10
		if (paletteX == 0 && paletteY == 1) paletteInfo = (paletteInfo & 0x3F) >> 4; // Bottom Left 01
		if (paletteX == 1 && paletteY == 1) paletteInfo =  paletteInfo >> 6;         // Bottom Right 11

		unsigned char colorIndex = (tileHi & 0x80) >> 6 | (tileLo & 0x80) >> 7;
		if (colorIndex == 0) {
			backgroundColor = colorTable[pal[0]];
		}
		else {
			backgroundColor = colorTable[pal[(0 * 4) + colorIndex]];
		}

		//tileLo <<= 1;
		//tileHi <<= 1;

		/*
		backgroundColor = getTileColor(
			vram[tileAddress] + (reg.control.flags.BackgroundTableAddress ? 0x100 : 0x0),  //Left or Right?
			reg.address.fineXScroll + (currentCycle % 8),
			reg.address.current.scroll.fineYScroll,
			1,
			false,
			false
		);*/

		finalColor = backgroundColor;
	} 

	if (renderSprites == true) {
		int x = currentCycle;
		int y = currentScanline;
		for (int i = 0; i < 8; i++) { 
			Sprite sprite = spr[i];
			unsigned char spriteY = sprite.yPosition + 1;
			unsigned char spriteX = sprite.xPosition;
			
			if (sprite.xPosition != 0 &&
				x >= spriteX &&
				x < spriteX + 8) 
			{
					spriteX = x - spriteX;
					spriteY = y - spriteY;
					
					unsigned char* spriteColor = getTileColor(
						sprite.tileIndex + (reg.control.flags.SpriteTableAddress ? 0x100 : 0x0),
						spriteX, 
						spriteY, 
						sprite.attributes.palette + 0x4,
						sprite.attributes.horizontalFlip, 
						sprite.attributes.verticalFlip
					);
					if (spriteColor != defaultColor) {
						if (i == 0) reg.status.flags.Sprite0Hit = (reg.status.flags.Sprite0Hit || (x != 255 && backgroundColor != defaultColor));
						if (sprite.attributes.priority == 0 || backgroundColor == defaultColor) finalColor = spriteColor;
						break;
					}
			}
		}
	}

	unsigned int combinedColor = finalColor[0] << 16 | finalColor[1] << 8 | finalColor[2];
	parent.graphics[currentCycle + (currentScanline * 256)] = combinedColor;

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
	if (colorIndex == 0) return colorTable[pal[0]];
	return colorTable[pal[(paletteIndex * 4) + colorIndex]];
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

			unsigned char* backgroundColor = colorTable[pal[0]];
			unsigned char* color = colorTable[pal[colorIndex + 6]];

			if (colorIndex == 0) color = backgroundColor;

			unsigned int combinedColor = color[0] << 16 | color[1] << 8 | color[2];
			parent.graphics[x + j + ((y + i) * 256)] = combinedColor;
			tileSliceA = tileSliceA >> 1;
			tileSliceB = tileSliceB >> 1;
		}
	}
}

void NES::PPU::step() {

	cycles += 1;
	currentCycle += 1;

	if (currentCycle == 341) { // Next Scanline
		currentCycle = 0;
		currentScanline++;
		if (currentScanline == 261) { // Next Frame
			frameCount++;
			currentScanline = -1;
			oddFrame = !oddFrame;
			parent.updateGraphics = true;
		}
	}

	if (currentCycle == 0 && currentScanline < 240) prepareSprites();
	
	bool renderingEnabled = (reg.mask.flags.ShowSprite || reg.mask.flags.ShowBackground);
	bool visibleScanline = (currentScanline >= 0 && currentScanline < 240);
	bool preRenderScanline = (currentScanline == -1);

	//Render Pixel During Visible Scanlines
	if (renderingEnabled && visibleScanline && currentCycle >= 0 && currentCycle < 256) {
		renderPixel();
	}

	
	if (renderingEnabled && (visibleScanline || preRenderScanline) && currentCycle == 256) {
		// Update Y
		if (reg.address.current.scroll.fineYScroll < 0x7) {
			reg.address.current.scroll.fineYScroll += 0x1;
		}
		else {
			reg.address.current.scroll.fineYScroll = 0x0;
			if (reg.address.current.scroll.coarseYScroll == 29) {
				reg.address.current.scroll.coarseYScroll = 0x0;
				reg.address.current.scroll.nameTableY ^= 1;
			}
			else if (reg.address.current.scroll.coarseYScroll == 31) {
				reg.address.current.scroll.coarseYScroll = 0x0;
			}
			else {
				reg.address.current.scroll.coarseYScroll += 0x1;
			}
		}
	}



	if (renderingEnabled && (visibleScanline || preRenderScanline) && (currentCycle >= 327 || currentCycle < 256)) {
		int tileAddress = reg.address.current.address & 0x0FFF;
		int tileIndex = vram[tileAddress] + 0x100;
		tileIndex *= 0x10;
		tileIndex += (currentScanline % 8);

		if (preRenderScanline == false || currentScanline < 328) {
			tileLo <<= 1;
			tileHi <<= 1;
		}

		if (currentCycle % 8 == 0) {
			if (reg.address.current.scroll.coarseXScroll == 31) {
				reg.address.current.scroll.coarseXScroll == 0;
				reg.address.current.scroll.nameTableX ^= 1;
			}
			else {
				reg.address.current.scroll.coarseXScroll += 1;
			}
		} else if (currentCycle % 8 == 5) {
			tileLo |= parent.memory->chr[tileIndex];
		} else if (currentCycle % 8 == 7) {
			tileHi |= parent.memory->chr[tileIndex + 8];
		}

	}

	// VBlank Scanlines
	if (currentScanline == 241 && currentCycle == 1 && parent.cpu->cycles > 100) {
		vBlankStart();
	}
	
	if (currentScanline == -1 && currentCycle == 1 && parent.cpu->cycles > 100) {
		vBlankEnd();
	}

	if (renderingEnabled && currentCycle == 257) {
		//v: ....F.. ...EDCBA = t: ....F.. ...EDCBA
		reg.address.current.scroll.coarseXScroll = reg.address.temp.scroll.coarseXScroll;
		reg.address.current.scroll.nameTableX = reg.address.temp.scroll.nameTableX;
	}

	if (preRenderScanline && currentCycle >= 280 && currentCycle < 305) {
		//v: IHGF.ED CBA..... = t : IHGF.ED CBA.....
		reg.address.current.scroll.coarseYScroll = reg.address.temp.scroll.coarseYScroll;
		reg.address.current.scroll.fineYScroll = reg.address.temp.scroll.fineYScroll;
		reg.address.current.scroll.nameTableY = reg.address.temp.scroll.nameTableY;
	}
}

void NES::PPU::vBlankStart() {
    reg.status.flags.VBlankStarted = true;
	if (reg.control.flags.NMI == true) {
		parent.cpu->requestNMI = true;
	}
}

void NES::PPU::vBlankEnd() {
	reg.status.flags.VBlankStarted = false;
	reg.status.flags.Sprite0Hit = false;
}
