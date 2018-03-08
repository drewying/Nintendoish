#pragma once

#include "Mapper.h"

namespace NES {
	class Mapper1 :
		public Mapper
	{
	public:

		uint8_t loadRegister = 0x10;
		uint8_t controlRegister;

		Mapper1(Cartridge &cartridge) : Mapper(cartridge) {}

		uint8_t getTileData(uint16_t index);
		void  setTileData(uint16_t index, uint8_t value);

		uint8_t getProgramData(uint16_t index);
		void  setProgramData(uint16_t index, uint8_t value);
	};

}

