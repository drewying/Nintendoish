#pragma once

#include "NES.h"
#include <stdint.h>

namespace NES {
    class Console;
    class APU {
    public:
        APU(Console &console) : console(console) { };

        Console &console;

        uint32_t totalCycles = 0;
        uint32_t currentCycle = 0;

        uint8_t frameCounter;


        uint8_t getAPURegister(uint16_t index);
        void setAPURegister(uint16_t index, uint8_t value);

        void step();

    };
};
 