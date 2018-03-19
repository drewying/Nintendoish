#pragma once

#include "Mapper.h"

namespace NES {
    class Mapper4 :
        public Mapper
    {
    public:

        Mapper4(Cartridge &cartridge) : Mapper(cartridge) {}

        uint8_t getTileData(uint16_t index);
        void  setTileData(uint16_t index, uint8_t value);

        uint8_t getProgramData(uint16_t index);
        void  setProgramData(uint16_t index, uint8_t value);
    };

}

