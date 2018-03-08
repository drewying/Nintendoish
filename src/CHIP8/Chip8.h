#pragma once
#include <string>

class Chip8
{
    uint8_t delayTimer;
    uint8_t soundTimer;
    uint8_t V[16];
    uint8_t memory[4096];
    uint8_t keys[16];
    uint16_t stack[16];
    uint16_t SP;
    uint16_t I;
    uint16_t PC;
    unsigned int   instructionCount;
public:
    bool drawFlag;
    uint8_t graphics[64 * 32];

    void handleKeyPress(uint8_t key, bool isKeyDown);
    void emulateCycle();
    void loadProgram(const char* path);
    void reset();
    Chip8();
    ~Chip8();
};

