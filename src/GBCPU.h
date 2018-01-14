#pragma once

#include <functional>

class GBCPU
{
	struct registers {
		struct {
			union {
				struct {
					unsigned char f;
					unsigned char a;
				};
				unsigned short af;
			};
		};

		struct {
			union {
				struct {
					unsigned char c;
					unsigned char b;
				};
				unsigned short bc;
			};
		};

		struct {
			union {
				struct {
					unsigned char e;
					unsigned char d;
				};
				unsigned short de;
			};
		};

		struct {
			union {
				struct {
					unsigned char l;
					unsigned char h;
				};
				unsigned short hl;
			};
		};

		unsigned short sp;
		unsigned short pc;
	};

	struct clock {
		unsigned short m;
		unsigned short p;
	};

	registers r;
	clock c;

	unsigned char mem[0xFFFF];

	void loadWord(unsigned short *targetRegister);
	void loadByte(unsigned char *targetRegister);
	
public:
	void handleKeyPress(unsigned char key, bool isKeyDown);
	void createInstructionTable();
	void emulateCycle();
	void loadProgram(const char* path);
	void reset();
	GBCPU();
	~GBCPU();
};

