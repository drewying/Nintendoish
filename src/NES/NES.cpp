#include "NES.h"
#include "CPU.h"
#include "Memory.h"
#include "PPU.h"
#include "Mapper0.h"

#include <iostream>
#include <sstream>
#include <iomanip>
#include <stdio.h>


using namespace std;

NES::Console::Console() {
	memory = new Memory(*this);
	cpu = new CPU(*memory);
	ppu = new PPU(*this);
	ppuMemory = new PPUMemory(*this);
	controllerOne = new Controller();
	reset();
}

NES::Console::~Console() {}

void NES::Console::emulateCycle() {
	
	int estimatedCycles = cpu->stallCycles > 0 ? 0x0 : cpu->timingTable[memory->get(cpu->reg.PC)];
	estimatedCycles *= 3;
	estimatedCycles = 0x0;

	for (int i = 0; i < estimatedCycles; i++) {
		ppu->step();
		if (cpu->requestNMI == true) {
			//printf("\n\nNMI REQUESTED PRE EXEC\n\n");
		}
	};

	int actualCycles = cpu->loadNextInstruction();

	for (int i = 0; i < (actualCycles * 3) - estimatedCycles; i++) {
		ppu->step();
		if (cpu->requestNMI == true) {
			//printf("\n\nNMI REQUESTED POST EXEC\n\n");
		}
	};
}

void NES::Console::loadProgram(const char* path)
{
	cartridge = new Cartridge(path);
	cpu->reg.PC = memory->resetVector();
	std::cout << "Loaded" << std::endl;
}

void NES::Console::reset()
{
	cpu->reg.A = 0;
	cpu->reg.X = 0;
	cpu->reg.Y = 0;
 	cpu->reg.S = 0xFD;
    cpu->reg.P.status.T2 = true;
    cpu->reg.P.status.Interrupt = true;
    cpu->reg.S = 0xFD;
}
