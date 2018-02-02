#include "NES.h"
#include "CPU.h"
#include "Memory.h"
#include "PPU.h"

#include <iostream>
#include <sstream>
#include <iomanip>
#include <stdio.h>

using namespace std;

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

void NES::Console::loadProgram(const char * path)
{
	FILE *rom = fopen(path, "rb");
	if (rom) {
        char header[16];
        fread(header, sizeof(char), 16, rom);
        if (header[0] != 'N' ||
            header[1] != 'E' ||
            header[2] != 'S' ||
            header[3] != '\x1A') {
            std::cout << "Invalid Format" << std::endl;
            exit(0);
        }
        
        bool hasTrainer = (header[6] & 0x4) >> 2;
        if (hasTrainer) {
            fseek(rom, 512, SEEK_SET);
        }
        memory->extended = (header[4] == 2);
		fread(memory->prg, sizeof(char), header[4] * 16384, rom);
        fread(memory->chr, sizeof(char), header[5] * 8192, rom);
        cpu->reg.PC = memory->resetVector();
    }
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

NES::Console::Console(){
    memory = new Memory(*this);
    cpu = new CPU(*memory);
    ppu = new PPU(*this);
	reset();
}

NES::Console::~Console() {}
