#pragma once

#ifdef __cplusplus

#include "NES.h"
#include <stdint.h>

namespace NES {
    class PPUMemory {
    public:
        PPUMemory(Console &console) : console(console) {}

        Console &console;

        uint8_t get(uint16_t index);
        void set(uint16_t index, uint8_t value);
        
    private:
        uint8_t  vram[0x800] = { 0 };    // Video Memory/Name Tables
        uint8_t    pal[0x20] = { 0x3F }; // Palette Memory. Initialized to black.
    
        uint16_t mirrorIndex(uint16_t index);
    };
};

#endif
