#pragma once

#include "Mapper.h"

namespace NES {
    class Mapper2 :
        public Mapper
    {
    public:
        Mapper2(Cartridge &cartridge);

        uint8_t prgOffset0 = 0x0;
        uint8_t prgOffset1 = 0x0;

        uint8_t getTileData(uint16_t index);
        void  setTileData(uint16_t index, uint8_t value);

        uint8_t getProgramData(uint16_t index);
        void  setProgramData(uint16_t index, uint8_t value);
    };

}

