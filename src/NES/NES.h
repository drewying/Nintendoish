#pragma once
#include "CPU.h"
#include "PPU.h"
#include "Memory.h"

#include <functional>
#include <string>


namespace NES {
    class CPU;
    class Memory;
    class PPU;
    
    class Console {
	public:
		void emulateCycle();
		void loadProgram(const char* path);
		void reset();

        CPU *cpu;
        Memory *memory;
        PPU *ppu;

		unsigned int graphics[256 * 240] = { 0 };

		unsigned char controllerOne = 0x40;
		unsigned char controllerTwo = 0x40;

		Console();
		~Console();
    };
};

