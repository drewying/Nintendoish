#pragma once

#include "Mapper.h"

namespace NES {
	class Mapper1 :
		public Mapper
	{
	public:

		unsigned char loadRegister = 0x10;
		unsigned char controlRegister;

		Mapper1(Cartridge &cartridge) : Mapper(cartridge) {}

		unsigned char getTileData(unsigned short index);
		void  setTileData(unsigned short index, unsigned char value);

		unsigned char getProgramData(unsigned short index);
		void  setProgramData(unsigned short index, unsigned char value);
	};

}

