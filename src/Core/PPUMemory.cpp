#include "PPUMemory.h"
#include "Cartridge.h"

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
        return vram[mirrorIndex(index - 0x1000)];
    } else  if (index < 0x4000) {
        // Palette Access
        index -= 0x3F00;
        index %= 0x20; // Palette Mirroring
        if (index >= 0x10 && index % 0x4 == 0) {
            index -=0x10;
        }
        return pal[index];
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
        vram[mirrorIndex(index - 0x1000)] = value;
    } else if (index < 0x4000) {
        // Palette Access
        index -= 0x3F00;
        index %= 0x20; // Palette Mirroring
        if (index >= 0x10 && index % 0x4 == 0) {
            index -=0x10;
        }
        pal[index] = value;
    }
}

uint16_t PPUMemory::mirrorIndex(uint16_t index) {
    index -= 0x2000;
    
    switch (console.cartridge->currentMirroring) {
    case Cartridge::HorizontalMirror:
        if (index >= 0x400) index -= 0x400;
        if (index >= 0x800) index -= 0x400;
        return index;
        break;
    case Cartridge::VerticalMirror:
        if (index >= 0x800) index -= 0x800;
        return index;
        break;
    case Cartridge::SingleScreenA:
        return index % 0x400;
        break;
    case Cartridge::SingleScreenB:
        return (index % 0x400) + 0x400;
        break;
    case Cartridge::FourScreen:
        break;
    }
    return 0x0;
}
