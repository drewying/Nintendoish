#include "APU.h"
#include <iostream>

using namespace std;
using namespace NES;

uint8_t APU::getAPURegister(uint16_t index) {
    switch (index) {
    case 0x4015:

        break;
    case 0x4017:
        return frameCounter;
        break;
    default:
        return 0x0;
        break;
    }
}

void APU::setAPURegister(uint16_t index, uint8_t value) {
    if (index >= 0x4000 && index <= 0x4003) pulse1.writeRegister(index, value);
    if (index >= 0x4004 && index <= 0x4007) pulse2.writeRegister(index - 0x4, value);
    if (index >= 0x4008 && index <= 0x400B) processTriangle(index, value);
    if (index >= 0x400C && index <= 0x400F) processNoise(index, value);
    if (index >= 0x4010 && index <= 0x4013) processDMC(index, value);
    if (index == 0x4015) processControl(index, value);
    if (index == 0x4017) frameCounter = value;
}

void APU::processTriangle(uint16_t index, uint8_t value) {
}

void APU::processNoise(uint16_t index, uint8_t value) {

}

void APU::processDMC(uint16_t index, uint8_t value) {

}

void APU::processControl(uint16_t index, uint8_t value) {
    pulse1.enabled = ((value & 0x1) == 0x1);
    pulse2.enabled = ((value & 0x2) == 0x2);
}

float APU::sample() {
    //TODO Lookup instead of linear approximation
    return 0.00752 * (pulse1.sample() + pulse2.sample());
}

void APU::step() {
    totalCycles++;
    currentCycle++;

    stepCycle = !stepCycle;
    if (stepCycle == true) {
        stepFrameCounter();
    }

    static bool r = 0;
    static int nextClock = (console.CPU_CLOCK_RATE / console.AUDIO_SAMPLE_RATE);
    if (currentCycle == nextClock) {
        r = !r;
        nextClock += (console.CPU_CLOCK_RATE / console.AUDIO_SAMPLE_RATE) + r;
        if (console.audioBufferLength < console.AUDIO_BUFFER_SIZE) {
            console.audioBuffer[console.audioBufferLength++] = sample();
        } else {
            printf("\n Buffer Overflow");
        }
    } 
}

void APU::stepFrameCounter() {

    pulse1.stepTimer();
    pulse2.stepTimer();


    bool sequence = frameCounter & 0x80;
    bool irqEnabled = frameCounter & 0x40;
    
    if (sequence == true) {
        //5-Step Sequence
        if (currentCycle == 3729) {
            //Clock Envelopes and Triangle Linear Counter
            pulse1.stepEnvelope();
            pulse2.stepEnvelope();
        }
        if (currentCycle == 7457) {
            //Clock Envelopes and Triangle Linear Counter
            //Clock Length Counters and Sweep Units 
            pulse1.stepLengthCounter();
            pulse2.stepLengthCounter();
            pulse1.stepEnvelope();
            pulse2.stepEnvelope();
        }
        if (currentCycle == 11186) {
            //Clock Envelopes and Triangle Linear Counter
        }
        if (currentCycle == 18641) {
            //Clock Envelopes and Triangle Linear Counter
            //Clock Length Counters and Sweep Units 
            currentCycle = 0;
            pulse1.stepLengthCounter();
            pulse2.stepLengthCounter();
            pulse1.stepEnvelope();
            pulse2.stepEnvelope();
        }
    } else {
        //4-Step Sequence
        if (currentCycle == 3729) {
            //Clock Envelopes and Triangle Linear Counter
            pulse1.stepEnvelope();
            pulse2.stepEnvelope();
        }
        if (currentCycle == 7457) {
            //Clock Envelopes and Triangle Linear Counter
            //Clock Length Counters and Sweep Units 
            pulse1.stepLengthCounter();
            pulse2.stepLengthCounter();
            pulse1.stepEnvelope();
            pulse2.stepEnvelope();
        }
        if (currentCycle == 11186) {
            //Clock Envelopes and Triangle Linear Counter
            pulse1.stepEnvelope();
            pulse2.stepEnvelope();
        }
        if (currentCycle == 14914) {
            //IRQ
            printf("Ruh Roh. IRQ Not Yet Implemented");
        }
        
        if (currentCycle == 14915) {
            //Clock Envelopes and Triangle Linear Counter
            //Clock Length Counters and Sweep Units 
            currentCycle = 0;
            pulse1.stepLengthCounter();
            pulse2.stepLengthCounter();
            pulse1.stepEnvelope();
            pulse2.stepEnvelope();
        }
    }
}
