#pragma once

#include "NES.h"
#include <stdint.h>

namespace NES {
    class Console;
    class Memory {
    public:
        Console &parent;
		bool extended = false;
		
		unsigned char get(unsigned short index);
		void  set(unsigned short index, unsigned char value);
        
		unsigned short resetVector(); 
        Memory(Console &parent): parent(parent) {}
     
		unsigned char prg[0x8000] = { 0 }; // Program Data
		unsigned char ram[0x800]  = { 0 }; // System Memory 
    };
};

