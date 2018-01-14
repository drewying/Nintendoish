#include "NES.h"
#include "CPU.h"
#include "Memory.h"
#include "PPU.h"

#include <iostream>
#include <sstream>
#include <iomanip>
#include <stdio.h>

using namespace std;

void NES::Console::emulateCycle()
{
	if (stallCycles > 0x0) {
		stallCycles--;
		for (int i = 0; i < 3; i++) {
			ppu->step();
		}
		return;
	}

    int estimatedCycles = cpu->loadNextInstruction();
	int pre = estimatedCycles / 2;
	int post = estimatedCycles / 2;
	if (estimatedCycles % 2 == 1) post++;

	for (int i = 0; i < estimatedCycles * 3; i++) {
		ppu->step();
	}

	int actualCycles = cpu->executeLoadedInstruction();
	for (int i = 0; i < (actualCycles * 3) - (estimatedCycles * 3); i++) {
	//while (ppu->cycles < cpu->cycles * 3) {
		ppu->step();
	}

	//for (int i = 0; i < post * 3; i++) {
		//ppu->step();
	//}

	if (triggerNMI == true) {
		cpu->NMI();
		for (int i = 0; i < 21; i++) {
			ppu->step();
		}
		triggerNMI = false;
	}
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
        fread(ppu->memory.chr, sizeof(char), header[5] * 8192, rom);
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
