#pragma once

#include "Mapper.h"

namespace NES {
    class Mapper1 :
        public Mapper
    {
    public:
        Mapper1(Cartridge &cartridge);

        uint8_t prgRAM[0x2000];

        uint8_t loadRegister = 0x10;
        uint8_t prgOffset1 = 0x0;
        uint8_t prgOffset2 = 0x0;
        uint8_t chrOffset1 = 0x0;
        uint8_t chrOffset2 = 0x0;

        uint8_t prgBankMode = 0x0;
        uint8_t chrBankMode = 0x0;

        uint8_t getTileData(uint16_t index);
        void  setTileData(uint16_t index, uint8_t value);

        uint8_t getProgramData(uint16_t index);
        void  setProgramData(uint16_t index, uint8_t value);
        void clearLoadRegister();
    };

}

