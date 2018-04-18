#pragma once

#include "NES.h"
#include <stdint.h>
#include <functional>

namespace NES {
    class Console;
    class APU {
    public:

        struct Divider {
            uint16_t period;
            uint16_t counter;
            void reload() {
                counter = period;
            }
            void tick() {
                if (counter == 0) {
                    reload();
                } else {
                    counter--;
                }
            }
        };

        struct Channel {
            bool enabled = false;
            uint16_t lengthCounter;
            uint8_t duty;
            virtual uint8_t sample() { return 0x0; };
            virtual void stepEnvelope() {};
            virtual void stepSweep() {};
            virtual void stepTimer() {};
            virtual void writeRegister(uint16_t index, uint8_t value) {};
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
            bool useConstantVolume;
            
            Divider timer;

            //Envelope Generator
            bool startEnvelope;
            bool loopEnvelope;
            Divider decayDivider;
            Divider envelopeDivider;
            

            uint8_t volume;
            uint8_t sequencerIndex = 0x0;
            uint8_t periodLowBits = 0x0;
            
            void writeRegister(uint16_t index, uint8_t value) {
                switch (index) {
                case 0x4000:
                    duty = value >> 0x6;
                    haltLengthCounter = (value >> 0x5) & 0x1;
                    loopEnvelope = haltLengthCounter;
                    useConstantVolume = (value >> 0x4) & 0x1;
                    volume = (value & 0xF);
                    envelopeDivider.period = volume;
                    break;
                case 0x4001:
                    //sweep
                    break;
                case 0x4002:
                    //timer low 8 bits
                    periodLowBits = value;
                    break;
                case 0x4003:
                    timer.period = (value & 0x3) << 0x8;
                    timer.period |= periodLowBits;
                    lengthCounter = value >> 0x3;
                    sequencerIndex = 0x0;
                    startEnvelope = true;
                    //TODO restart envelope
                    break;
                default:
                    break;
                }
            };

            uint8_t sample() {
                if (timer.counter < 8 || lengthCounter == 0 || dutyTable[duty][sequencerIndex] == 0 || enabled != true) {
                    return 0;
                } else if (useConstantVolume == true) {
                    return volume;
                } else {
                    return decayDivider.counter;
                }
            }

            void stepEnvelope() {
                if (startEnvelope == false) {
                    envelopeDivider.tick();
                    if (envelopeDivider.counter == 0) {
                        envelopeDivider.period = volume;
                        decayDivider.tick();
                        if (decayDivider.counter == 0) {
                            if (loopEnvelope == false) {
                                decayDivider.period = 0;
                            }
                        }
                    }
                } else {
                    startEnvelope = false;
                    decayDivider.period = 15;
                    decayDivider.reload();
                    envelopeDivider.reload();
                }
            }

            void stepSweep() {

            }

            void stepTimer() {
                timer.tick();
                if (timer.counter == 0) {
                    sequencerIndex++;
                    sequencerIndex %= 8;
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

        void processTriangle(uint16_t index, uint8_t value);
        void processNoise(uint16_t index, uint8_t value);
        void processDMC(uint16_t index, uint8_t value);
        void processControl(uint16_t index, uint8_t value);
        
    };
};
 
