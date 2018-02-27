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
		vramRegister.writeLatch = false;
		return status;
		break;
	case 0x2004: //OAM data Read/Write
		return oam[reg.oamAddress];
		break;
	case 0x2007: //PPU Data Read/Write
		address = vramRegister.v.address;
		vramRegister.v.address += reg.control.flags.VRAMAddress ? 32 : 1;

		if (address < 0x2000) {
			return parent.memory->chr[address];
		}
		if (address < 0x3000) {
			//TODO Proper Mirroring
			if (address < 0x2800) {
				return vram[address - 0x2000];
			}
			else {
				return vram[address - 0x2800];
			}
		}
		if (address < 0x3F00) {
			return vram[address - 0x3000];
		}
		if (address < 0x4000) {
			return pal[address - 0x3F00];
		}
		//exit(0);
		break;
	default:
		//exit(0);
		break;
	}
}

void NES::PPU::setPPURegister(unsigned short index, unsigned char value) {
	switch (index) {
	case 0x2000: //PPU Control Flags
		reg.control.byte = value;
		vramRegister.t.scroll.nameTableX = (value & 0x1); //t: ...BA.. ........ = d: ......BA
		vramRegister.t.scroll.nameTableY = ((value & 0x2) >> 1);

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
		if (vramRegister.writeLatch == false) {
			vramRegister.fineXScroll = value & 0x7;						   // x:              CBA = d: .....CBA
			vramRegister.t.scroll.coarseXScroll = ((value & 0xF8) >> 3);   // t: ....... ...HGFED = d : HGFED...
		} else {
			// t: CBA..HG FED..... = d: HGFEDCBA
			vramRegister.t.scroll.fineYScroll = (value & 0x7);
			vramRegister.t.scroll.coarseYScroll = ((value & 0xF8) >> 3);
		}
		vramRegister.writeLatch = !vramRegister.writeLatch;
		break;
	case 0x2006: //PPU Read/Write Address
		if (vramRegister.writeLatch == false) {
			vramRegister.t.byte.hi = value;
		}
		else {
			vramRegister.t.byte.lo = value;
			vramRegister.v.address = vramRegister.t.address;
		}
		vramRegister.writeLatch = !vramRegister.writeLatch;
		break;
	case 0x2007: { //PPU Data Read/Write
			short address = vramRegister.v.address;
			if (address < 0x2000) {
				parent.memory->chr[address] = value;
			} else if (address < 0x3000) {
				//TODO Proper Mirroring
				if (address < 0x2800) {
					vram[address - 0x2000] = value;
				} else {
					vram[address - 0x2800] = value;
				}
			} else if (address < 0x3F00) {
				vram[address - 0x3000] = value;
			} else if (address < 0x4000) {
				pal[address - 0x3F00] = value;
			} else {
				//exit(0);
			}
			vramRegister.v.address += reg.control.flags.VRAMAddress ? 32 : 1;
		}
		break;
	case 0x4014: // OAM DMA High Address
		copyDMAMemory(value);
		break;
	default:
		//exit(0);
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
	vramRegister.v.address = 0x0; 
	vramRegister.t.address = 0x0;
	oddFrame = false;
	vramRegister.writeLatch = false;
}


void NES::PPU::prepareSprites() {
	Sprite* sprite = (Sprite*)oam;
	int spriteCount = 0;
	int oamIndex = 0;
	std::fill_n(spr, 8, nullptr);
	while (spriteCount < 8 && oamIndex < 64) {
		if (sprite->yPosition != 0 &&
			currentScanline >= sprite->yPosition + 1 &&
			currentScanline < sprite->yPosition + 1 + 8)
		{
			spr[spriteCount] = sprite;
			spriteCount++;
		}
		sprite++; // Pointer arithmatic FTW!
		oamIndex++;
	}
}

void NES::PPU::renderPixel() {

	unsigned char x = currentCycle - 1;
	unsigned char y = currentScanline;

	bool renderBackground = reg.mask.flags.ShowBackground == true && (x > 7 || reg.mask.flags.LeftBackground == true);
	bool renderSprites = reg.mask.flags.ShowSprite == true && (x > 7 || reg.mask.flags.LeftSprite == true);


	unsigned short backgoundPalette = 0x0;
	unsigned short backgoundColorIndex = 0x0;
	unsigned short spritePalette = 0x0;
	unsigned short spriteColorIndex = 0x0;
	bool backgroundPriority = false;

	if (renderBackground == true) {

		//Tile
		unsigned short mask = 0x8000 >> vramRegister.fineXScroll;
		backgoundColorIndex = (shift.tileHi & mask) >> (14 - vramRegister.fineXScroll) | (shift.tileLo & mask) >> (15 - vramRegister.fineXScroll);

		//Palette
		backgoundPalette = shift.attributeTable;
		int paletteX = (x & 0x1F) >> 4;
		int paletteY = (y & 0x1F) >> 4;

		if (paletteX == 0 && paletteY == 0) backgoundPalette = (backgoundPalette &  0x3);      // Top Left 00
		if (paletteX == 1 && paletteY == 0) backgoundPalette = (backgoundPalette &  0xF) >> 2; // Top Right 10
		if (paletteX == 0 && paletteY == 1) backgoundPalette = (backgoundPalette & 0x3F) >> 4; // Bottom Left 01
		if (paletteX == 1 && paletteY == 1) backgoundPalette = backgoundPalette >> 6;          // Bottom Right 11
		
		
		
	} 

	if (renderSprites == true) {
		for (int i = 0; i < 8; i++) { 
			Sprite* sprite = spr[i];

			if (sprite != nullptr &&
				x >= (sprite->xPosition + 1) &&
				x < (sprite->xPosition + 9))
			{
					unsigned char spriteX = x - (sprite->xPosition + 1);
					unsigned char spriteY = y - (sprite->yPosition + 1);
					unsigned short tileIndex = sprite->tileIndex + (reg.control.flags.SpriteTableAddress ? 0x100 : 0x0);
					tileIndex *= 0x10;
					tileIndex += sprite->attributes.verticalFlip ? 8 - spriteY : spriteY;
	
					unsigned char tileSliceA = parent.memory->chr[tileIndex];
					unsigned char tileSliceB = parent.memory->chr[tileIndex + 8];

					tileSliceA = tileSliceA >> (sprite->attributes.horizontalFlip ? spriteX : 7 - spriteX);
					tileSliceB = tileSliceB >> (sprite->attributes.horizontalFlip ? spriteX : 7 - spriteX);

					spriteColorIndex = (tileSliceB & 0x1) << 1 | (tileSliceA & 0x1);
					spritePalette = sprite->attributes.palette + 0x4;
					
					backgroundPriority = sprite->attributes.priority == 1;

					//Check for sprite 0 hit.
					reg.status.flags.Sprite0Hit = reg.status.flags.Sprite0Hit || (sprite == (Sprite*)oam && spriteColorIndex != 0x0 && backgoundColorIndex != 0x0 && x != 255);
					if (spriteColorIndex != 0x0) break;
			}
		}
	}
	unsigned char* finalColor = colorTable[pal[0]]; //Default Color
	if (backgoundColorIndex != 0x0) finalColor = colorTable[pal[(backgoundPalette * 4) + backgoundColorIndex]];
	if (spriteColorIndex != 0x0 && backgroundPriority == false) finalColor = colorTable[pal[(spritePalette * 4) + spriteColorIndex]];

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
			oddFrame ^= 1;
			parent.updateGraphics = true;
		}
	}

	bool renderingEnabled = reg.mask.flags.ShowSprite || reg.mask.flags.ShowBackground;
	bool visibleScanline = currentScanline >= 0 && currentScanline < 240;
	bool preRenderScanline = currentScanline == -1;
	bool renderScanline = visibleScanline || preRenderScanline;
	bool visibleCycle = currentCycle >= 1 && currentCycle <= 256;
	bool preRenderCycle = (currentCycle >= 321 && currentCycle <= 336);

	// Prepare Sprites
	if (visibleScanline && currentCycle == 0) prepareSprites();


	//Skip 0,0 on odd frames
	if (reg.mask.flags.ShowBackground && currentCycle == 0 && currentScanline == 0) {
		return;
	}

	//Render Pixel During Visible Scanlines
	if (renderingEnabled && visibleScanline && visibleCycle) {
		renderPixel();
	}

	if (renderingEnabled && renderScanline) {
		if (visibleCycle || preRenderCycle) {
			if (currentCycle % 8 == 0) {
				//Update X Scroll
				if (vramRegister.v.scroll.coarseXScroll == 31) {
					vramRegister.v.scroll.coarseXScroll = 0x0;
					vramRegister.v.scroll.nameTableX ^= 1;
				} else {
					vramRegister.v.scroll.coarseXScroll += 1;
				}
			}
			//Fetch name table Byte
			if (currentCycle % 8 == 1) {
				unsigned short index = vramRegister.v.address & 0x0FFF;
				//TODO Proper Mirroring
				if (index < 0x800) {
					latch.nameTable = vram[index];
				}
				else {
					latch.nameTable = vram[index - 0x800];
				}
			}
			//Fetch attribute table byte
			if (currentCycle % 8 == 3) {
				//TODO mirroring
				unsigned short address = vramRegister.v.address;
				latch.attributeTable = vram[0x3C0 | (address & 0x0C00) | ((address >> 4) & 0x38) | ((address >> 2) & 0x07)];
			}
			//Fetch lo tile byte
			if (currentCycle % 8 == 5) {
				short index = latch.nameTable + (reg.control.flags.BackgroundTableAddress ? 0x100 : 0);
				index *= 0x10;
				index += vramRegister.v.scroll.fineYScroll;
				latch.tileLo = parent.memory->chr[index];
			}
			//Fetch hi tile byte
			if (currentCycle % 8 == 7) {
				short index = latch.nameTable + (reg.control.flags.BackgroundTableAddress ? 0x100 : 0);
				index *= 0x10;
				index += vramRegister.v.scroll.fineYScroll;
				latch.tileHi = parent.memory->chr[index + 8];
			}
		}

		//The background shift registers shift during each of dots 2...257 and 322...337, inclusive.
		if ((currentCycle >= 2 && currentCycle <= 257) || (currentCycle >= 322 && currentCycle <= 337)) {
			shift.tileLo <<= 1;
			shift.tileHi <<= 1;
			// The lower 8 bits are reloaded into background shift registes at ticks 9, 17, 25, ..., 257 and ticks 329, and 337.
			if (currentCycle % 8 == 1) {
				shift.tileLo |= latch.tileLo;
				shift.tileHi |= latch.tileHi;
				shift.attributeTable = latch.attributeTable;
			}
		}

		if (currentCycle == 256) {
			// Update Y Scroll
			if (vramRegister.v.scroll.fineYScroll < 0x7) {
				vramRegister.v.scroll.fineYScroll += 0x1;
			}
			else {
				vramRegister.v.scroll.fineYScroll = 0x0;
				if (vramRegister.v.scroll.coarseYScroll == 29) {
					vramRegister.v.scroll.coarseYScroll = 0x0;
					vramRegister.v.scroll.nameTableY ^= 1;
				}
				else if (vramRegister.v.scroll.coarseYScroll == 31) {
					vramRegister.v.scroll.coarseYScroll = 0x0;
				}
				else {
					vramRegister.v.scroll.coarseYScroll += 0x1;
				}
			}
		}

		if (currentCycle == 257) {
			//Copy X: v: ....F.. ...EDCBA = t: ....F.. ...EDCBA
			vramRegister.v.scroll.coarseXScroll = vramRegister.t.scroll.coarseXScroll;
			//vramRegister.v.scroll.coarseXScroll = 5;
			vramRegister.v.scroll.nameTableX    = vramRegister.t.scroll.nameTableX;
		}
	}

	// VBlank Scanlines
	if (currentScanline == 241 && currentCycle == 1) {
		vBlankStart();
	}

	if (preRenderScanline) {
		if (currentCycle == 1) {
			vBlankEnd();
		}

		if (renderingEnabled && currentCycle >= 280 && currentCycle <= 304) {
			// Copy Y: v: IHGF.ED CBA..... = t : IHGF.ED CBA.....
			vramRegister.v.scroll.coarseYScroll = vramRegister.t.scroll.coarseYScroll;
			vramRegister.v.scroll.fineYScroll   = vramRegister.t.scroll.fineYScroll;
			vramRegister.v.scroll.nameTableY    = vramRegister.t.scroll.nameTableY;
		}
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
