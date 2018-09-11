#include "APU.h"
#include <iostream>

using namespace std;
using namespace NES;

uint8_t APU::getAPURegister(uint16_t index) {
    switch (index) {
        case 0x4015: {
            bool irqFlag = console.cpu->requestIRQ;
            //Clear IRQfLAG
            console.cpu->requestIRQ = false;
            return        (dmc.irqEnabledFlag << 0x7) |
                                     (irqFlag << 0x6) |
                                         (0x0 << 0x5) |
            ((dmc.bytesRemaining.counter > 0) << 0x4) | //DMC status goes here
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
    if (index >= 0x4010 && index <= 0x4013) dmc.writeRegister(index, value);
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
    dmc.bytesRemaining.counter = 0x0;
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
    
    dmc.enabled = ((value & 0x10) == 0x10);
    if (dmc.enabled == true) {
        if (dmc.bytesRemaining.counter == 0x0) dmc.reload();
    } else {
        dmc.bytesRemaining.counter = 0x0;
    }
}

float APU::sample() {
    //TODO Lookup instead of linear approximation
    float pulseOut = 0.00752f * (float)(pulse1.sample() + pulse2.sample());
    float tndOut = 0.00851 * triangle.sample() + 0.00494 * noise.sample() + 0.00335 * dmc.sample();
    return pulseOut + tndOut;
}

void APU::step() {
    totalCycles++;
    stepFrameCounter();
    
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
    if (processFrameCounterWrite && currentCycle % 2 == 0) {
        processFrameCounterWrite = false;
        currentCycle = 0;
        if ((frameCounter & 0x80) == 0x80) {
            clockHalfFrame();
            clockQuarterFrame();
        }
        if ((frameCounter & 0x40) == 0x40) {
            console.cpu->requestIRQ = false;
        }
    }
   
    if (currentCycle % 2 == 0) {
        pulse1.stepTimer();
        pulse2.stepTimer();
        noise.stepTimer();
        
    }
    dmc.stepTimer();
    triangle.stepTimer();
    
    bool sequenceMode = ((frameCounter & 0x80) == 0x80);
    bool interruptInhibit = ((frameCounter & 0x40) == 0x40);

    if (sequenceMode == false) {
        //Mode 0x0 4-Step Sequence
        if (currentCycle == 7459) {
            clockQuarterFrame();
        }
        if (currentCycle == 14915) {
            clockHalfFrame();
            clockQuarterFrame();
        }
        if (currentCycle == 22373) {
            clockQuarterFrame();
        }
        if (currentCycle == 29830) {
            //IRQ
            if (interruptInhibit == false) {
                console.cpu->requestIRQ = true;
            }
        }
        if (currentCycle == 29831) {
            clockHalfFrame();
            clockQuarterFrame();
            //IRQ
            if (interruptInhibit == false) {
                console.cpu->requestIRQ = true;
            }
        }
        if (currentCycle == 29832) {
            //IRQ
            if (interruptInhibit == false) {
                console.cpu->requestIRQ = true;
            }
        }
        currentCycle++;
        if (currentCycle >= 37289) currentCycle = 7459;
    } else {
        //Mode 0x1 5-Step Sequence
        if (currentCycle == 7459) {
            clockQuarterFrame();
        }
        if (currentCycle == 14915) {
            clockHalfFrame();
            clockQuarterFrame();
        }
        if (currentCycle == 22373) {
            clockQuarterFrame();
        }
        if (currentCycle == 37283) {
            clockHalfFrame();
            clockQuarterFrame();
        }
        currentCycle++;
        if (currentCycle >= 44741) currentCycle = 7459;
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
