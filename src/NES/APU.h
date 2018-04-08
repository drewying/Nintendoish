#pragma once

#include "NES.h"
#include <stdint.h>

namespace NES {
    class Console;
    class APU {
    public:
        
        struct Channel {
            uint16_t timer;
            uint16_t lengthCounter;
            uint16_t volume;
            uint8_t divider;
            uint8_t duty;
            uint8_t currentPosition;
            bool haltLengthCounter;
            bool constantVolume;
            //virtual void stepEnvelope();
            //virtual void stepSweep();
            //virtual void stepCounter();
        };
        
        struct Pulse : Channel {};
        
        APU(Console &console) : console(console) { };

        Console &console;
        
        Pulse pulse1 = Pulse();
        Pulse pulse2 = Pulse();
        
        uint32_t totalCycles = 0;
        uint32_t currentCycle = 0;

        uint8_t frameCounter;


        uint8_t getAPURegister(uint16_t index);
        void setAPURegister(uint16_t index, uint8_t value);

        void step();

        void processPulse(uint16_t index, uint8_t value);
        void processTriangle(uint16_t index, uint8_t value);
        void processNoise(uint16_t index, uint8_t value);
        void processDMC(uint16_t index, uint8_t value);
        void processControl(uint16_t index, uint8_t value);
        
    };
};
 
