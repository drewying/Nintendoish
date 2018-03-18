#pragma once

#include "Cartridge.h"
#include "CPU.h"
#include "Controller.h"
#include "Memory.h"
#include "PPU.h"
#include "PPUMemory.h"

#include <stdint.h>
#include <functional>
#include <string>
#include <stdint.h>

namespace NES {
    class APU;
    class Cartridge;
    class CPU;
    class Controller;
    class Memory;
    class PPU;
    class PPUMemory;

    class Console {
    public:
        int emulateCycle();
        void loadProgram(const char* path);
        void reset();

        APU *apu;
        Cartridge *cartridge;
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

