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
    cpu->step();
    for (int i = 0; i < 3; i++) ppu->step();
    apu->step();
    return 0;
}

void NES::Console::loadProgram(const char* path) {
    cartridge = new Cartridge(*this, path);
    cpu->reg.PC = memory->resetVector();
    cpu->loadNextInstruction();
    std::cout << "Loaded" << std::endl;
}

void NES::Console::reset() {
    cpu->reset();
    apu->reset();
    ppu->reset();
}
