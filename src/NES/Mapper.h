#pragma once

#include "NES.h"
#include "Cartridge.h"

#include <stdint.h>

namespace NES {
	class Cartridge;
	class Mapper {
	public:
		Cartridge &cartridge;

		Mapper(Cartridge &cartridge) : cartridge(cartridge) {}
		~Mapper() {}

		virtual uint8_t getTileData(uint16_t index) { return 0x0;  };
		virtual void  setTileData(uint16_t index, uint8_t value) {};

		virtual uint8_t getProgramData(uint16_t index) { return 0x0;  };
		virtual void  setProgramData(uint16_t index, uint8_t value) {};
	};
};

