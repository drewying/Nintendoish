#pragma once

#ifdef __cplusplus

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

        const static unsigned int CPU_CLOCK_RATE = 1789773;
        const static unsigned int AUDIO_SAMPLE_RATE = 44100;
        const static unsigned int AUDIO_BUFFER_SIZE = 44100; //1 second worth of audio buffer
        const static unsigned int DISPLAY_BUFFER_SIZE = 256 * 240; //NES resolution
        
        unsigned int audioBufferLength = 0x0;
        float audioBuffer[AUDIO_BUFFER_SIZE] = { 0 };
        
        unsigned int displayBuffer[DISPLAY_BUFFER_SIZE] = { 0 };
        
        
        Console();
        ~Console();
    };
};

#endif

