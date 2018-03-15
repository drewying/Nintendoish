#include "APU.h"
#include <iostream>

using namespace std;

uint8_t NES::APU::getAPURegister(uint16_t index) {
    switch (index) {
    case 0x4017:
        return frameCounter;
        break;
    default:
        return 0x0;
        break;
    }
}

void NES::APU::setAPURegister(uint16_t index, uint8_t value) {
    switch (index) {
    case 0x4010:
        if (value & 0x80) {
            printf("Ruh Roh. DMC IRQ Not enabled");
        }
        break;
    case 0x4017:
        frameCounter = value;
        break;
    default:
        break;
    }
}

void NES::APU::step() {
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
            printf("Ruh Roh. IRQ Not Yet Implemented");
        }
        
        if (currentCycle == 14915) {
            //Clock Envelopes and Triangle Linear Counter
            //Clock Length Counters and Sweep Units 
            currentCycle = 0;
        }
    }
}
