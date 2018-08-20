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
    reset();
}

NES::Console::~Console() {}

int NES::Console::emulateCycle() {
    int cycles = cpu->step();
    for (int i = 0; i < (cycles * 3); i++) ppu->step();
    for (int i = 0; i < cycles; i++) apu->step();
    return cycles;
}

void NES::Console::loadProgram(const char* path) {
    cartridge = new Cartridge(*this, path);
    cpu->reg.PC = memory->resetVector();
    std::cout << "Loaded" << std::endl;
}

void NES::Console::reset() {
    cpu->reg.A = 0;
    cpu->reg.X = 0;
    cpu->reg.Y = 0;
    cpu->reg.S = 0xFD;
    cpu->reg.P.status.T2 = true;
    cpu->reg.P.status.Interrupt = true;
    cpu->reg.S = 0xFD;
}