#pragma once

#include "Mapper.h"

namespace NES {
    class Mapper4 :
        public Mapper
    {
    public:
        
        uint8_t prgRAM[0x2000];
        
        uint8_t prgOffset0 = 0x0;
        uint8_t prgOffset1 = 0x0;
        uint8_t prgOffset2 = 0x0;
        uint8_t prgOffset3 = 0x0;
        
        uint8_t chrOffset0 = 0x0;
        uint8_t chrOffset1 = 0x0;
        uint8_t chrOffset2 = 0x0;
        uint8_t chrOffset3 = 0x0;
        uint8_t chrOffset4 = 0x0;
        uint8_t chrOffset5 = 0x0;
        uint8_t chrOffset6 = 0x0;
        uint8_t chrOffset7 = 0x0;
        
        uint8_t chrMode = 0x0;
        uint8_t prgMode = 0x0;
        uint8_t selectedRegister = 0x0;
        uint8_t registers[8];
        
        bool enableRamWrites = true;
        bool enableRam = true;
        bool enableIRQ = true;
        bool reloadIRQ = false;
        bool scanlineLatch = false;
        
        void tickScanlineCounter(uint16_t index);
        uint8_t irqReload = 0x0;
        uint8_t counter = 0x0;
        
        void updateOffsets();

        uint8_t getTileData(uint16_t index);
        void  setTileData(uint16_t index, uint8_t value);

        uint8_t getProgramData(uint16_t index);
        void  setProgramData(uint16_t index, uint8_t value);
        
        virtual void step();
        
        Mapper4(Cartridge &cartridge);
    };

}

