#pragma once

#include "CPU.h"
#include "Controller.h"
#include "Memory.h"
#include "PPUMemory.h"

#include <functional>
#include <string>
#include <stdint.h>

namespace NES {
    class CPU;
    class Memory;
    class PPU;
	class Controller;
	class PPUMemory;

    class Console {
	public:
		void emulateCycle();
		void loadProgram(const char* path);
		void reset();

        CPU *cpu;
        Memory *memory;
        PPU *ppu;
		PPUMemory *ppuMemory;
		Controller *controllerOne;

		unsigned int graphics[256 * 240] = { 0 };
		unsigned int updateGraphics = false;

		Console();
		~Console();
    };
};

