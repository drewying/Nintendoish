#pragma once

#include "Mapper.h"

namespace NES {
    class Mapper9 :
        public Mapper
    {
    public:
        Mapper9(Cartridge &cartridge);
        
        uint8_t prgRam[0x2000];
        uint8_t prgOffset0 = 0x0;
        uint8_t prgOffset1 = 0x0;
        
        uint8_t chrOffset0FD = 0x0;
        uint8_t chrOffset0FE = 0x0;
        uint8_t chrOffset1FD = 0x0;
        uint8_t chrOffset1FE = 0x0;
        
        bool latch0 = false;
        bool latch1 = false;
        
        uint8_t getTileData(uint16_t index);
        void  setTileData(uint16_t index, uint8_t value);

        uint8_t getProgramData(uint16_t index);
        void  setProgramData(uint16_t index, uint8_t value);
    };
}

