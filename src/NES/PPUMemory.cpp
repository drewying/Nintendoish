#include "PPUMemory.h"

using namespace NES;

uint8_t PPUMemory::get(uint16_t index) {
    if (index < 0x2000) {
        // Pattern Table Access
        return console.cartridge->getTileData(index);
    } else if (index < 0x3000) {
        // Name Table Access
        return vram[mirrorIndex(index)];
    } else if (index < 0x3F00) { 
        // Name Table Mirroring
        return vram[index - 0x3000];
    } else  if (index < 0x4000) {
        // Palette Access
        if (index >= 0x3F10 && (index - 0x3F10) % 4 == 0) index -= 0x10; // Palette Mirroring
        return pal[index - 0x3F00];
    }
    return 0x0;
}

void PPUMemory::set(uint16_t index, uint8_t value) {
    if (index < 0x2000) {
        // Pattern Table Access
        console.cartridge->setTileData(index, value);
    } else if (index < 0x3000) {
        // Name Table Access
        vram[mirrorIndex(index)] = value;
    } else if (index < 0x3F00) {
        // Name Table Mirroring
        vram[index - 0x3000] = value;
    } else if (index < 0x4000) {
        // Palette Access
        if (index >= 0x3F10 && (index - 0x3F10) % 0x4 == 0) index -= 0x10; // Pallette Mirroring
        pal[index - 0x3F00] = value;
    }
}

uint16_t PPUMemory::mirrorIndex(uint16_t index) {
    bool horizontal = false;
    if (horizontal) {
        // Horizontal Mirroring
        index -= 0x2000;
        if (index % 0x800 >= 0x400) index -= 0x400;
        return vram[index];
    }
    else {
        // Vertical Mirroring
        if (index < 0x2800) {
            return vram[index - 0x2000];
        }
        else {
            return vram[index - 0x2800];
        }
    }
}