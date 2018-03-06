#pragma once

#include "NES.h"
#include <stdint.h>

namespace NES {
    class Console;
    class Memory {
    public:		

		unsigned char ram[0x800] = { 0 }; // System Memory
		Console &console;

		unsigned char get(unsigned short index);
		void  set(unsigned short index, unsigned char value);
		unsigned short resetVector();

		Memory(Console &console): console(console) {}
    };
};

