#pragma once
#include <string>

class Chip8
{
	unsigned char delayTimer;
	unsigned char soundTimer;
	unsigned char V[16];
	unsigned char memory[4096];
	unsigned char keys[16];
	unsigned short stack[16];
	unsigned short SP;
	unsigned short I;
	unsigned short PC;
	unsigned int   instructionCount;
public:
	bool drawFlag;
	unsigned char graphics[64 * 32];

	void handleKeyPress(unsigned char key, bool isKeyDown);
	void emulateCycle();
	void loadProgram(const char* path);
	void reset();
	Chip8();
	~Chip8();
};

