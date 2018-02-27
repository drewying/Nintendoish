#pragma once

#include "NES.h"

namespace NES {
    class Console;
    class PPU {
    public:

		union Address {
			struct {
				unsigned lo : 8,
					     hi : 7;
			} byte;
			struct {
				unsigned coarseXScroll : 5,
					coarseYScroll : 5,
					nameTableX : 1,
					nameTableY : 1,
					fineYScroll : 3;
			} scroll;
			unsigned address : 15;
		};

		//Registers
		struct Registers {
			union {
				struct {
					unsigned BaseAddressHi : 1,
						BaseAddressLo : 1,
						VRAMAddress : 1,
						SpriteTableAddress : 1,
						BackgroundTableAddress : 1,
						SpriteSize : 1,
						MasterSlave : 1,
						NMI : 1;
				} flags;
				unsigned char byte;
			} control;

			union {
				struct {
					unsigned Greyscale : 1,
						LeftBackground : 1,
						LeftSprite : 1,
						ShowBackground : 1,
						ShowSprite : 1,
						EmphasizeRed : 1,
						EmphasizeGreen : 1,
						EmphasizeBlue : 1;
				} flags;
				unsigned char byte;
			} mask;

			union {
				struct {
					unsigned Address : 5,
						SpriteOverflow : 1,
						Sprite0Hit : 1,
						VBlankStarted : 1;
				} flags;
				unsigned char byte;
			} status;
			unsigned char oamAddress;
		} reg;

		//Background
		struct {
			unsigned short tileLo = 0x0;
			unsigned short tileHi = 0x0;
			unsigned char attributeTable = 0x0;
		} shift;

		struct {
			unsigned char tileLo = 0x0;
			unsigned char tileHi = 0x0;
			unsigned char attributeTable = 0x0;
			unsigned char nameTable = 0x0;
		} latch;

		struct {
			Address v;
			Address t;
			unsigned fineXScroll : 3;
			bool writeLatch = false;
		} vramRegister;

		
		struct Sprite {
		public:
			unsigned char yPosition;
			unsigned char tileIndex;
			struct {
				unsigned char palette : 2,
					unused : 3,
					priority : 1,
					horizontalFlip : 1,
					verticalFlip : 1;
			} attributes;
			unsigned char xPosition;
		};


		//Memory
		Sprite*        spr[8]      = { 0 };    // Active Sprites
 		unsigned char vram[0x2000] = { 0 };    // Video Memory/Name Tables
		unsigned char oam[0x100]   = { 0 };    // Object Attribute Memory
		unsigned char pal[0x20]    = { 0x3F }; // Palette Memory. Initialized to black.

		unsigned char getPPURegister(unsigned short index);
		void setPPURegister(unsigned short index, unsigned char value);
		void copyDMAMemory(unsigned char index);

  
		// Emulation
		int cycles = 0x0;
		int currentCycle = 0;
		int currentScanline = 241;
		int frameCount = 0;
		bool oddFrame = false;

		void step();
		void reset();
		void vBlankStart();
		void vBlankEnd();

        Console &parent;

		// Rendering
		void renderPatternTable();
		void renderTile(int x, int y, int tileIndex);
		void prepareSprites();
		void renderPixel();
		
		unsigned char* getTileColor(unsigned int tileIndex, unsigned int tileX, unsigned int tileY, unsigned int paletteIndex, unsigned int flipHorizontal, unsigned int flipVertical);
		

        PPU(Console &parent) : parent(parent) {
            reset();
        };
        
        
        unsigned char colorTable[0x40][0x3] = {
            {84, 84,  84},
            {0,  30, 116},
            {8,  16, 144},
            {48,  0, 136},
            {68,  0, 100},
            {92,  0,  48},
            {84,  4,   0},
            {60, 24,   0},
            {32, 42,   0},
            {8,  58,   0},
            {0,  64,   0},
            {0,  60,   0},
            {0,  50,  60},
            {0,   0,   0},
			{0,   0,   0},
			{0,   0,   0},
            
            {152, 150, 152},
            {8,    76, 196},
            {48,   50, 236},
            {92,   30, 228},
            {136,  20, 176},
            {160,  20, 100},
            {152,  34,  32},
            {120,  60,   0},
            {84,   90,   0},
            {40,  114,   0},
            {8,   124,   0},
            {0,   118,  40},
            {0,   102, 120},
            {0,     0,   0},
			{0,     0,   0},
			{0,     0,   0},
            
            {236, 238, 236},
            {76,  154, 236},
            {120, 124, 236},
            {176,  98, 236},
            {228,  84, 236},
            {236,  88, 180},
            {236, 106, 100},
            {212, 136,  32},
            {160, 170,   0},
            {116, 196,   0},
            {76,  208,  32},
            {56,  204, 108},
            {56,  180, 204},
            {60,   60,  60},
			{ 0,    0,   0},
			{ 0,    0,   0},

            {236, 238, 236},
            {168, 204, 236},
            {188, 188, 236},
            {212, 178, 236},
            {236, 174, 236},
            {236, 174, 212},
            {236, 180, 176},
            {228, 196, 144},
            {204, 210, 120},
            {180, 222, 120},
            {168, 226, 144},
            {152, 226, 180},
            {160, 214, 228},
            {160, 162, 160},
			{0,     0,   0},
			{0,     0,   0}
        };
    };
}
