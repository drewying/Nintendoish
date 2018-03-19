#pragma once

#include "APU.h"
#include "Cartridge.h"
#include "Controller.h"
#include "CPU.h"
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
    class Controller;
    class CPU;
    class Memory;
    class PPU;
    class PPUMemory;
    
    struct Dot {
        uint8_t* baseColor;
        uint8_t* backgroundColor;
        uint8_t* spriteColor;
        uint8_t priority; //0: Base Color, 1, Background Color, 2 Sprite Color
    };
    
    
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
        
        Dot graphics[256 * 240] = { 0 };

        Console();
        ~Console();
    };
};

