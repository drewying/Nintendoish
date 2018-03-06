#pragma once

#include "NES.h"
#include <stdint.h>

namespace NES {
	class PPUMemory {
	public:
		unsigned char vram[0x2000] = { 0 };    // Video Memory/Name Tables
		unsigned char    pal[0x20] = { 0x3F }; // Palette Memory. Initialized to black.
	
		unsigned char get(unsigned short index);
		void  set(unsigned short index, unsigned char value);

		Console &console;

		PPUMemory(Console &console) : console(console) {}
	};
};