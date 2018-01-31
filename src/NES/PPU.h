#pragma once

#include "NES.h"

namespace NES {
    class Console;
    class PPU {
    public:

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
			} status;
			unsigned char byte;
		} PPUCTRL;

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
			} status;
			unsigned char byte;
		} PPUMASK;

		union {
			struct {
				unsigned Address : 5,
					SpriteOverflow : 1,
					Sprite0Hit : 1,
					VBlankStarted : 1;
			} status;
			unsigned char byte;
		} PPUSTATUS;

		union {
			struct {
				unsigned char hi;
				unsigned char lo;
			} byte;
			unsigned short address;
		} PPUADDR;

		unsigned char DMAINDEX;
		unsigned char OAMADDR;
		unsigned char PPUSCROLL;
		bool PPUADDRLATCH;
		bool extended = false;

        struct Memory {
        public:
            bool extended = false;

            PPU &ppu;
            
            unsigned char &operator [](unsigned short index);
            
            Memory(PPU &ppu) : ppu(ppu) {}
        } memory;
        
		struct Sprite {
		public:
			char yPosition;
			char tileIndex;
			char attributeData;
			char xPosition;
			Sprite(char yPosition, 
				   char tileIndex,
				   char attributeData,
				   char xPosition) : 
				yPosition(yPosition), 
				tileIndex(tileIndex),
				attributeData(attributeData),
				xPosition(xPosition) 
			{};
			Sprite() {}
		};

		Sprite spriteList[8];


  
        bool ODDFRAME;
        
		unsigned short lastWrite = 0x0;
		unsigned short lastValue = 0x0;

		unsigned int cycles = 0x0;

		int currentCycle = 0;
		int currentScanline = 241;
		int frameCount = 0;

		bool skipCycle = false;

        Console &parent;

		void processWrite();

		unsigned char getPPURegister(unsigned short index);

		void setPPURegister(unsigned short index, unsigned char value);

		void copyDMAMemory(unsigned char index);
        
        void step();
        void reset();

		void renderScanline();

		void renderPatternTable();

		void renderTile(int x, int y, int tileIndex);
        
        void vBlankStart();
        void vBlankEnd();

        PPU(Console &parent) : parent(parent), memory(*this) {
            reset();
        };
        
        ~PPU();
        
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
            {160, 162, 160}
        };
    };
}
