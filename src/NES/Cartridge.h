#pragma once

#include "Mapper.h"

#include <stdint.h>

namespace NES {
	class Mapper;
	class Cartridge {
	public:
		Mapper* mapper;
		uint8_t* prg;
		uint8_t* chr;
		uint8_t prgSize;
		uint8_t chrSize;
		uint8_t mapperNumber;
		bool batteryBackup = false;
		bool verticalMirroring = false;
		bool horizontalMirroring = false;
		
		uint8_t getTileData(uint16_t index);
		void setTileData(uint16_t index, uint8_t value);

		uint8_t getProgramData(uint16_t index);
		void  setProgramData(uint16_t index, uint8_t value);

		Cartridge(const char* path);
		~Cartridge();
	};
};
