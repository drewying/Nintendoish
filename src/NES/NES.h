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

        const static unsigned int CPU_CLOCK_RATE = 1789773;
        const static unsigned int AUDIO_SAMPLE_RATE = 44100;
        const static unsigned int AUDIO_BUFFER_SIZE = 736 * 60; //60 frames worth of audio buffer
        unsigned int audioBufferLength = 0x0;

        unsigned int displayBuffer[256 * 240] = { 0 };
        float audioBuffer[AUDIO_BUFFER_SIZE] = { 0 }; //2 seconds of audio
        
        Console();
        ~Console();
    };
};

