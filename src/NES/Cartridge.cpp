#include "Cartridge.h"
#include <stdlib.h> 
#include <stdio.h>

#include "Mapper0.h"
#include "Mapper1.h"
#include "Mapper2.h"

using namespace NES;

Cartridge::Cartridge(const char* path) {
	FILE *rom = fopen(path, "rb");
	if (rom) {
		unsigned char header[16];
		fread(header, sizeof(char), 16, rom);
		if (header[0] != 'N' ||
			header[1] != 'E' ||
			header[2] != 'S' ||
			header[3] != '\x1A') {
			printf("File is not an NES ROM.");
			exit(0);
		}

		prgSize = header[4];
		chrSize = header[5];
		if (chrSize == 0) chrSize = (header[8] == 0 ? 0x1 : header[8]);  //No ROM present, allocate RAM instead.
		verticalMirroring = (header[6] & 0x1) == 0x1;
		horizontalMirroring = (header[6] & 0x1) == 0x0;
		if (header[6] & 0x4 == 0x4) verticalMirroring = horizontalMirroring = true;
		batteryBackup = header[6] & 0x2 == 0x2;
		mapperNumber = (header[7] & 0xF) | (header[6] >> 4);

		bool hasTrainer = (header[6] & 0x4) == 0x4;
		if (hasTrainer) {
			fseek(rom, 512, SEEK_SET);
		}

		//prg = (unsigned char*)calloc(prgSize * 0x4000, sizeof(char));
		chr = (unsigned char*)calloc(chrSize * 0x2000, sizeof(char));
		
		fread(prg, sizeof(char), prgSize * 0x4000, rom);
		fread(chr, sizeof(char), header[5] * 0x2000, rom);

		switch (mapperNumber) {
		case 0:
			mapper = new Mapper0(*this);
			break;
		case 1:
			mapper = new Mapper1(*this);
			break;
		case 2:
			mapper = new Mapper2(*this);
			break;
		default:
			printf("Unsupported Mapper");
			exit(0);
		}
	}
}

unsigned char Cartridge::getTileData(unsigned short index) { return mapper->getTileData(index); }

void Cartridge::setTileData(unsigned short index, unsigned char value) { return mapper->setTileData(index, value); }

unsigned char Cartridge::getProgramData(unsigned short index) { return mapper->getProgramData(index); }

void  Cartridge::setProgramData(unsigned short index, unsigned char value) { return mapper->setProgramData(index, value); }

NES::Cartridge::~Cartridge() {
	free(prg);
	free(chr);
	delete mapper;
}
