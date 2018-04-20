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
            bool enabled = true;
            void reload() {
                counter = period;
            }
            void tick() {
                if (enabled == false) {
                    return;
                }
                if (counter == 0) {
                    reload();
                } else {
                    counter--;
                }
            }
        };

        struct Channel {
            uint8_t lengthCounterTable[0x20] = {
                 10, 254, 20,  2, 40,  4, 80,  6,
                160,   8, 60, 10, 14, 12, 26, 14,
                 12,  16, 24, 18, 48, 20, 96, 22,
                192,  24, 72, 26, 16, 28, 32, 30
            };

            bool enabled = false;
            bool lengthCounterEnabled = false;
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

            //Sweep
            Divider sweepDivider;
            bool sweepEnable = false;
            bool sweepReload = false;
            bool sweepNegate = false;
            uint8_t sweepShift = 0x0;

            //Envelope
            bool envelopeReload;
            bool envelopeLoop;
            Divider envelopeVolume;
            Divider envelopeDivider;
            

            uint8_t volume;
            uint8_t sequencerIndex = 0x0;
            uint8_t periodLowBits = 0x0;

            Pulse() {
                envelopeVolume.period = 15;
            }
            
            void writeRegister(uint16_t index, uint8_t value) {
                switch (index) {
                case 0x4000:
                    duty = value >> 0x6;
                    haltLengthCounter = (value >> 0x5) & 0x1;
                    envelopeLoop = haltLengthCounter;
                    useConstantVolume = (value >> 0x4) & 0x1;
                    volume = (value & 0xF);
                    envelopeDivider.period = volume;
                    break;
                case 0x4001:
                    //Sweep
                    sweepEnable = ((value & 0x80) == 0x80);
                    sweepDivider.period = (value & 0x70) >> 4;
                    sweepNegate = ((value & 0x8) == 0x8);
                    sweepReload = true;
                    sweepShift = value & 0x7;
                    break;
                case 0x4002:
                    //Timer low 8 bits
                    periodLowBits = value;
                    break;
                case 0x4003:
                    timer.period = (value & 0x3) << 0x8;
                    timer.period |= periodLowBits;
                    lengthCounter = lengthCounterTable[value >> 0x3];
                    sequencerIndex = 0x0;
                    envelopeReload = true;
                    break;
                default:
                    break;
                }
            };

            uint8_t sample() {
                if (timer.period < 8 || lengthCounter == 0 || timer.period > 0x7FF || dutyTable[duty][sequencerIndex] == 0 || enabled == false) {
                    return 0;
                } else if (useConstantVolume == true) {
                    return volume;
                } else {
                    return envelopeVolume.counter;
                }
            }

            void stepEnvelope() {
                if (envelopeReload == true) {
                    envelopeReload = false;
                    envelopeVolume.enabled = true;
                    envelopeVolume.reload();
                    envelopeDivider.reload();
                } else {
                    if (envelopeDivider.counter == 0) {
                        envelopeDivider.period = volume;
                        if (envelopeVolume.counter == 0) {
                            if (envelopeLoop == false) {
                                envelopeVolume.enabled = false;
                            } 
                        }
                        envelopeVolume.tick();
                    }
                    envelopeDivider.tick();
                }
            }

            void stepSweep() {
                if (sweepReload == true) {
                    sweepReload = false;
                    sweepDivider.reload();
                } else {
                    if (sweepDivider.counter == 0 && sweepEnable == true) {
                        uint16_t change = timer.period >> sweepShift;
                        if (sweepNegate == true) {
                            timer.period -= change;
                        } else {
                            timer.period += change;
                        }
                    }
                    sweepDivider.tick();
                }
            }

            void stepTimer() {
                if (timer.counter == 0) {
                    sequencerIndex++;
                    sequencerIndex %= 8;
                }
                timer.tick();
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
 
