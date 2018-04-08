#include "APU.h"
#include <iostream>

using namespace std;
using namespace NES;

uint8_t APU::getAPURegister(uint16_t index) {
    switch (index) {
    case 0x4017:
        return frameCounter;
        break;
    default:
        return 0x0;
        break;
    }
}

void APU::setAPURegister(uint16_t index, uint8_t value) {
    if (index >= 0x4000 && index <= 0x4007) processPulse(index, value);
    if (index >= 0x4008 && index <= 0x400B) processTriangle(index, value);
    if (index >= 0x400C && index <= 0x400F) processNoise(index, value);
    if (index >= 0x4010 && index <= 0x4013) processDMC(index, value);
    if (index == 0x4015) processControl(index, value);
    if (index == 0x4017) frameCounter = value;
}

void APU::processPulse(uint16_t index, uint8_t value) {
    Pulse *pulse = &pulse1;
    if (index >= 0x4004) {
        pulse = &pulse2;
        index -= 0x4;
    }
    
    switch (index) {
        case 0x4000:
            pulse->duty = value >> 0x6;
            pulse->haltLengthCounter = (value >> 0x5) & 0x1;
            pulse->constantVolume = (value >> 0x4) & 0x1;
            pulse->divider = (value & 0xF);
            break;
            
        default:
            break;
    }
    
}

void APU::processTriangle(uint16_t index, uint8_t value) {
}

void APU::processNoise(uint16_t index, uint8_t value) {

}

void APU::processDMC(uint16_t index, uint8_t value) {

}

void APU::processControl(uint16_t index, uint8_t value) {
    
}

void APU::step() {
    totalCycles++;
    currentCycle++;
    bool sequence = frameCounter & 0x80;
    bool irqEnabled = frameCounter & 0x40;
    
    if (sequence == true) {
        //5-Step Sequence
        if (currentCycle == 3729) {
            //Clock Envelopes and Triangle Linear Counter
        }
        if (currentCycle == 7457) {
            //Clock Envelopes and Triangle Linear Counter
            //Clock Length Counters and Sweep Units 
        }
        if (currentCycle == 11186) {
            //Clock Envelopes and Triangle Linear Counter
        }
        if (currentCycle == 18641) {
            //Clock Envelopes and Triangle Linear Counter
            //Clock Length Counters and Sweep Units 
            currentCycle = 0;
        }
    } else {
        //4-Step Sequence
        if (currentCycle == 3729) {
            //Clock Envelopes and Triangle Linear Counter
        }
        if (currentCycle == 7457) {
            //Clock Envelopes and Triangle Linear Counter
            //Clock Length Counters and Sweep Units 
        }
        if (currentCycle == 11186) {
            //Clock Envelopes and Triangle Linear Counter
        }
        if (currentCycle == 14914) {
            //IRQ
            //printf("Ruh Roh. IRQ Not Yet Implemented");
        }
        
        if (currentCycle == 14915) {
            //Clock Envelopes and Triangle Linear Counter
            //Clock Length Counters and Sweep Units 
            currentCycle = 0;
        }
    }
}
