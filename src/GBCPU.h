#pragma once

#include <functional>

class GBCPU
{
	struct registers {
		struct {
			union {
				struct {
					uint8_t f;
					uint8_t a;
				};
				uint16_t af;
			};
		};

		struct {
			union {
				struct {
					uint8_t c;
					uint8_t b;
				};
				uint16_t bc;
			};
		};

		struct {
			union {
				struct {
					uint8_t e;
					uint8_t d;
				};
				uint16_t de;
			};
		};

		struct {
			union {
				struct {
					uint8_t l;
					uint8_t h;
				};
				uint16_t hl;
			};
		};

		uint16_t sp;
		uint16_t pc;
	};

	struct clock {
		uint16_t m;
		uint16_t p;
	};

	registers r;
	clock c;

	uint8_t mem[0xFFFF];

	void loadWord(uint16_t *targetRegister);
	void loadByte(uint8_t *targetRegister);
	
public:
	void handleKeyPress(uint8_t key, bool isKeyDown);
	void createInstructionTable();
	void emulateCycle();
	void loadProgram(const char* path);
	void reset();
	GBCPU();
	~GBCPU();
};

