#include "Cartridge.h"
#include <stdlib.h> 
#include <stdio.h>

#include "Mapper0.h"
#include "Mapper1.h"
#include "Mapper2.h"
#include "Mapper3.h"

using namespace NES;

Cartridge::Cartridge(const char* path) {
    FILE *rom = fopen(path, "rb");
    if (rom) {
        uint8_t header[16];
        fread(header, sizeof(uint8_t), 16, rom);
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
        if ((header[6] & 0x4) == 0x4) verticalMirroring = horizontalMirroring = true;
        batteryBackup = header[6] & 0x2 == 0x2;
        mapperNumber = (header[7] & 0xF) | (header[6] >> 4);

        bool hasTrainer = (header[6] & 0x4) == 0x4;
        if (hasTrainer) {
            fseek(rom, 512, SEEK_SET);
        }

        prg = (uint8_t*)calloc(prgSize * 0x4000, sizeof(uint8_t));
        chr = (uint8_t*)calloc(chrSize * 0x2000, sizeof(uint8_t));
        
        fread(prg, sizeof(uint8_t), prgSize * 0x4000, rom);
        fread(chr, sizeof(uint8_t), header[5] * 0x2000, rom);

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
        case 3:
            mapper = new Mapper3(*this);
            break;
        default:
            printf("Unsupported Mapper");
            exit(0);
        }
    }
    else {
        printf("File not found");
        exit(0);
    }
}

uint8_t Cartridge::getTileData(uint16_t index) { return mapper->getTileData(index); }

void Cartridge::setTileData(uint16_t index, uint8_t value) { return mapper->setTileData(index, value); }

uint8_t Cartridge::getProgramData(uint16_t index) { return mapper->getProgramData(index); }

void  Cartridge::setProgramData(uint16_t index, uint8_t value) { return mapper->setProgramData(index, value); }

NES::Cartridge::~Cartridge() {
    free(prg);
    free(chr);
    delete mapper;
}
