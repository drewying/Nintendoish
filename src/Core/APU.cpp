#include "APU.h"
#include <iostream>

using namespace std;
using namespace NES;

uint8_t APU::getAPURegister(uint16_t index) {
    switch (index) {
        case 0x4015: {
            bool irqFlag = frameIRQ;
            //Clear IRQfLAG
            frameIRQ = false;
            return                       (0x0 << 0x7) |
                                     (irqFlag << 0x6) |
                                         (0x0 << 0x5) |
                                         (0x0 << 0x4) | //DMC status goes here
                   ((noise.lengthCounter > 0) << 0x3) |
                ((triangle.lengthCounter > 0) << 0x2) |
                  ((pulse2.lengthCounter > 0) << 0x1) |
                   (pulse1.lengthCounter > 0);
        }

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
    if (index >= 0x4008 && index <= 0x400B) triangle.writeRegister(index, value);
    if (index >= 0x400C && index <= 0x400F) noise.writeRegister(index, value);
    if (index >= 0x4010 && index <= 0x4013) processDMC(index, value);
    if (index == 0x4015) processControl(index, value);
    if (index == 0x4017) {
        frameCounter = value;
        processFrameCounterWrite = true;
    }
}

void APU::reset() {
    pulse1.lengthCounter = 0x0;
    pulse2.lengthCounter = 0x0;
    triangle.lengthCounter = 0x0;
    noise.lengthCounter = 0x0;
    dmc.lengthCounter = 0x0;
    frameIRQ = 0x0;
}
void APU::processDMC(uint16_t index, uint8_t value) {

}

void APU::processControl(uint16_t index, uint8_t value) {
    pulse1.enabled = ((value & 0x1) == 0x1);
    if (pulse1.enabled == false) pulse1.lengthCounter = 0x0;
    pulse2.enabled = ((value & 0x2) == 0x2);
    if (pulse2.enabled == false) pulse2.lengthCounter = 0x0;
    triangle.enabled = ((value & 0x4) == 0x4);
    if (triangle.enabled == false) triangle.lengthCounter = 0x0;
    noise.enabled = ((value & 0x8) == 0x8);
    if (noise.enabled == false) noise.lengthCounter = 0x0;
}

float APU::sample() {
    //TODO Lookup instead of linear approximation
    float pulseOut = 0.00752f * (float)(pulse1.sample() + pulse2.sample());
    float tndOut = 0.00851 * triangle.sample() + 0.00494 * noise.sample();
    return pulseOut + tndOut;
}

void APU::step() {
    totalCycles++;
    triangle.stepTimer();

    if (totalCycles % 2 == 1) {
        stepFrameCounter();
    }
    
    static bool r = 0;
    static int nextClock = (console.CPU_CLOCK_RATE / console.AUDIO_SAMPLE_RATE);
    if (totalCycles == nextClock) {
        r = !r;
        nextClock += (console.CPU_CLOCK_RATE / console.AUDIO_SAMPLE_RATE) + r;
        if (console.audioBufferLength < console.AUDIO_BUFFER_SIZE) {
            console.audioBuffer[console.audioBufferLength++] = sample();
        }
    } 
}

void APU::stepFrameCounter() {
    currentCycle++;

    if (processFrameCounterWrite) {
        processFrameCounterWrite = false;
        currentCycle = 0;
        if ((frameCounter & 0x80) == 0x80) {
            clockHalfFrame();
            clockQuarterFrame();
        }
        if ((frameCounter & 0x40) == 0x40) {
            frameIRQ = false;
        }
    }
   
    pulse1.stepTimer();
    pulse2.stepTimer();
    noise.stepTimer();

    bool sequenceMode = ((frameCounter & 0x80) == 0x80);
    bool interruptInhibit = ((frameCounter & 0x40) == 0x40);

    if (sequenceMode == true) {
        //5-Step Sequence
        if (currentCycle == 3729) {
            clockQuarterFrame();
        }
        if (currentCycle == 7457) {
            clockHalfFrame();
            clockQuarterFrame();
        }
        if (currentCycle == 11186) {
            clockQuarterFrame();
        }
        if (currentCycle == 18641) {
            clockHalfFrame();
            clockQuarterFrame();
            currentCycle = 0;
        }
    } else {
        //4-Step Sequence
        if (currentCycle == 3729) {
            clockQuarterFrame();
        }
        if (currentCycle == 7457) {
            clockHalfFrame();
            clockQuarterFrame();
        }
        if (currentCycle == 11186) {
            clockQuarterFrame();
        }
        if (currentCycle == 14915) {
            clockHalfFrame();
            clockQuarterFrame();
            //IRQ
            if (interruptInhibit == false) {
                frameIRQ = true;
                console.cpu->requestIRQ = true;
            }
            currentCycle = 0;
        }
    }
}

void APU::clockHalfFrame() {
    //Clock Length Counters and Sweep Units 
    pulse1.stepLengthCounter();
    pulse1.stepSweep();
    pulse2.stepLengthCounter();
    pulse2.stepSweep();
    noise.stepLengthCounter();
    triangle.stepLengthCounter();
}

void APU::clockQuarterFrame() {
    //Clock Envelopes and Triangle Linear Counter
    pulse1.stepEnvelope();
    pulse2.stepEnvelope();
    noise.stepEnvelope();
    triangle.stepLinearCounter();
}
