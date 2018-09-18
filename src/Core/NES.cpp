#include "NES.h"
#include "CPU.h"
#include "Memory.h"
#include "PPU.h"
#include "APU.h"

#include <iostream>
#include <sstream>
#include <iomanip>
#include <stdio.h>


using namespace std;

NES::Console::Console() {
    apu = new APU(*this);
    memory = new Memory(*this);
    cpu = new CPU(*memory);
    ppu = new PPU(*this);
    ppuMemory = new PPUMemory(*this);
    controllerOne = new Controller();
}

NES::Console::~Console() {}

int NES::Console::emulateCycle() {
    apu->step();
    cpu->step();
    for (int i = 0; i < 3; i++) ppu->step();
    return 0;
}

void NES::Console::loadProgram(const char* path) {
    cartridge = new Cartridge(*this, path);

    // At power, it is as if $00 were written to $4017 then a 9 - 12 clock delay, then execution from address in reset vector.
    apu->setAPURegister(0x4017, 0x0);
    for (int i = 0; i < 9; i++) apu->step();

    cpu->reg.PC = memory->resetVector();
    cpu->loadNextInstruction();
    std::cout << "Loaded" << std::endl;
}

void NES::Console::reset() {
    apu->reset();
    //At reset, the apu has a 9-12 clock delay then execution from address in reset vector
    for (int i = 0; i < 9; i++) apu->step();

    cpu->reset();
    ppu->reset();
}
