#pragma once

#include "NES.h"
#include "Cartridge.h"

namespace NES {
	class Cartridge;
	class Mapper {
	public:
		Cartridge &cartridge;

		Mapper(Cartridge &cartridge) : cartridge(cartridge) {}
		~Mapper() {}

		virtual unsigned char getTileData(unsigned short index) { return 0x0;  };
		virtual void  setTileData(unsigned short index, unsigned char value) {};

		virtual unsigned char getProgramData(unsigned short index) { return 0x0;  };
		virtual void  setProgramData(unsigned short index, unsigned char value) {};
	};
}

