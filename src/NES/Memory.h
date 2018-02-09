#pragma once
#include "NES.h"

namespace NES {
    class Console;
    class Memory {
    public:
		unsigned char prg[0x8000]   = { 0 }; // Program Data
		unsigned char ram[0x800]    = { 0 }; // System Memory 
		unsigned char chr[0x2000]   = { 0 }; // Tile Data

        Console &parent;
        
		bool extended = false;

		unsigned char get(unsigned short index);
		void set(unsigned short index, unsigned char value);

        unsigned short resetVector();
        
        Memory(Console &parent): parent(parent) {}
        ~Memory();
    };
};
