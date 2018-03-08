#include "Chip8.h"
#include <stdio.h>
#include <algorithm>
#include <stdlib.h>

uint8_t FONTSET[80] =
{
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

void Chip8::handleKeyPress(uint8_t key, bool isKeyDown)
{
    switch (key) {
    case '1':
        keys[0x1] = isKeyDown;
        break;
    case '2':
        keys[0x2] = isKeyDown;
        break;
    case '3':
        keys[0x3] = isKeyDown;
        break;
    case '4':
        keys[0xC] = isKeyDown;
        break;
    case 'q':
        keys[0x4] = isKeyDown;
        break;
    case 'w':
        keys[0x5] = isKeyDown;
        break;
    case 'e':
        keys[0x6] = isKeyDown;
        break;
    case 'r':
        keys[0xD] = isKeyDown;
        break;
    case 'a':
        keys[0x7] = isKeyDown;
        break;
    case 's':
        keys[0x8] = isKeyDown;
        break;
    case 'd':
        keys[0x9] = isKeyDown;
        break;
    case 'f':
        keys[0xE] = isKeyDown;
        break;
    case 'z':
        keys[0xA] = isKeyDown;
        break;
    case 'x':
        keys[0x0] = isKeyDown;
        break;
    case 'c':
        keys[0xB] = isKeyDown;
        break;
    case 'v':
        keys[0xF] = isKeyDown;
        break;
    }
}

void Chip8::emulateCycle()
{
    uint16_t opcode = memory[PC] << 8 | memory[PC + 1];
    uint8_t X = (opcode & 0x0F00) >> 8;
    uint8_t Y = (opcode & 0x00F0) >> 4;
    uint8_t VX = V[X];
    uint8_t VY = V[Y];
    uint16_t N = opcode & 0x000F;
    uint16_t NN = opcode & 0x00FF;
    uint16_t NNN = opcode & 0x0FFF;

    drawFlag = false;

    PC += 2;

    switch (opcode & 0xF000) {
        case 0x0000:
            switch (opcode & 0x00FF) {
            case 0xE0:
                // Clears the screen.
                std::fill_n(graphics, 64 * 32, 0x0);
                drawFlag = true;
                break;
            case 0xEE:
                // Returns from a subroutine.
                SP -= 1;
                PC = stack[SP];
                break;
            default:
                printf("Unknown op\n");
            }
            break;
        case 0x1000:
            // Jumps to address NNN.
            PC = NNN;
            break;
        case 0x2000:
            // Calls subroutine at NNN.
            stack[SP] = PC;
            SP += 1;
            PC = NNN;
            break;
        case 0x3000:
            // Skips the next instruction if VX equals NN
            if (VX == NN) PC += 2;
            break;
        case 0x4000:
            // Skips the next instruction if VX doesn't equal NN. 
            if (VX != NN) PC += 2;
            break;
        case 0x5000:
            // Skips the next instruction if VX equals VY
            if (VX == VY) PC += 2;
            break;
        case 0x6000:
            // Sets VX to NN.
            V[X] = NN;
            break;
        case 0x7000:
            // Adds NN to VX.
            V[X] += NN;
            break;
        case 0x8000:
            switch (opcode & 0x000F) {
                case 0x0:
                    // Sets VX to the value of VY.
                    V[X] = VY;
                    break;
                case 0x1:
                    // Sets VX to VX or VY
                    V[X] = VX | VY;
                    break;
                case 0x2:
                    // Sets VX to VX and VY. 
                    V[X] = VX & VY;
                    break;
                case 0x3:
                    // Sets VX to VX xor VY.
                    V[X] = VX ^ VY;
                    break;
                case 0x4:
                    // Adds VY to VX. VF is set to 1 when there's a carry, and to 0 when there isn't.
                    V[0xF] = VY > (0xFF - VX);
                    V[X] += VY;
                    break;
                case 0x5:
                    // VY is subtracted from VX. VF is set to 0 when there's a borrow, and 1 when there isn't
                    V[0xF] = VY <= VX;
                    V[X] = VX - VY;
                    break;
                case 0x6:
                    // Shifts VX right by one. VF is set to the value of the least significant bit of VX before the shift.
                    V[0xF] = VX & 1;
                    V[X] = VX >> 1;
                    break;
                case 0x7:
                    // Sets VX to VY minus VX. VF is set to 0 when there's a borrow, and 1 when there isn't.
                    V[0xF] = VX <= VY;
                    V[X] = VY - VX;
                    break;
                case 0xE:
                    // Shifts VX left by one. VF is set to the value of the most significant bit of VX before the shift.
                    V[0xF] = (VX & (0x80 != 0));
                    V[X] = VX << 1;
                    break;
                default:
                    printf("Unknown op\n");

            }
            break;
        case 0x9000:
            // Skips the next instruction if VX doesn't equal VY.
            if (VX != VY) PC += 2;
            break;
        case 0xA000:
            // Sets I to the address NNN.
            I = NNN;
            break;
        case 0xB000:
            // Jumps to the address NNN plus V0.
            PC = NNN + V[0x0];
            break;
        case 0xC000:
            // Sets VX to the result of a bitwise and operation on a random number (Typically: 0 to 255) and NN.
            V[X] = rand() & NN;
            break;
        case 0xD000:
            // Draws a sprite at coordinate (VX, VY)
            uint16_t pixel;
            V[0xF] = 0;
            for (int yline = 0; yline < N; yline++)
            {
                pixel = memory[I + yline];
                for (int xline = 0; xline < 8; xline++)
                {
                    if ((pixel & (0x80 >> xline)) != 0)
                    {
                        if (graphics[(VX + xline + ((VY + yline) * 64))] == 1) 
                            V[0xF] = 1;
                        graphics[VX + xline + ((VY + yline) * 64)] ^= 1;
                    }
                }
            }

            drawFlag = true;
            break;
        case 0xE000:
            switch (opcode & 0x00FF) {
                case 0x9E:
                    //Skips the next instruction if the key stored in VX is pressed
                    if (keys[VX]) PC += 2;
                    break;
                case 0xA1:
                    //Skips the next instruction if the key stored in VX isn't pressed
                    if (keys[VX] != true) PC += 2;
                    break;
                default:
                    printf("Unknown op\n");
            }
            break;
        case 0xF000:
            switch (opcode & 0x00FF) {
                case 0x07:
                    // Sets VX to the value of the delay timer.
                    V[X] = delayTimer;
                    break;
                case 0x0A:
                    for (int i = 0; i < 0xF; i++) {
                        if (keys[i]) {
                            PC -= 2;
                            break;
                        }
                    }
                    break;
                case 0x15:
                    // Sets the delay timer to VX.
                    delayTimer = VX;
                    break;
                case 0x18:
                    // Sets the sound timer to VX.
                    soundTimer = VX;
                    break;
                case 0x1E:
                    // Adds VX to I
                    I += VX;
                    break;
                case 0x29:
                    // Sets I to the location of the sprite for the character in VX.
                    I = VX * 5;
                    break;
                case 0x33:
                    // Stores the binary-coded decimal representation of VX.
                    memory[I] = VX / 100;
                    memory[I + 1] = (VX / 10) % 10;
                    memory[I + 2] = (VX % 100) % 10;
                    break;
                case 0x55:
                    // Stores V0 to VX (including VX) in memory starting at address I.
                    for (int i = 0; i <= X; i++) {
                        memory[I + i] = V[i];
                    }
                    break;
                case 0x65:
                    // Fills V0 to VX (including VX) with values from memory starting at address I.
                    for (int i = 0; i <= X; i++) {
                        V[i] = memory[I + i];
                    }
                    break;
                default:
                    printf("Unknown op\n");
            }
            break;
        default:
            printf("Unknown op\n");
    }
    instructionCount += 1;
    if (instructionCount % 9 == 0) {
        instructionCount = 0;
        if (delayTimer > 0) delayTimer -= 1;
        if (soundTimer > 0) soundTimer -= 1;
    }
}

void Chip8::loadProgram(const char* path)
{
    FILE *rom = fopen(path, "rb");
    if (rom) {
        fread(memory + 0x200, sizeof(char), 4096, rom);
    }
    printf("Loaded\n");
}

void Chip8::reset()
{
    PC = 0x200;
    I = 0x0;
    SP = 0x0;
    delayTimer = 0x0;
    soundTimer = 0x0;
    instructionCount = 0;

    std::fill_n(memory, 4096, 0x0);
    std::fill_n(V, 16, 0x0);
    std::fill_n(keys, 16, 0x0);
    std::fill_n(graphics, 64 * 32, 0x0);

    for (int i = 0; i < 80; i++) {
        memory[i] = FONTSET[i];
    }
}
Chip8::Chip8()
{
    reset();
}


Chip8::~Chip8()
{
}
