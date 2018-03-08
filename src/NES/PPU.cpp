#include "PPU.h"
#include <iostream>

using namespace std;

uint8_t NES::PPU::getPPURegister(uint16_t index) {
	uint8_t status;
	uint16_t address;
	static uint8_t readBuffer = 0x0;
	switch (index) {
	case 0x2002: //PPU Status Flags
		status = reg.status.byte;
		reg.status.flags.VBlankEnabled = false;
		vramRegister.writeLatch = false;
		return status;
		break;
	case 0x2004: //OAM data Read/Write
		return oam[reg.oamAddress];
		break;
	case 0x2007: //PPU Data Read/Write
		address = vramRegister.v.address;
		vramRegister.v.address += reg.control.flags.VRAMAddressIncrement ? 32 : 1;
		
		if (address % 0x4000 < 0x3F00) {
			uint8_t value = readBuffer;
			readBuffer = console.ppuMemory->get(address);
			return value;
		} else {
			readBuffer = console.ppuMemory->get(address - 0x1000);
			return console.ppuMemory->get(address);
		}
		break;
	default:
		break;
	}
}

void NES::PPU::setPPURegister(uint16_t index, uint8_t value) {
	switch (index) {
	case 0x2000: //PPU Control Flags
		reg.control.byte = value;
		vramRegister.t.scroll.nameTableX = (value & 0x1); //t: ...BA.. ........ = d: ......BA
		vramRegister.t.scroll.nameTableY = ((value & 0x2) >> 1);

		if (reg.control.flags.NMI == true && reg.status.flags.VBlankEnabled == true) {
			console.cpu->requestNMI = true;
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
			uint16_t address = vramRegister.v.address;
			console.ppuMemory->set(address, value);
			vramRegister.v.address += reg.control.flags.VRAMAddressIncrement ? 32 : 1;
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
		reg.status.flags.LastWrite = value & 0x1F;
	}
}

void NES::PPU::copyDMAMemory(uint8_t index) {
	console.cpu->stallCycles = (console.cpu->cycles) % 2 == 1 ? 513 : 514;
	for (int i = 0x0; i < 0x100; i++) {
		oam[i] = console.memory->get((index * 0x100) + i);
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
	unsigned int spriteCount = 0;
	unsigned int oamIndex = 0;
	std::fill_n(spr, 8, nullptr);
	if (reg.control.flags.TallSprites == true) {
		printf("Ruh Roh. Tall Sprites not working\n");
	}
	while (spriteCount < 8 && oamIndex < 64) {
		if (sprite->yPosition != 0 &&
			currentScanline >= sprite->yPosition &&
			currentScanline < sprite->yPosition + 8)
		{
			spr[spriteCount] = sprite;
			spriteCount++;
		}
		sprite++; // Pointer arithmatic FTW!
		oamIndex++;
	}
}

void NES::PPU::renderPixel() {

	unsigned int x = currentCycle - 1;
	unsigned int y = currentScanline;
	
	uint16_t backgoundColor = 0x0;
	uint16_t spriteColor = 0x0;

	bool renderBackground = reg.mask.flags.RenderBackground == true && (x > 7 || reg.mask.flags.RenderLeftBackground == true);
	bool renderSprites = reg.mask.flags.RenderSprites == true && (x > 7 || reg.mask.flags.RenderLeftSprites == true);
	bool backgroundPriority = false;

	if (renderBackground == true) {

		//Tile
		uint16_t mask = 0x8000 >> vramRegister.fineXScroll;
		uint16_t backgoundColorIndex = (shift.tileHi & mask) >> (14 - vramRegister.fineXScroll) | (shift.tileLo & mask) >> (15 - vramRegister.fineXScroll);
		uint16_t backgoundPalette = (shift.attributeTableHi & mask) >> (14 - vramRegister.fineXScroll) | (shift.attributeTableLo & mask) >> (15 - vramRegister.fineXScroll);
		if (backgoundColorIndex != 0x0) backgoundColor = backgoundPalette * 4 + backgoundColorIndex;
	} 

	if (renderSprites == true) {
		for (int i = 0; i < 8; i++) { 
			Sprite* sprite = spr[i];

			if (sprite != nullptr &&
				x >= sprite->xPosition &&
				x  < (sprite->xPosition + 8))
			{
					uint8_t spriteX = x - sprite->xPosition;
					uint8_t spriteY = y - (sprite->yPosition + 1);
					uint16_t tileIndex = sprite->tileIndex + (reg.control.flags.SpriteTableSelect ? 0x100 : 0x0);
					tileIndex *= 0x10;
					tileIndex += sprite->attributes.verticalFlip ? 7 - spriteY : spriteY;
	
					uint8_t tileSliceA = console.ppuMemory->get(tileIndex);
					uint8_t tileSliceB = console.ppuMemory->get(tileIndex + 8);

					tileSliceA = tileSliceA >> (sprite->attributes.horizontalFlip ? spriteX : 7 - spriteX);
					tileSliceB = tileSliceB >> (sprite->attributes.horizontalFlip ? spriteX : 7 - spriteX);

					uint16_t spriteColorIndex = (tileSliceB & 0x1) << 1 | (tileSliceA & 0x1);
					uint16_t spritePalette = sprite->attributes.palette + 0x4;
					if (spriteColorIndex != 0x0) spriteColor = spritePalette * 4 + spriteColorIndex;

					backgroundPriority = sprite->attributes.priority == 1;

					//Check for sprite 0 hit.
					reg.status.flags.Sprite0Hit = reg.status.flags.Sprite0Hit || (sprite == (Sprite*)oam && spriteColor != 0x0 && backgoundColor != 0x0 && currentCycle != 255);
					if (spriteColor != 0x0) break;
			}
		}
	}
	
	uint8_t* finalColor = colorTable[console.ppuMemory->get(0x3F00)]; //Default Color
	if (backgoundColor != 0x0) finalColor = colorTable[console.ppuMemory->get(0x3F00 | backgoundColor)];
	if (spriteColor != 0x0 && (backgroundPriority == false || backgoundColor == 0x0)) finalColor = colorTable[console.ppuMemory->get(0x3F00 | spriteColor)];

	unsigned int combinedColor = finalColor[0] << 16 | finalColor[1] << 8 | finalColor[2];
	console.graphics[x + (y * 256)] = combinedColor;
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
		uint8_t tileSliceA = console.ppuMemory->get(tileIndex + i);
		uint8_t tileSliceB = console.ppuMemory->get(tileIndex + i + 8);
		for (int j = 7; j >= 0; j--) {
			uint16_t colorIndex = (tileSliceB & 0x1) << 1 | (tileSliceA & 0x1);

			uint8_t* backgroundColor = colorTable[console.ppuMemory->get(0x3F00)];
			uint8_t* color = colorTable[console.ppuMemory->get(0x3F00 | (colorIndex + 6))];

			if (colorIndex == 0) color = backgroundColor;

			unsigned int combinedColor = color[0] << 16 | color[1] << 8 | color[2];
			console.graphics[x + j + ((y + i) * 256)] = combinedColor;
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
			console.updateGraphics = true;
		}
	}

	bool renderingEnabled = reg.mask.flags.RenderSprites || reg.mask.flags.RenderBackground;
	bool visibleScanline = currentScanline >= 0 && currentScanline < 240;
	bool preRenderScanline = currentScanline == -1;
	bool renderScanline = visibleScanline || preRenderScanline;
	bool visibleCycle = currentCycle >= 1 && currentCycle <= 256;
	bool preRenderCycle = (currentCycle >= 321 && currentCycle <= 336);

	// Skip 0,0 on odd frames
	if (oddFrame && reg.mask.flags.RenderBackground && currentCycle == 0 && currentScanline == 0) {
		return;
	}

	// Prepare Sprites
	if (visibleScanline && currentCycle == 257) prepareSprites();

	if (renderingEnabled && renderScanline) {
		if (visibleCycle || preRenderCycle) {

			//Update X Scroll
			if (currentCycle % 8 == 0) {
				if (vramRegister.v.scroll.coarseXScroll == 31) {
					vramRegister.v.scroll.coarseXScroll = 0x0;
					vramRegister.v.scroll.nameTableX ^= 1;
				} else {
					vramRegister.v.scroll.coarseXScroll += 1;
				}
			}
			
			//Fetch name table Byte
			if (currentCycle % 8 == 1) {
				uint16_t index = 0x2000 | (vramRegister.v.address & 0x0FFF);
				latch.nameTable = console.ppuMemory->get(index);
			}
			
			//Fetch attribute table byte
			if (currentCycle % 8 == 3) {
				uint16_t address = vramRegister.v.address;
				uint8_t attributeTable = console.ppuMemory->get(0x23C0 | (address & 0x0C00) | ((address >> 4) & 0x38) | ((address >> 2) & 0x07));
				
				unsigned int paletteX = (vramRegister.v.scroll.coarseXScroll & 0x3) >> 1;
				unsigned int paletteY = (vramRegister.v.scroll.coarseYScroll & 0x3) >> 1;

				if (paletteX == 0 && paletteY == 0) attributeTable = (attributeTable & 0x3);       // Top Left 00
				if (paletteX == 1 && paletteY == 0) attributeTable = (attributeTable & 0xF) >> 2;  // Top Right 10
				if (paletteX == 0 && paletteY == 1) attributeTable = (attributeTable & 0x3F) >> 4; // Bottom Left 01
				if (paletteX == 1 && paletteY == 1) attributeTable = attributeTable >> 6;          // Bottom Right 11
				
				latch.attributeTable = attributeTable;
			}

			//Fetch lo tile byte
			if (currentCycle % 8 == 5) {
				uint16_t index = latch.nameTable + (reg.control.flags.BackgroundTableSelect ? 0x100 : 0);
				index *= 0x10;
				index += vramRegister.v.scroll.fineYScroll;
				latch.tileLo = console.ppuMemory->get(index);
			}
			
			//Fetch hi tile byte
			if (currentCycle % 8 == 7) {
				uint16_t index = latch.nameTable + (reg.control.flags.BackgroundTableSelect ? 0x100 : 0);
				index *= 0x10;
				index += vramRegister.v.scroll.fineYScroll;
				latch.tileHi = console.ppuMemory->get(index + 8);
			}
		}

		//The background shift registers shift during each of dots 2...257 and 322...337, inclusive.
		if ((currentCycle >= 2 && currentCycle <= 257) || (currentCycle >= 322 && currentCycle <= 337)) {
			shift.tileLo <<= 1;
			shift.tileHi <<= 1;
			shift.attributeTableLo <<= 1;
			shift.attributeTableHi <<= 1;

			// The lower 8 bits are reloaded into background shift registes at ticks 9, 17, 25, ..., 257 and ticks 329, and 337.
			if (currentCycle % 8 == 1) {
				shift.tileLo |= latch.tileLo;
				shift.tileHi |= latch.tileHi;
				shift.attributeTableLo |= (latch.attributeTable & 0x1) * 0xFF;
				shift.attributeTableHi |= ((latch.attributeTable & 0x2) >> 1) * 0xFF;
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
			vramRegister.v.scroll.nameTableX    = vramRegister.t.scroll.nameTableX;
		}
	}

	//Render Pixel During Visible Scanlines
	if (renderingEnabled && visibleScanline && visibleCycle) {
		renderPixel();
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
    reg.status.flags.VBlankEnabled = true;
	if (reg.control.flags.NMI == true) {
		console.cpu->requestNMI = true;
	}
}

void NES::PPU::vBlankEnd() {
	reg.status.flags.VBlankEnabled = false;
	reg.status.flags.Sprite0Hit = false;
}
