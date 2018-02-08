#pragma once
#include "NES.h"

namespace NES {
    class Console;
    class Memory {
    public:
		unsigned char prg[0x8000]   = { 0 }; // Program Data
		unsigned char ram[0x800]    = { 0 }; // System Memory
		unsigned char vram[0x2000]  = { 0 }; // Video Memory/Name Tables 
		unsigned char chr[0x2000]   = { 0 }; // Tile Data
		unsigned char oam[0x100]    = { 0 }; // Object Attribute Memory
		unsigned char pal[0x20]     = { 0x3F }; // Palette Memory. Initialized to black.
        Console &parent;
        
		bool extended = false;

		unsigned char get(unsigned short index);
		void set(unsigned short index, unsigned char value);

        unsigned short resetVector();
        
        Memory(Console &parent): parent(parent) {}
        ~Memory();
    };
};
