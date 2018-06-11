#pragma once

#ifdef __cplusplus

#include "NES.h"
#include <stdint.h>

namespace NES {
    class Console;
    class Memory {
    public:		
        Memory(Console &console) : console(console) {}

        Console &console;

        uint8_t ram[0x800] = { 0 }; // System Memory
        
        uint8_t get(uint16_t index);
        void  set(uint16_t index, uint8_t value);
        uint16_t resetVector();
    };
};

#endif
