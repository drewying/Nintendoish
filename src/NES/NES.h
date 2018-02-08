#pragma once
#include "CPU.h"
#include "PPU.h"
#include "Memory.h"
#include "Controller.h"

#include <functional>
#include <string>


namespace NES {
    class CPU;
    class Memory;
    class PPU;
	class Controller;
    
    class Console {
	public:
		void emulateCycle();
		void loadProgram(const char* path);
		void reset();

        CPU *cpu;
        Memory *memory;
        PPU *ppu;
		Controller *controllerOne;

		unsigned int graphics[256 * 240] = { 0 };
		unsigned int updateGraphics = false;

		Console();
		~Console();
    };
};

