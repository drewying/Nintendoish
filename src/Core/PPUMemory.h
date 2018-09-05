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
        uint8_t    pal[0x20] = { 0x9,0x1,  0x0, 0x1,0x0, 0x2, 0x2, 0xD, // Palette Memory. Initialized to random values from Blargg's NES
                                 0x8,0x10,0x08,0x24,0x0, 0x0, 0x4,0x2C, 
                                 0x9, 0x1,0x34, 0x3,0x0, 0x4, 0x0,0x14,
                                 0x8,0x3A,0x00,0x02,0x0,0x20,0x2C, 0x8 }; 
    
        uint16_t mirrorIndex(uint16_t index);
    };
};

#endif