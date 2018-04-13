#pragma once

#include "NES.h"
#include <stdint.h>

namespace NES {
    class Console;
    class APU {
    public:
        struct Channel {
            uint16_t timer;
            uint16_t timerLength;
            uint16_t lengthCounter;
            uint8_t volume;
            uint8_t divider;
            uint8_t duty;
            virtual uint8_t sample() { return 0x0; };
            virtual void stepEnvelope() {};
            virtual void stepSweep() {};
            virtual void stepTimer() {};
            virtual void stepLengthCounter() {};
        };
        
        struct Pulse : Channel {
            uint8_t dutyTable[4][8] = {
                { 0, 1, 0, 0, 0, 0, 0, 0 },
                { 0, 1, 1, 0, 0, 0, 0, 0 },
                { 0, 1, 1, 1, 1, 0, 0, 0 },
                { 1, 0, 0, 1, 1, 1, 1, 1 }
            };

            bool haltLengthCounter;
            bool constantVolume;
            uint8_t sequence = 0x0;
            uint8_t timerLow = 0x0;

            uint8_t sample() {
                if (timer < 8 || lengthCounter == 0 || dutyTable[duty][sequence] == 0) {
                    return 0;
                }
                return dutyTable[duty][sequence] * volume;
            }

            void stepEnvelope() {

            }

            void stepSweep() {

            }

            void stepTimer() {
                if (timer == 0) {
                    sequence++;
                    if (sequence == 8) {
                        sequence = 0;
                    }
                    timer = timerLength;
                } else {
                    timer--;
                }
            }

            void stepLengthCounter() {
                if (haltLengthCounter == false && lengthCounter > 0) {
                    lengthCounter--;
                }
            }
        };
        
        APU(Console &console) : console(console) { };

        Console &console;
        
        Pulse pulse1 = Pulse();
        Pulse pulse2 = Pulse();
        
        uint32_t totalCycles = 0;
        uint32_t currentCycle = 0;
        bool stepCycle = false;
        uint8_t frameCounter;


        uint8_t getAPURegister(uint16_t index);
        void setAPURegister(uint16_t index, uint8_t value);

        void step();
        void stepFrameCounter();

        float sample();

        void processPulse(uint16_t index, uint8_t value);
        void processTriangle(uint16_t index, uint8_t value);
        void processNoise(uint16_t index, uint8_t value);
        void processDMC(uint16_t index, uint8_t value);
        void processControl(uint16_t index, uint8_t value);
        
    };
};
 
