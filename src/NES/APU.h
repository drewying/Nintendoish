#pragma once

#include "NES.h"
#include <stdint.h>
#include <functional>

namespace NES {
    class Console;
    class APU {
    public:

        struct Divider {
            uint16_t period = 0;
            uint16_t counter = 0;
            bool loopCounter = true;
            void reload() {
                counter = period;
            }
            void tick() {
                if (counter == 0 && loopCounter == true) {
                    reload();
                } else if (counter > 0) {
                    counter--;
                }
            }
        };
        
        struct Envelope {
            bool startFlag = false;
            bool useConstantVolume = false;
            Divider decay;
            Divider divider;
            
            Envelope() {
                decay.period = 15;
                decay.loopCounter = false;
            }
            void tick() {
                if (startFlag == false) {
                    if (divider.counter == 0) {
                        decay.tick();
                    }
                    divider.tick();
                } else {
                    startFlag = false;
                    divider.reload();
                    decay.reload();
                }
            }
            void setVolume(uint16_t volume) {
                divider.period = volume;
            }
            uint16_t getVolume() {
                if (useConstantVolume) {
                    return divider.period;
                } else {
                    return decay.counter;
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
            bool haltLengthCounter = false;
            uint16_t lengthCounter;
            uint8_t duty;
            virtual uint8_t sample() { return 0x0; };
            virtual void stepEnvelope() {};
            virtual void stepTimer() {};
            virtual void writeRegister(uint16_t index, uint8_t value) {};
            void setLengthCounter(uint8_t value) {
                lengthCounter = lengthCounterTable[value >> 0x3];
            }
            void stepLengthCounter() {
                if (haltLengthCounter == false && lengthCounter > 0) {
                    lengthCounter--;
                }
            }
        };

        struct Pulse : Channel {
            uint8_t dutyTable[4][8] = {
                { 0, 1, 0, 0, 0, 0, 0, 0 },
                { 0, 1, 1, 0, 0, 0, 0, 0 },
                { 0, 1, 1, 1, 1, 0, 0, 0 },
                { 1, 0, 0, 1, 1, 1, 1, 1 }
            };

            Divider timer;

            //Sweep
            Divider sweepDivider;
            bool sweepEnable = false;
            bool sweepReload = false;
            bool sweepNegate = false;
            uint8_t sweepShift = 0x0;

            //Envelope
            Envelope envelope;

            uint8_t sequencerIndex = 0x0;

            void writeRegister(uint16_t index, uint8_t value) {
                switch (index) {
                case 0x4000:
                    duty = value >> 0x6;
                    haltLengthCounter = (value >> 0x5) & 0x1;
                    envelope.decay.loopCounter = haltLengthCounter;
                    envelope.useConstantVolume = (value >> 0x4) & 0x1;
                    envelope.setVolume(value & 0xF);
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
                    timer.period = (timer.period & 0xFF00) | value;
                    break;
                case 0x4003:
                    timer.period = (timer.period & 0x00FF) | ((value & 7) << 8);
                    setLengthCounter(value >> 0x3);
                    sequencerIndex = 0x0;
                    envelope.startFlag = true;
                    break;
                default:
                    break;
                }
            };

            uint8_t sample() {
                if (timer.period < 8 || lengthCounter == 0 || timer.period > 0x7FF || dutyTable[duty][sequencerIndex] == 0 || enabled == false) {
                    return 0;
                }
                return envelope.getVolume();
            }

            void stepEnvelope() {
                envelope.tick();
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
                        }
                        else {
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
        };

        struct Triangle : Channel {
            uint8_t dutyTable[32] = {
                15, 14, 13, 12, 11, 10,  9,  8,  7,  6,  5,  4,  3,  2,  1,  0,
                 0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15
            };

            Triangle() {
                linearCounter.loopCounter = false;
            }

            Divider timer;
            Divider linearCounter;
            uint8_t sequencerIndex;
            bool linearCounterEnabled = false;
            bool linearCounterReload = false;

            uint8_t sample() { 
                return dutyTable[sequencerIndex];
            };
            void stepLinearCounter() {
                if (linearCounterReload == true) {
                    linearCounter.reload();
                } else {
                    linearCounter.tick();
                }
                if (linearCounterEnabled == false) {
                    linearCounterReload = false;
                }
            };
            void stepTimer() {
                if (timer.counter == 0 && enabled == true && linearCounter.counter != 0 && lengthCounter != 0) {
                    sequencerIndex++;
                    sequencerIndex %= 32;
                }
                timer.tick();
            };
            void writeRegister(uint16_t index, uint8_t value) {
                switch (index){
                case 0x4008:
                    linearCounterEnabled = ((value & 0x80) == 0x80);
                    linearCounter.period = (value & 0x7F);
                    haltLengthCounter = !linearCounterEnabled;
                    break;
                case 0x400A:
                    timer.period = (timer.period & 0xFF00) | value;
                    break;
                case 0x400B:
                    timer.period = (timer.period & 0x00FF) | ((value & 7) << 8);
                    setLengthCounter(value >> 0x3);
                    linearCounterReload = true;
                    break;
                }
            };
        };
        
        struct Noise : Channel {
            uint16_t periodTable[16] = {
                4, 8, 16, 32, 64, 96, 128, 160, 202, 254, 380, 508, 762, 1016, 2034, 4068
            };

            Divider timer;
            uint16_t shiftRegister = 0x1;
            bool modeFlag = 0;
            Envelope envelope;

            void writeRegister(uint16_t index, uint8_t value) {
                switch (index) {
                case 0x400C:
                    haltLengthCounter = (value >> 0x5) & 0x1;
                    envelope.decay.loopCounter = haltLengthCounter;
                    envelope.useConstantVolume = (value >> 0x4) & 0x1;
                    envelope.setVolume(value & 0xF);
                    break;
                case 0x400E:
                    modeFlag = ((value & 0x80) == 0x80);
                    timer.period = periodTable[(value & 0xF)];
                    timer.reload();
                    break;
                case 0x400F:
                    setLengthCounter(value >> 0x3);
                    envelope.startFlag = true;
                    break;
                }
            };

            uint8_t sample() { 
                if (enabled == false | lengthCounter == 0 | (shiftRegister & 0x1) == 0x1) {
                    return 0;
                }
                return envelope.getVolume();
            };
            
            void stepEnvelope() {
                envelope.tick();
            };
            
            void stepTimer() {
                if (timer.counter == 0) {
                    uint8_t feedback = shiftRegister & 0x1;
                    feedback ^= (modeFlag ? ((shiftRegister & 0x40) >> 0x6) : ((shiftRegister & 0x2) >> 0x1));
                    shiftRegister = shiftRegister >> 0x1;
                    shiftRegister |= (feedback << 14);
                }
                timer.tick();
            };
        };

        struct DMC : Channel {
            uint16_t periodTable[0x10] = {
                428, 380, 340, 320, 286, 254, 226, 214, 190, 160, 142, 128, 106,  84,  72,  54
            };

            uint16_t currentAddress;
            uint16_t currentLength;
            uint8_t shiftRegister;
            uint8_t shiftCount;
            uint8_t sampleBuffer;
            uint8_t sampleLength;

            bool interruptFlag;
            Divider timer;
            uint8_t sample() {
                
            }
            void stepTimer() {

            }
            void writeRegister(uint16_t index, uint8_t value) {
                switch (index) {
                case 0x4010:
                    interruptFlag = value & 0x80 == 0x80;
                    timer.loopCounter = value & 0x40 == 0x40;
                    timer.period = periodTable[value & 0xF];
                    break;
                case 0x4011:
                    break;
                }
            }
        };

        APU(Console &console) : console(console) { };

        Console &console;
        
        Pulse pulse1 = Pulse();
        Pulse pulse2 = Pulse();
        Triangle triangle = Triangle();
        Noise noise = Noise();

        uint32_t totalCycles = 0;
        uint32_t currentCycle = 0;
        uint8_t frameCounter;


        uint8_t getAPURegister(uint16_t index);
        void setAPURegister(uint16_t index, uint8_t value);

        void step();
        void stepFrameCounter();

        float sample();

        void clockQuarterFrame();
        void clockHalfFrame();
        void processDMC(uint16_t index, uint8_t value);
        void processControl(uint16_t index, uint8_t value);
        
    };
};
 
