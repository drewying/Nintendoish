#pragma once

#include "Mapper.h"

namespace NES {
	class Mapper;
	class Cartridge {
	public:
		Mapper* mapper;
		unsigned char* prg;
		unsigned char* chr;
		unsigned char prgSize;
		unsigned char chrSize;
		unsigned char mapperNumber;
		bool batteryBackup = false;
		bool verticalMirroring = false;
		bool horizontalMirroring = false;
		
		unsigned char getTileData(unsigned short index);
		void setTileData(unsigned short index, unsigned char value);

		unsigned char getProgramData(unsigned short index);
		void  setProgramData(unsigned short index, unsigned char value);

		Cartridge(const char* path);
		~Cartridge();
	};
}
