#pragma once

#include "NES.h"
#include <stdint.h>

namespace NES {
    class Console;
    class PPU {
    public:
        PPU(Console &console) : console(console) { reset(); };

        Console &console;

        // Internal Registers
        struct Registers {
            union {
                struct {
                    unsigned BaseAddressHi : 1,
                        BaseAddressLo : 1,
                        VRAMAddressIncrement : 1,
                        SpriteTableSelect : 1,
                        BackgroundTableSelect : 1,
                        TallSprites : 1,
                        MasterSlave : 1,
                        NMI : 1;
                } flags;
                uint8_t byte;
            } control;

            union {
                struct {
                    unsigned Greyscale : 1,
                        RenderLeftBackground : 1,
                        RenderLeftSprites : 1,
                        RenderBackground : 1,
                        RenderSprites : 1,
                        EmphasizeRed : 1,
                        EmphasizeGreen : 1,
                        EmphasizeBlue : 1;
                } flags;
                uint8_t byte;
            } mask;

            union {
                struct {
                    unsigned LastWrite : 5,
                        SpriteOverflow : 1,
                        Sprite0Hit : 1,
                        VBlankEnabled : 1;
                } flags;
                uint8_t byte;
            } status;
            uint8_t oamAddress;
        } reg;

        //Background Registers
        struct {
            uint16_t tileLo = 0x0;
            uint16_t tileHi = 0x0;
            uint16_t attributeTableHi = 0x0;
            uint16_t attributeTableLo = 0x0;
        } shift;

        struct {
            uint8_t tileLo = 0x0;
            uint8_t tileHi = 0x0;
            uint8_t attributeTable = 0x0;
            uint8_t nameTable = 0x0;
        } latch;

        union Address {
            struct {
                unsigned lo : 8,
                    hi : 7;
            } byte;
            struct {
                unsigned coarseXScroll : 5,
                    coarseYScroll : 5,
                    nameTableX : 1,
                    nameTableY : 1,
                    fineYScroll : 3;
            } scroll;
            unsigned address : 15;
        };

        struct {
            Address v; // Current VRAM Address
            Address t; // Temporary VRAM Address
            unsigned fineXScroll : 3;
            bool writeLatch = false;
        } vramRegister;

        //Sprite Memory
        struct Sprite {
            uint8_t yPosition;
            uint8_t tileIndex;
            struct {
                uint8_t palette : 2,
                    unused : 3,
                    priority : 1,
                    horizontalFlip : 1,
                    verticalFlip : 1;
            } attributes;
            uint8_t xPosition;
        };

        uint8_t activeSpriteCount = 0;
        Sprite* spr[8]            = { 0 };    // Active Sprites
        uint8_t oam[0x100]        = { 0 };    // Object Attribute Memory
        uint8_t sprTiles[0x10]    = { 0 };    // Sprite Tiles
        
        //PPU Access Methods
        uint8_t getPPURegister(uint16_t index);
        void setPPURegister(uint16_t index, uint8_t value);
        void copyDMAMemory(uint8_t index);

  
        // Emulation
        int32_t totalCycles     = 0;
        int32_t totalFrames     = 0;
        int32_t currentCycle    = 0;
        int32_t currentScanline = 241;
        bool oddFrame = false;

        void step();
        void reset();
        void vBlankStart();
        void vBlankEnd();

        // Rendering Helpers
        void renderPatternTable();
        void renderTile(int x, int y, int tileIndex);
        void fetchSprites();
        void evaluateSprites();
        void renderPixel();		
        
        uint8_t colorTable[0x40][0x3] = {
            {84, 84,  84},
            {0,  30, 116},
            {8,  16, 144},
            {48,  0, 136},
            {68,  0, 100},
            {92,  0,  48},
            {84,  4,   0},
            {60, 24,   0},
            {32, 42,   0},
            {8,  58,   0},
            {0,  64,   0},
            {0,  60,   0},
            {0,  50,  60},
            {0,   0,   0},
            {0,   0,   0},
            {0,   0,   0},
            
            {152, 150, 152},
            {8,    76, 196},
            {48,   50, 236},
            {92,   30, 228},
            {136,  20, 176},
            {160,  20, 100},
            {152,  34,  32},
            {120,  60,   0},
            {84,   90,   0},
            {40,  114,   0},
            {8,   124,   0},
            {0,   118,  40},
            {0,   102, 120},
            {0,     0,   0},
            {0,     0,   0},
            {0,     0,   0},
            
            {236, 238, 236},
            {76,  154, 236},
            {120, 124, 236},
            {176,  98, 236},
            {228,  84, 236},
            {236,  88, 180},
            {236, 106, 100},
            {212, 136,  32},
            {160, 170,   0},
            {116, 196,   0},
            {76,  208,  32},
            {56,  204, 108},
            {56,  180, 204},
            {60,   60,  60},
            { 0,    0,   0},
            { 0,    0,   0},

            {236, 238, 236},
            {168, 204, 236},
            {188, 188, 236},
            {212, 178, 236},
            {236, 174, 236},
            {236, 174, 212},
            {236, 180, 176},
            {228, 196, 144},
            {204, 210, 120},
            {180, 222, 120},
            {168, 226, 144},
            {152, 226, 180},
            {160, 214, 228},
            {160, 162, 160},
            {0,     0,   0},
            {0,     0,   0}
        };
    };
};
