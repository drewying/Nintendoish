#include "PPU.h"
#include <iostream>

using namespace std;

unsigned char &NES::PPU::Memory::operator [](unsigned short index) {
	ppu.lastWrite = index;
    switch (index){
        case 0x2000:
			ppu.lastValue = ppu.PPUCTRL.byte;
			return ppu.PPUCTRL.byte;
            break;
        case 0x2001:
			ppu.lastValue = ppu.PPUMASK.byte;
            return ppu.PPUMASK.byte;
            break;
        case 0x2002:
			ppu.lastValue = ppu.PPUSTATUS.byte;
            return ppu.PPUSTATUS.byte;
            break;
        case 0x2003:
			ppu.lastValue = ppu.OAMADDR;
            return ppu.OAMADDR;
            break;
        case 0x2004:
			ppu.lastValue = oam[ppu.OAMADDR];
            return oam[ppu.OAMADDR];
            break;
        case 0x2005:
			//TODO
			ppu.lastValue = ppu.PPUSCROLL;
            return ppu.PPUSCROLL;
            break;
        case 0x2006:
			ppu.lastValue = ppu.PPUADDR.address;
            if (ppu.PPUADDRLATCH == false) {
                ppu.PPUADDRLATCH = true;
                return ppu.PPUADDR.byte.lo;
            } else {
				ppu.PPUADDRLATCH = false;
                return ppu.PPUADDR.byte.hi;
            }
            break;
        case 0x2007:
            if (ppu.PPUADDR.address < 0x2000) {
                return chr[ppu.PPUADDR.address];
            }
            if (ppu.PPUADDR.address < 0x3000) {
                ppu.PPUADDR.address++;
                return vram[ppu.PPUADDR.address - 0x2001];
            }
            if (ppu.PPUADDR.address < 0x3F00) {
                ppu.PPUADDR.address += 0x1;
                return vram[ppu.PPUADDR.address - 0x3001];
            }
            if (ppu.PPUADDR.address < 0x4000) {
                exit(0);
                //return parent->ppu->pallet[parent->ppu->PPUADDR.address - 0x3F00];
            }
            break;
        case 0x4014:
			ppu.lastValue = ppu.DMAINDEX;
			return ppu.DMAINDEX;
            break;
        default:
            exit(0);
            break;
    }
    
    return chr[0x0];
}

void NES::PPU::copyDMAMemory(unsigned char index) {
	parent.stallCycles = (parent.cpu->cycles - 1) % 2 == 1 ? 514 : 513;
	for (int i = 0x0; i < 0xFF; i++) {
		memory.oam[i] = parent.cpu->memory[(index * 0x100) + i];
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

		printf("Render Sprite\n");
		if (oddCycle == true) {
			// On odd cycles, data is read from (primary) OAM
			if (memory.oam[currentOamIndex] <= (currentScanline + 1) && (memory.oam[currentOamIndex] + 8) >= (currentScanline + 1)) {
				currentSprite = (Sprite*)&memory.oam[currentOamIndex];
				writeFlag = true;
			}
		} else if (writeFlag == true){
			// On even cycles, data is written to secondary OAM
			memcpy(&spriteList[currentSpriteIndex], currentSprite, 4);
			writeFlag = false;
			currentSpriteIndex += 1;
		}
		currentOamIndex += 4;

	}
	if (currentCycle >= 257 && currentCycle < 320) {
		OAMADDR = 0x0;
		//Tile Data
	}
}

void NES::PPU::step() {
	cycles += 1;

	if (skipCycle) {
		skipCycle = false;
		return;
	}

	if (lastWrite != 0x0) {			
		if (parent.cpu->cycles < 29658 &&
		   (lastWrite == 0x2000 ||
			lastWrite == 0x2001 ||
			lastWrite == 0x2005 ||
			lastWrite == 0x2006)
			) {
			memory[lastWrite] = lastValue;
		} else {
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
						parent.triggerNMI = true;
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

	if ((PPUMASK.status.ShowSprite || PPUMASK.status.ShowBackground) && currentScanline >= 0 && currentScanline <= 239 && currentCycle >= 1 && currentCycle <= 340) {
		//skipCycle = true;
	}

    // Render Visible Scanlines
    if (currentScanline >= 0 && currentScanline <= 239) {
		renderScanline();
    }
    
    if (currentScanline == 240) {
        // Do Nothing. Idle Scanline
    }

	currentCycle += 1;
   
	// VBlank Scanlines
    if (currentScanline == 241 && currentCycle == 1 && parent.cpu->cycles > 100) {
        vBlankStart();
    }
    
	
	 if (currentCycle == 341){ // Next Scanline
        currentCycle = 0;
        currentScanline++;
        if (currentScanline == 261) { // Next Frame
            currentScanline = -1;
            ODDFRAME = !ODDFRAME;
            vBlankEnd();
        }
    }
}

void NES::PPU::vBlankStart() {
    PPUSTATUS.status.VBlankStarted = true;
	if (PPUCTRL.status.NMI == true) {
		printf("\n\n\nTRIGGER NMI\n\n\n");
		parent.triggerNMI = true;
	}
}

void NES::PPU::vBlankEnd() {
    PPUSTATUS.status.VBlankStarted = false;
}
