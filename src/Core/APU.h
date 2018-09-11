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
            uint16_t lengthCounter = 0x0;
            uint8_t duty;
            virtual uint8_t sample() { return 0x0; };
            virtual void stepEnvelope() {};
            virtual void stepTimer() {};
            virtual void writeRegister(uint16_t index, uint8_t value) {};
            void setLengthCounter(uint8_t value) {
                if (enabled == true) {
                    lengthCounter = lengthCounterTable[value];
                }
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
                    haltLengthCounter = linearCounterEnabled;
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
                if (enabled == false || lengthCounter == 0 || (shiftRegister & 0x1) == 0x1) {
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
            DMC(Console &console) : console(console) { 
                bitsRemaining.period = 8;
                timer.loopCounter = true;
            };

            uint16_t periodTable[0x10] = {
                428, 380, 340, 320, 286, 254, 226, 214, 190, 160, 142, 128, 106,  84,  72,  54
            };

            Console &console;

            // Memory reader
            uint16_t currentAddress = 0x0;
            uint16_t sampleAddress = 0x0;
            uint16_t sampleLength = 0x0;
            uint8_t sampleBuffer = 0x0;

            //Output Unit
            uint8_t shiftRegister = 0x0;
            uint8_t outputLevel = 0x0;
            bool silenceFlag = false;
            bool irqEnabledFlag = false;

            Divider timer;
            Divider bitsRemaining;
            Divider bytesRemaining;

            uint8_t sample() {
                return outputLevel;
            }

            void stepMemoryReader() {
                if (sampleBuffer == 0x0 && bytesRemaining.counter > 0x0) {
                    if (console.cpu->stallCycles > 7) {
                        // If OAM DMA is in progress, it is paused for two cycles.
                        console.cpu->stallCycles += 2;
                    } else {
                        //The CPU is stalled for up to 4 CPU cycles
                        console.cpu->stallCycles += 4;
                    }
                    // The sample buffer is filled with the next sample byte read from the current address
                    sampleBuffer = console.memory->get(currentAddress);

                    //The address is incremented; if it exceeds $FFFF, it is wrapped around to $8000.
                    currentAddress++;
                    if (currentAddress == 0x0) {
                        currentAddress = 0x8000;
                    }

                    //The bytes remaining counter is decremented; if it becomes zero and the loop flag is set, the sample is restarted(see above); 
                    //otherwise, if the bytes remaining counter becomes zero and the IRQ enabled flag is set, the interrupt flag is set.
                    if (bytesRemaining.counter == 0x0 && bytesRemaining.loopCounter == true) {
                        currentAddress = sampleAddress;
                    } else if (bytesRemaining.counter == 0x0 && bytesRemaining.loopCounter == false && irqEnabledFlag == true) {
                        console.cpu->requestIRQ = true;
                    }
                    bytesRemaining.tick();
                }
            }

            void stepOutputUnit() {
                // If the silence flag is clear, the output level changes based on bit 0 of the shift register.
                if (silenceFlag == false) {
                    bool addOutput = shiftRegister & 0x1;
                    if (addOutput == true && outputLevel <= 125) outputLevel += 2;
                    if (addOutput == false && outputLevel >= 2) outputLevel -= 2;
                }
                // The right shift register is clocked.
                shiftRegister >>= 1;

                // the bits-remaining counter is decremented. If it becomes zero, a new output cycle is started.
                if (bitsRemaining.counter == 0) {
                    // If the sample buffer is empty, then the silence flag is set; otherwise, the silence flag is cleared and the sample buffer is emptied into the shift register.
                    if (sampleBuffer == 0x0) {
                        silenceFlag = true;
                    } else {
                        silenceFlag = false;
                        shiftRegister = sampleBuffer;
                        sampleBuffer = 0x0;
                        bitsRemaining.reload();
                    }
                }
                bitsRemaining.tick();
            }
            
            void stepTimer() {
                stepMemoryReader();
                if (timer.counter == 0) {
                    stepOutputUnit();
                }
                timer.tick();
            }

            void writeRegister(uint16_t index, uint8_t value) {
                switch (index) {
                case 0x4010:
                    irqEnabledFlag = (value & 0x80) == 0x80;
                    bytesRemaining.loopCounter = (value & 0x40) == 0x40;
                    timer.period = periodTable[value & 0xF];
                    timer.reload();
                    break;
                case 0x4011:
                    outputLevel = value & 0x7F;
                    break;
                case 0x4012:
                    // Sample address = %11AAAAAA.AA000000 = $C000 + (A * 64) 
                    sampleAddress = 0xC000 + (value * 0x40);
                    currentAddress = sampleAddress;
                    break;
                case 0x4013:
                    // Sample length = %LLLL.LLLL0001 = (L * 16) + 1 
                    sampleLength = (value * 10) + 1;
                    bytesRemaining.period = sampleLength;
                    bytesRemaining.reload();
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
        DMC dmc = DMC(console);

        uint32_t totalCycles = 0;
        uint32_t currentCycle = 0;
        uint8_t frameCounter;
        bool processFrameCounterWrite = false;
        uint8_t getAPURegister(uint16_t index);
        void setAPURegister(uint16_t index, uint8_t value);

        void step();
        void stepFrameCounter();

        float sample();

        void clockQuarterFrame();
        void clockHalfFrame();
        void processDMC(uint16_t index, uint8_t value);
        void processControl(uint16_t index, uint8_t value);
        void reset();
    };
};
 
