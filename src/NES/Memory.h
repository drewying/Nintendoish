#pragma once
#include "NES.h"

namespace NES {
    class Console;
    class Memory {
    public:
        bool extended = false;
        unsigned char prg[0x8000];  // Program Data
        unsigned char ram[0x800];   // System Memory
        Console &parent;
        
        unsigned char &operator [](unsigned short index);
        unsigned short resetVector();
        
        Memory(Console &parent): parent(parent) {}
        ~Memory();
    };
};
