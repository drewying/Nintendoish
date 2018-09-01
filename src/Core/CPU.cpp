#include "CPU.h"
#include "NES.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <stdio.h>

using namespace std;
using namespace NES;

CPU::CPU(NES::Memory &memory):
    memory(memory),
    debugTable {
        "BRK", "ORA", "STP", "SLO", "NOP", "ORA", "ASL", "SLO", "PHP", "ORA", "ASL", "ANC", "NOP", "ORA", "ASL", "SLO",
        "BPL", "ORA", "STP", "SLO", "NOP", "ORA", "ASL", "SLO", "CLC", "ORA", "NOP", "SLO", "NOP", "ORA", "ASL", "SLO",
        
        "JSR", "AND", "STP", "RLA", "BIT", "AND", "ROL", "RLA", "PLP", "AND", "ROL", "ANC", "BIT", "AND", "ROL", "RLA",
        "BMI", "AND", "STP", "RLA", "NOP", "AND", "ROL", "RLA", "SEC", "AND", "NOP", "RLA", "NOP", "AND", "ROL", "RLA",
        
        "RTI", "EOR", "STP", "SRE", "NOP", "EOR", "LSR", "SRE", "PHA", "EOR", "LSR", "ALR", "JMP", "EOR", "LSR", "SRE",
        "BVC", "EOR", "STP", "SRE", "NOP", "EOR", "LSR", "SRE", "CLI", "EOR", "NOP", "SRE", "NOP", "EOR", "LSR", "SRE",
        
        "RTS", "ADC", "STP", "RRA", "NOP", "ADC", "ROR", "RRA", "PLA", "ADC", "ROR", "ARR", "JMP", "ADC", "ROR", "RRA",
        "BVS", "ADC", "STP", "RRA", "NOP", "ADC", "ROR", "RRA", "SEI", "ADC", "NOP", "RRA", "NOP", "ADC", "ROR", "RRA",
        
        "NOP", "STA", "NOP", "SAX", "STY", "STA", "STX", "SAX", "DEY", "NOP", "TXA", "XAA", "STY", "STA", "STX", "SAX",
        "BCC", "STA", "STP", "AHX", "STY", "STA", "STX", "SAX", "TYA", "STA", "TXS", "TAS", "SHY", "STA", "SHX", "AHX",
        
        "LDY", "LDA", "LDX", "LAX", "LDY", "LDA", "LDX", "LAX", "TAY", "LDA", "TAX", "LAX", "LDY", "LDA", "LDX", "LAX",
        "BCS", "LDA", "STP", "LAX", "LDY", "LDA", "LDX", "LAX", "CLV", "LDA", "TSX", "LAS", "LDY", "LDA", "LDX", "LAX",
        
        "CPY", "CMP", "NOP", "DCP", "CPY", "CMP", "DEC", "DCP", "INY", "CMP", "DEX", "AXS", "CPY", "CMP", "DEC", "DCP",
        "BNE", "CMP", "STP", "DCP", "NOP", "CMP", "DEC", "DCP", "CLD", "CMP", "NOP", "DCP", "NOP", "CMP", "DEC", "DCP",
        
        "CPX", "SBC", "NOP", "ISB", "CPX", "SBC", "INC", "ISB", "INX", "SBC", "NOP", "SBC", "CPX", "SBC", "INC", "ISB",
        "BEQ", "SBC", "STP", "ISB", "NOP", "SBC", "INC", "ISB", "SED", "SBC", "NOP", "ISB", "NOP", "SBC", "INC", "ISB"
    },
    opTable {
        &CPU::BRK, &CPU::ORA, &CPU::STP, &CPU::SLO, &CPU::NOP, &CPU::ORA, &CPU::ASL, &CPU::SLO, &CPU::PHP, &CPU::ORA, &CPU::ASL, &CPU::ANC, &CPU::NOP, &CPU::ORA, &CPU::ASL, &CPU::SLO,
        &CPU::BPL, &CPU::ORA, &CPU::STP, &CPU::SLO, &CPU::NOP, &CPU::ORA, &CPU::ASL, &CPU::SLO, &CPU::CLC, &CPU::ORA, &CPU::NOP, &CPU::SLO, &CPU::NOP, &CPU::ORA, &CPU::ASL, &CPU::SLO,
        
        &CPU::JSR, &CPU::AND, &CPU::STP, &CPU::RLA, &CPU::BIT, &CPU::AND, &CPU::ROL, &CPU::RLA, &CPU::PLP, &CPU::AND, &CPU::ROL, &CPU::ANC, &CPU::BIT, &CPU::AND, &CPU::ROL, &CPU::RLA,
        &CPU::BMI, &CPU::AND, &CPU::STP, &CPU::RLA, &CPU::NOP, &CPU::AND, &CPU::ROL, &CPU::RLA, &CPU::SEC, &CPU::AND, &CPU::NOP, &CPU::RLA, &CPU::NOP, &CPU::AND, &CPU::ROL, &CPU::RLA,
        
        &CPU::RTI, &CPU::EOR, &CPU::STP, &CPU::SRE, &CPU::NOP, &CPU::EOR, &CPU::LSR, &CPU::SRE, &CPU::PHA, &CPU::EOR, &CPU::LSR, &CPU::ALR, &CPU::JMP, &CPU::EOR, &CPU::LSR, &CPU::SRE,
        &CPU::BVC, &CPU::EOR, &CPU::STP, &CPU::SRE, &CPU::NOP, &CPU::EOR, &CPU::LSR, &CPU::SRE, &CPU::CLI, &CPU::EOR, &CPU::NOP, &CPU::SRE, &CPU::NOP, &CPU::EOR, &CPU::LSR, &CPU::SRE,
        
        &CPU::RTS, &CPU::ADC, &CPU::STP, &CPU::RRA, &CPU::NOP, &CPU::ADC, &CPU::ROR, &CPU::RRA, &CPU::PLA, &CPU::ADC, &CPU::ROR, &CPU::ARR, &CPU::JMP, &CPU::ADC, &CPU::ROR, &CPU::RRA,
        &CPU::BVS, &CPU::ADC, &CPU::STP, &CPU::RRA, &CPU::NOP, &CPU::ADC, &CPU::ROR, &CPU::RRA, &CPU::SEI, &CPU::ADC, &CPU::NOP, &CPU::RRA, &CPU::NOP, &CPU::ADC, &CPU::ROR, &CPU::RRA,
        
        &CPU::NOP, &CPU::STA, &CPU::NOP, &CPU::SAX, &CPU::STY, &CPU::STA, &CPU::STX, &CPU::SAX, &CPU::DEY, &CPU::NOP, &CPU::TXA, &CPU::XAA, &CPU::STY, &CPU::STA, &CPU::STX, &CPU::SAX,
        &CPU::BCC, &CPU::STA, &CPU::STP, &CPU::AHX, &CPU::STY, &CPU::STA, &CPU::STX, &CPU::SAX, &CPU::TYA, &CPU::STA, &CPU::TXS, &CPU::TAS, &CPU::SHY, &CPU::STA, &CPU::SHX, &CPU::AHX,
        
        &CPU::LDY, &CPU::LDA, &CPU::LDX, &CPU::LAX, &CPU::LDY, &CPU::LDA, &CPU::LDX, &CPU::LAX, &CPU::TAY, &CPU::LDA, &CPU::TAX, &CPU::LAX, &CPU::LDY, &CPU::LDA, &CPU::LDX, &CPU::LAX,
        &CPU::BCS, &CPU::LDA, &CPU::STP, &CPU::LAX, &CPU::LDY, &CPU::LDA, &CPU::LDX, &CPU::LAX, &CPU::CLV, &CPU::LDA, &CPU::TSX, &CPU::LAS, &CPU::LDY, &CPU::LDA, &CPU::LDX, &CPU::LAX,
        
        &CPU::CPY, &CPU::CMP, &CPU::NOP, &CPU::DCP, &CPU::CPY, &CPU::CMP, &CPU::DEC, &CPU::DCP, &CPU::INY, &CPU::CMP, &CPU::DEX, &CPU::AXS, &CPU::CPY, &CPU::CMP, &CPU::DEC, &CPU::DCP,
        &CPU::BNE, &CPU::CMP, &CPU::STP, &CPU::DCP, &CPU::NOP, &CPU::CMP, &CPU::DEC, &CPU::DCP, &CPU::CLD, &CPU::CMP, &CPU::NOP, &CPU::DCP, &CPU::NOP, &CPU::CMP, &CPU::DEC, &CPU::DCP,
        
        &CPU::CPX, &CPU::SBC, &CPU::NOP, &CPU::ISB, &CPU::CPX, &CPU::SBC, &CPU::INC, &CPU::ISB, &CPU::INX, &CPU::SBC, &CPU::NOP, &CPU::SBC, &CPU::CPX, &CPU::SBC, &CPU::INC, &CPU::ISB,
        &CPU::BEQ, &CPU::SBC, &CPU::STP, &CPU::ISB, &CPU::NOP, &CPU::SBC, &CPU::INC, &CPU::ISB, &CPU::SED, &CPU::SBC, &CPU::NOP, &CPU::ISB, &CPU::NOP, &CPU::SBC, &CPU::INC, &CPU::ISB
    },
    timingTable {
        0, 6, 2, 8, 3, 3, 5, 5, 3, 2, 2, 2, 4, 4, 6, 6,
        2, 5, 2, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7,
        6, 6, 2, 8, 3, 3, 5, 5, 4, 2, 2, 2, 4, 4, 6, 6,
        2, 5, 2, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7,
        6, 6, 2, 8, 3, 3, 5, 5, 3, 2, 2, 2, 3, 4, 6, 6,
        2, 5, 2, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7,
        6, 6, 2, 8, 3, 3, 5, 5, 4, 2, 2, 2, 5, 4, 6, 6,
        2, 5, 2, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7,
        2, 6, 2, 6, 3, 3, 3, 3, 2, 2, 2, 2, 4, 4, 4, 4,
        2, 6, 2, 6, 4, 4, 4, 4, 2, 5, 2, 5, 5, 5, 5, 5,
        2, 6, 2, 6, 3, 3, 3, 3, 2, 2, 2, 2, 4, 4, 4, 4,
        2, 5, 2, 5, 4, 4, 4, 4, 2, 4, 2, 4, 4, 4, 4, 4,
        2, 6, 2, 8, 3, 3, 5, 5, 2, 2, 2, 2, 4, 4, 6, 6,
        2, 5, 2, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7,
        2, 6, 2, 8, 3, 3, 5, 5, 2, 2, 2, 2, 4, 4, 6, 6,
        2, 5, 2, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7,
    },
    addressTable {
        0, 11, 0, 11, 5, 5, 5, 5, 0,  1, 4,  1, 8, 8,  8,  8,
        3, 12, 0, 12, 6, 6, 6, 6, 0, 10, 0, 10, 9, 9,  9,  9,
        
        8, 11, 0, 11, 5, 5, 5, 5, 0,  1, 4,  1, 8, 8,  8,  8,
        3, 12, 0, 12, 6, 6, 6, 6, 0, 10, 0, 10, 9, 9,  9,  9,
        
        0, 11, 0, 11, 5, 5, 5, 5, 0,  1, 4,  1, 8, 8,  8,  8,
        3, 12, 0, 12, 6, 6, 6, 6, 0, 10, 0, 10, 9, 9,  9,  9,
        
        0, 11, 0, 11, 5, 5, 5, 5, 0,  1, 4,  1, 2, 8,  8,  8,
        3, 12, 0, 12, 6, 6, 6, 6, 0, 10, 0, 10, 9, 9,  9,  9,
        
        1, 11, 1, 11, 5, 5, 5, 5, 0,  1, 4,  1, 8, 8,  8,  8,
        3, 12, 0, 12, 6, 6, 7, 7, 0, 10, 0, 10, 9, 9, 10, 10,
        
        1, 11, 1, 11, 5, 5, 5, 5, 0,  1, 4,  1, 8, 8,  8,  8,
        3, 12, 0, 12, 6, 6, 7, 7, 0, 10, 0, 10, 9, 9, 10, 10,
        
        1, 11, 1, 11, 5, 5, 5, 5, 0,  1, 4,  1, 8, 8,  8,  8,
        3, 12, 0, 12, 6, 6, 6, 6, 0, 10, 0, 10, 9, 9,  9,  9,
        
        1, 11, 1, 11, 5, 5, 5, 5, 0,  1, 4,  1, 8, 8,  8,  8,
        3, 12, 0, 12, 6, 6, 6, 6, 0, 10, 0, 10, 9, 9,  9,  9
    }
    {
        reg.P.byte = 0x24;
    }

CPU::~CPU() { }


uint8_t CPU::readProgram() {
    return memory.get(reg.PC++);
}

void CPU::executeLoadedInstruction() {
    (this->*opTable[loadedInstruction])(loadedAddress);
}

void CPU::loadNextInstruction() {
    //Load next instruction
    loadedInstruction = readProgram();
    stallCycles += timingTable[loadedInstruction];
    
    //Load next address
    uint8_t hi = 0x0;
    uint8_t lo = 0x0;
    loadedAddress = 0x0;
    pageBoundryCross = false;
    currentAddressMode = static_cast<AddressMode>(addressTable[loadedInstruction]);
    switch (currentAddressMode) {
        case Accumulator:
            // The Accumulator is implied as the operand, so no address needs to be specified.
            break;
        case Implicit:
            // The operand is implied, so it does not need to be specified.
            break;
        case Immediate:
            // The operand is used directly to perform the computation.
            loadedAddress = reg.PC;
            lo = readProgram();
            break;
        case Relative:
            // Branch instructions(e.g.BEQ, BCS) have a relative addressing mode that
            // specifies an 8 - bit signed offset relative to the current PC.
            lo = readProgram();
            loadedAddress = reg.PC + (int8_t)lo;
            break;
        case ZeroPage:
            // A single byte specifies an address in the first page of memory ($00xx),
            // also known as the zero page, and the byte at that address is used to perform the computation.
            lo = readProgram();
            loadedAddress = lo;
            break;
        case ZeroPageIndexX:
            // The value in X is added to the specified zero page address for a sum address.
            lo = readProgram();
            loadedAddress = (lo + reg.X) % 256;
            break;
        case ZeroPageIndexY:
            // The value in Y is added to the specified zero page address for a sum address.
            lo = readProgram();
            loadedAddress = (lo + reg.Y) % 256;
            break;
        case Indirect:
            // The JMP instruction has a special indirect addressing mode that can jump to the address stored in a
            // 16-bit pointer anywhere in memory.
            lo = readProgram();
            hi = readProgram();
            loadedAddress = (uint16_t)(memory.get(hi << 8 | lo) + memory.get((hi << 8 | (uint8_t)(lo + 1))) * 256);
            break;
        case Absolute:
            // A full 16 - bit address is specified and the byte at that address is used to perform the computation.
            lo = readProgram();
            hi = readProgram();
            loadedAddress = hi << 8 | lo;
            break;
        case AbsoluteIndexX:
            // The value in X is added to the specified address for a sum address.
            lo = readProgram();
            hi = readProgram();
            loadedAddress = (hi << 8 | lo) + reg.X;
            break;
        case AbsoluteIndexY:
            // The value in X is added to the specified address for a sum address.
            lo = readProgram();
            hi = readProgram();
            loadedAddress = (hi << 8 | lo) + reg.Y;
            break;
        case IndexIndirectX:
            // The value in X is added to the specified zero page address for a sum address.
            // The little-endian address stored at the two-byte pair of sum address (LSB) and sum address plus one (MSB)
            // is loaded and the value at that address is used to perform the computation.
            lo = readProgram();
            loadedAddress = (uint16_t)(memory.get((lo + reg.X) % 256) + memory.get((lo + reg.X + 1) % 256) * 256);
            break;
        case IndirectIndexY:
            // The value in Y is added to the address at the little-endian address stored at the two-byte pair of the
            // specified address (LSB) and the specified address plus one (MSB). The value at the sum address is used
            // to perform the computation. Indeed addressing mode actually repeats exactly the accumulator register's digits.
            lo = readProgram();
            loadedAddress = (uint16_t)(memory.get(lo) + memory.get((lo + 1) % 256) * 256 + reg.Y);
            break;
        default:
            break;
    }
}

void CPU::reset() {
    reg.A = 0;
    reg.X = 0;
    reg.Y = 0;
    reg.S = 0xFD;
    reg.P.status.Unused = true;
    reg.P.status.Interrupt = true;
    reg.S = 0xFD;
    reg.PC = memory.resetVector();
    stallCycles = 0x0;
    totalCycles = 0x0;
    loadNextInstruction();
}

void CPU::step() {
    totalCycles++;
    stallCycles--;
    if (stallCycles > 0x2) {
        return;
    }
    if (stallCycles == 0x2) {
        pollInterrurpts();
        return;
    }
    
    executeLoadedInstruction();
    executeInterrurpts();
    loadNextInstruction();
}

void CPU::setNZStatus(uint8_t value) {
    reg.P.status.Zero = (value == 0x0);
    reg.P.status.Negative = ((value & 0x80) != 0);
}

void CPU::branchOnCondition(bool condition, uint16_t address) {
    if (condition) {
        stallCycles++;
        checkForPageCross(address);
        reg.PC = address;
    }
}

void CPU::checkForPageCross(uint16_t address) {
    uint16_t indexAddress;
    switch (currentAddressMode) {
        case AbsoluteIndexX:
            indexAddress = address - reg.X;
            break;
        case AbsoluteIndexY:
        case IndirectIndexY:
            indexAddress = address - reg.Y;
            break;
        case Relative:
            indexAddress = reg.PC;
            break;
        default:
            indexAddress = 0x0;
            break;
    }
    
    if (indexAddress != 0x0 && ((address & 0xFF00) != (indexAddress & 0xFF00))){
        stallCycles++;
        //Dummy read
        memory.get(address - 0x100);
    }
}

void CPU::executeInterrurpts() {
    if (doNMI == true) {
        NMI();
    }
    if (doIRQ == true) {
        IRQ();
    }
    doNMI = false;
    doIRQ = false;
}

void CPU::pollInterrurpts() {
    if (requestNMI == true) {
        doNMI = true;
        requestNMI = false;
    } else if (requestIRQ == true) {
        if (reg.P.status.Interrupt == false) {
            doIRQ = true;
        }
        requestIRQ = false;
    }
}

void CPU::compareValues(uint8_t a, uint8_t b) {
    setNZStatus(a - b);
    reg.P.status.Carry = (a >= b);
}

void CPU::push(uint8_t byte) {
    memory.set(0x100 | reg.S--, byte);
}

uint8_t CPU::pull() {
    return memory.get(0x100 | ++reg.S);
}

void CPU::pushAddress(uint16_t address) {
    uint8_t lo = address & 0x00FF;
    uint8_t hi = address >> 8;
    push(hi);
    push(lo);
}

uint16_t CPU::pullAddress() {
    uint8_t lo = pull();
    uint8_t hi = pull();
    return hi << 8 | lo;
}

void CPU::NMI() {
    // Non-Maskable Interrupt
    pushAddress(reg.PC);
    PHP(0x0);
    reg.PC = memory.get(0xFFFB) << 8 | memory.get(0xFFFA);
    // reg.P.status.Interrupt = true;
    stallCycles += 7;
}

void CPU::IRQ() {
    // IRQ Interrupt
    pushAddress(reg.PC);
    PHP(0x0);
    reg.PC = memory.get(0xFFFF) << 8 | memory.get(0xFFFE);
    reg.P.status.Interrupt = true;
    stallCycles += 7;
}

void CPU::BCC(uint16_t address) {
    //Branch on Carry Clear
    branchOnCondition(reg.P.status.Carry == 0, address);
}

void CPU::BCS(uint16_t address) {
    //Branch on Carry Set
    branchOnCondition(reg.P.status.Carry == 1, address);
}

void CPU::BEQ(uint16_t address) {
    //Branch on Result Zero
    branchOnCondition(reg.P.status.Zero == 1, address);
}

void CPU::BIT(uint16_t address) {
    //Test Bits in Memory with Accumulator
    uint8_t value = memory.get(address);
    reg.P.status.Overflow = (value >> 6) & 1;
    reg.P.status.Zero = (value & reg.A) == 0;
    reg.P.status.Negative = (value & 0x80) != 0;
}

void CPU::BMI(uint16_t address) {
    //Branch on Result Minus
    branchOnCondition(reg.P.status.Negative == 1, address);
}

void CPU::BNE(uint16_t address) {
    //Branch on Result not Zero
    branchOnCondition(reg.P.status.Zero == 0, address);
}

void CPU::BPL(uint16_t address) {
    //Branch on Result Plus
    branchOnCondition(reg.P.status.Negative == 0, address);
}

void CPU::BRK(uint16_t address) {
    //Force Inturrupt
    reg.PC += 1;
    IRQ();
}

void CPU::BVC(uint16_t address) {
    //Branch on Overflow Clear
    branchOnCondition(reg.P.status.Overflow == 0, address);
}

void CPU::BVS(uint16_t address) {
    //Branch on Overflow Set
    branchOnCondition(reg.P.status.Overflow == 1, address);
}

void CPU::CLC(uint16_t address) {
    //Clear Carry Flag
    reg.P.status.Carry = 0;
}

void CPU::CLD(uint16_t address) {
    //Clear Decimal Mode
    reg.P.status.Decimal = 0;
}

void CPU::CLI(uint16_t address) {
    //Clear Interrupt Disable Status
    reg.P.status.Interrupt = 0;
}

void CPU::CLV(uint16_t address) {
    //Clear Overflow Flag
    reg.P.status.Overflow = 0;
}

void CPU::CPX(uint16_t address) {
    //Compare Memory and Index X
    compareValues(reg.X, memory.get(address));
}

void CPU::CPY(uint16_t address) {
    //Compare Memory with Index Y
    compareValues(reg.Y, memory.get(address));
}

void CPU::DEX(uint16_t address) {
    //Decrement Index X by One
    //Flags: N, Z
    reg.X -= 1;
    setNZStatus(reg.X);
}

void CPU::DEY(uint16_t address) {
    //Decrement Index Y by One
    //Flags: N, Z
    reg.Y -= 1;
    setNZStatus(reg.Y);
}

void CPU::INX(uint16_t address) {
    //Increment Index X by One
    //Flags: N, Z
    reg.X += 1;
    setNZStatus(reg.X);
}

void CPU::INY(uint16_t address) {
    //Increment Index Y by One
    //Flags: N, Z
    reg.Y += 1;
    setNZStatus(reg.Y);
}

void CPU::LDX(uint16_t address) {
    //Load Index X with Memory
    //Flags: N, Z
    checkForPageCross(address);
    reg.X = memory.get(address);
    setNZStatus(reg.X);
}

void CPU::LDY(uint16_t address) {
    //Load Index Y with Memory
    //Flags: N, Z
    checkForPageCross(address);
    reg.Y = memory.get(address);
    setNZStatus(reg.Y);
}

void CPU::JMP(uint16_t address) {
    //Jump to New Location
    reg.PC = address;
}

void CPU::JSR(uint16_t address) {
    //Jump to New Location Saving Return Address
    pushAddress(reg.PC - 1);
    reg.PC = address;
}

void CPU::NOP(uint16_t address) {
    //No Operation
    checkForPageCross(address);
}

void CPU::PHA(uint16_t address) {
    //Push Accumulator on Stack
    push(reg.A);
}

void CPU::PHP(uint16_t address) {
    //Push Processor Status on Stack
    uint8_t s = reg.P.byte;
    s |= 0x30;
    push(s);
}

void CPU::PLA(uint16_t address) {
    //Pull Accumulator from Stack
    //Flags: N, Z
    reg.A = pull();
    setNZStatus(reg.A);
}

void CPU::PLP(uint16_t address) {
    //Pull Processor Status from Stack
    uint8_t s = pull();
    reg.P.status.Negative  = (s & 128) != 0x0;
    reg.P.status.Overflow  = (s & 64)  != 0x0;
    reg.P.status.Decimal   = (s & 8)   != 0x0;
    reg.P.status.Interrupt = (s & 4)   != 0x0;
    reg.P.status.Zero      = (s & 2)   != 0x0;
    reg.P.status.Carry     = (s & 1)   != 0x0;
}

void CPU::RTI(uint16_t address) {
    //Return from interrupt
    //Flags: all
    reg.P.byte = ((pull() & 0xEF) | 0x20);
    reg.PC = pullAddress();
}

void CPU::RTS(uint16_t address) {
    //Return from Subroutines
    reg.PC = pullAddress() + 1;
}

void CPU::SEC(uint16_t address) {
    //Set Carry Flag
    reg.P.status.Carry = 1;
}

void CPU::SED(uint16_t address) {
    //Set Decimal Mode
    reg.P.status.Decimal = 1;
}

void CPU::SEI(uint16_t address) {
    //Set Interrupt Disable Status
    reg.P.status.Interrupt = 1;
}

void CPU::STX(uint16_t address) {
    //Store Index X in Memory
    memory.set(address, reg.X);
}

void CPU::STY(uint16_t address) {
    //Store Index Y in Memory
    memory.set(address, reg.Y);
}

void CPU::TAX(uint16_t address) {
    //Transfer Accumulator to Index X
    //Flags: N, Z
    reg.X = reg.A;
    setNZStatus(reg.X);
}

void CPU::TAY(uint16_t address) {
    //Transfer Accumulator to Index Y
    //Flags: N, Z
    reg.Y = reg.A;
    setNZStatus(reg.Y);
}

void CPU::TSX(uint16_t address) {
    //Transfer Stack Pointer to Index X
    //Flags: N, Z
    reg.X = reg.S;
    setNZStatus(reg.X);
}

void CPU::TYA(uint16_t address) {
    //Transfer Index Y to Accumulator
    //Flags: N, Z
    reg.A = reg.Y;
    setNZStatus(reg.A);
}

void CPU::TXA(uint16_t address) {
    //Transfer Index X to Accumulator
    //Flags: N, Z
    reg.A = reg.X;
    setNZStatus(reg.A);
}

void CPU::TXS(uint16_t address) {
    //Transfer Index X to Stack Pointer
    reg.S = reg.X;
}

// ALU
void CPU::ORA(uint16_t address) {
    //OR Memory with Accumulator
    //Flags: N, Z
    checkForPageCross(address);
    reg.A = reg.A | memory.get(address);
    setNZStatus(reg.A);
}

void CPU::AND(uint16_t address) {
    //AND Memory with Accumulator
    //Flags: N, Z
    checkForPageCross(address);
    reg.A = reg.A & memory.get(address);
    setNZStatus(reg.A);
}

void CPU::EOR(uint16_t address) {
    //Exclusive-OR Memory with Accumulator
    //Flags: N, Z
    checkForPageCross(address);
    reg.A = reg.A ^ memory.get(address);
    setNZStatus(reg.A);
}

void CPU::ADC(uint16_t address) {
    //Add Memory to Accumulator with Carry
    //Flags: N, V, Z, C
    checkForPageCross(address);
    uint8_t a = reg.A;
    reg.A = reg.A + memory.get(address) + reg.P.status.Carry;
    
    reg.P.status.Carry = ((int)a + (int)memory.get(address) + (int)reg.P.status.Carry) > 0xFF;
    reg.P.status.Overflow = ((a ^ memory.get(address)) & 0x80) == 0 &&
    ((a ^ reg.A) & 0x80) != 0;
    
    setNZStatus(reg.A);
}

void CPU::STA(uint16_t address) {
    //Dummy Read
    memory.get(address - 0x100);
    
    //Store Accumulator in Memory
    memory.set(address, reg.A);
}

void CPU::LDA(uint16_t address) {
    //Load Accumulator with Memory
    //Flags: N, Z
    checkForPageCross(address);
    reg.A = memory.get(address);
    setNZStatus(reg.A);
}

void CPU::CMP(uint16_t address) {
    //Compare Memory and Accumulator
    checkForPageCross(address);
    compareValues(reg.A, memory.get(address));
}

void CPU::SBC(uint16_t address) {
    //Subtract Memory from Accumulator with Borrow
    //Flags: N, V, Z, C
    checkForPageCross(address);
    uint8_t a = reg.A;
    reg.A = reg.A - memory.get(address) - (1 - reg.P.status.Carry);
    reg.P.status.Carry = ((int)a - (int)memory.get(address) - (int)(1 - reg.P.status.Carry)) >= 0x0;
    reg.P.status.Overflow = ((a ^ memory.get(address)) & 0x80) != 0 && ((a ^ reg.A) & 0x80) != 0;
    setNZStatus(reg.A);
}

// RMW
void CPU::ASL(uint16_t address) {
    //Arithmetic Shift Left One Bit
    //Flags: N, Z, C
    uint8_t value;
    if (currentAddressMode == Accumulator) {
        value = reg.A;
    } else {
        value = memory.get(address);
    }
    
    reg.P.status.Carry = (value >> 7) & 1;
    value <<= 1;
    
    if (currentAddressMode == Accumulator) {
        reg.A = value;
    }
    else {
        memory.set(address, value);
    }

    setNZStatus(value);
}

void CPU::DEC(uint16_t address) {
    //Decrement Memory by One
    //Flags: N, Z
    memory.set(address, memory.get(address) - 1);
    setNZStatus(memory.get(address));
}

void CPU::INC(uint16_t address) {
    //Increment Memory by One
    //Flags: N, Z
    memory.set(address, memory.get(address) + 1);
    setNZStatus(memory.get(address));
}

void CPU::LSR(uint16_t address) {
    //Logical Shift Right One Bit
    //Flags: N, Z, C
    uint8_t value;
    if (currentAddressMode == Accumulator) {
        value = reg.A;
    } else {
        value = memory.get(address);
    }
    
    reg.P.status.Carry = value & 0x1;
    value >>= 1;

    if (currentAddressMode == Accumulator) {
        reg.A = value;
    }
    else {
        memory.set(address, value);
    }

    setNZStatus(value);
}

void CPU::ROL(uint16_t address) {
    //Rotate Left One Bit
    //Flags: N, Z, C
    uint8_t value;
    if (currentAddressMode == Accumulator) {
        value = reg.A;
    } else {
        value = memory.get(address);
    }
    
    char c = reg.P.status.Carry;
    reg.P.status.Carry = (value >> 0x7) & 0x1;
    value = (value << 0x1) | c;

    if (currentAddressMode == Accumulator) {
        reg.A = value;
    }
    else {
        memory.set(address, value);
    }
    
    setNZStatus(value);
}

void CPU::ROR(uint16_t address) {
    //Rotate Right One Bit
    //Flags: N, Z, C
    uint8_t value;
    if (currentAddressMode == Accumulator) {
        value = reg.A;
    } else {
        value = memory.get(address);
    }
    
    uint8_t c = reg.P.status.Carry;
    reg.P.status.Carry = value & 0x1;
    value = (value >> 0x1) | (c << 0x7);

    if (currentAddressMode == Accumulator) {
        reg.A = value;
    } else {
        memory.set(address, value);
    }

    setNZStatus(value);
}

// Unimplemented
void CPU::AHX(uint16_t address) {}
void CPU::ALR(uint16_t address) {}
void CPU::ANC(uint16_t address) {}
void CPU::ARR(uint16_t address) {}
void CPU::AXS(uint16_t address) {}
void CPU::DCP(uint16_t address) {
    // DEC(address);
    // CMP(address);
}
void CPU::ISB(uint16_t address) {
//    INC(address);
//    SBC(address);
}
void CPU::LAS(uint16_t address) {
    checkForPageCross(address);
}
void CPU::LAX(uint16_t address) {
    checkForPageCross(address);
    reg.A = memory.get(address);
    setNZStatus(reg.A);
    reg.X = memory.get(address);
    setNZStatus(reg.X);
}
void CPU::RLA(uint16_t address) {
    // ROL(address);
    // AND(address);
}
void CPU::RRA(uint16_t address) {
    // ROR(address);
    // ADC(address);
}
void CPU::SLO(uint16_t address) {
    // ASL(address);
    // ORA(address);
}
void CPU::SAX(uint16_t address) {
    // memory.set(address, reg.A & reg.X);
}
void CPU::SHX(uint16_t address) {}
void CPU::SHY(uint16_t address) {}
void CPU::SRE(uint16_t address) {
    // LSR(address);
    // EOR(address);
}
void CPU::STP(uint16_t address) {}
void CPU::TAS(uint16_t address) {}
void CPU::XAA(uint16_t address) {}
