#include "CPU.h"
#include "NES.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <stdio.h>

using namespace std;

NES::CPU::CPU(NES::Memory &memory):
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
        &NES::CPU::BRK, &NES::CPU::ORA, &NES::CPU::STP, &NES::CPU::SLO, &NES::CPU::NOP, &NES::CPU::ORA, &NES::CPU::ASL, &NES::CPU::SLO, &NES::CPU::PHP, &NES::CPU::ORA, &NES::CPU::ASL, &NES::CPU::ANC, &NES::CPU::NOP, &NES::CPU::ORA, &NES::CPU::ASL, &NES::CPU::SLO,
        &NES::CPU::BPL, &NES::CPU::ORA, &NES::CPU::STP, &NES::CPU::SLO, &NES::CPU::NOP, &NES::CPU::ORA, &NES::CPU::ASL, &NES::CPU::SLO, &NES::CPU::CLC, &NES::CPU::ORA, &NES::CPU::NOP, &NES::CPU::SLO, &NES::CPU::NOP, &NES::CPU::ORA, &NES::CPU::ASL, &NES::CPU::SLO,
        
        &NES::CPU::JSR, &NES::CPU::AND, &NES::CPU::STP, &NES::CPU::RLA, &NES::CPU::BIT, &NES::CPU::AND, &NES::CPU::ROL, &NES::CPU::RLA, &NES::CPU::PLP, &NES::CPU::AND, &NES::CPU::ROL, &NES::CPU::ANC, &NES::CPU::BIT, &NES::CPU::AND, &NES::CPU::ROL, &NES::CPU::RLA,
        &NES::CPU::BMI, &NES::CPU::AND, &NES::CPU::STP, &NES::CPU::RLA, &NES::CPU::NOP, &NES::CPU::AND, &NES::CPU::ROL, &NES::CPU::RLA, &NES::CPU::SEC, &NES::CPU::AND, &NES::CPU::NOP, &NES::CPU::RLA, &NES::CPU::NOP, &NES::CPU::AND, &NES::CPU::ROL, &NES::CPU::RLA,
        
        &NES::CPU::RTI, &NES::CPU::EOR, &NES::CPU::STP, &NES::CPU::SRE, &NES::CPU::NOP, &NES::CPU::EOR, &NES::CPU::LSR, &NES::CPU::SRE, &NES::CPU::PHA, &NES::CPU::EOR, &NES::CPU::LSR, &NES::CPU::ALR, &NES::CPU::JMP, &NES::CPU::EOR, &NES::CPU::LSR, &NES::CPU::SRE,
        &NES::CPU::BVC, &NES::CPU::EOR, &NES::CPU::STP, &NES::CPU::SRE, &NES::CPU::NOP, &NES::CPU::EOR, &NES::CPU::LSR, &NES::CPU::SRE, &NES::CPU::CLI, &NES::CPU::EOR, &NES::CPU::NOP, &NES::CPU::SRE, &NES::CPU::NOP, &NES::CPU::EOR, &NES::CPU::LSR, &NES::CPU::SRE,
        
        &NES::CPU::RTS, &NES::CPU::ADC, &NES::CPU::STP, &NES::CPU::RRA, &NES::CPU::NOP, &NES::CPU::ADC, &NES::CPU::ROR, &NES::CPU::RRA, &NES::CPU::PLA, &NES::CPU::ADC, &NES::CPU::ROR, &NES::CPU::ARR, &NES::CPU::JMP, &NES::CPU::ADC, &NES::CPU::ROR, &NES::CPU::RRA,
        &NES::CPU::BVS, &NES::CPU::ADC, &NES::CPU::STP, &NES::CPU::RRA, &NES::CPU::NOP, &NES::CPU::ADC, &NES::CPU::ROR, &NES::CPU::RRA, &NES::CPU::SEI, &NES::CPU::ADC, &NES::CPU::NOP, &NES::CPU::RRA, &NES::CPU::NOP, &NES::CPU::ADC, &NES::CPU::ROR, &NES::CPU::RRA,
        
        &NES::CPU::NOP, &NES::CPU::STA, &NES::CPU::NOP, &NES::CPU::SAX, &NES::CPU::STY, &NES::CPU::STA, &NES::CPU::STX, &NES::CPU::SAX, &NES::CPU::DEY, &NES::CPU::NOP, &NES::CPU::TXA, &NES::CPU::XAA, &NES::CPU::STY, &NES::CPU::STA, &NES::CPU::STX, &NES::CPU::SAX,
        &NES::CPU::BCC, &NES::CPU::STA, &NES::CPU::STP, &NES::CPU::AHX, &NES::CPU::STY, &NES::CPU::STA, &NES::CPU::STX, &NES::CPU::SAX, &NES::CPU::TYA, &NES::CPU::STA, &NES::CPU::TXS, &NES::CPU::TAS, &NES::CPU::SHY, &NES::CPU::STA, &NES::CPU::SHX, &NES::CPU::AHX,
        
        &NES::CPU::LDY, &NES::CPU::LDA, &NES::CPU::LDX, &NES::CPU::LAX, &NES::CPU::LDY, &NES::CPU::LDA, &NES::CPU::LDX, &NES::CPU::LAX, &NES::CPU::TAY, &NES::CPU::LDA, &NES::CPU::TAX, &NES::CPU::LAX, &NES::CPU::LDY, &NES::CPU::LDA, &NES::CPU::LDX, &NES::CPU::LAX,
        &NES::CPU::BCS, &NES::CPU::LDA, &NES::CPU::STP, &NES::CPU::LAX, &NES::CPU::LDY, &NES::CPU::LDA, &NES::CPU::LDX, &NES::CPU::LAX, &NES::CPU::CLV, &NES::CPU::LDA, &NES::CPU::TSX, &NES::CPU::LAS, &NES::CPU::LDY, &NES::CPU::LDA, &NES::CPU::LDX, &NES::CPU::LAX,
        
        &NES::CPU::CPY, &NES::CPU::CMP, &NES::CPU::NOP, &NES::CPU::DCP, &NES::CPU::CPY, &NES::CPU::CMP, &NES::CPU::DEC, &NES::CPU::DCP, &NES::CPU::INY, &NES::CPU::CMP, &NES::CPU::DEX, &NES::CPU::AXS, &NES::CPU::CPY, &NES::CPU::CMP, &NES::CPU::DEC, &NES::CPU::DCP,
        &NES::CPU::BNE, &NES::CPU::CMP, &NES::CPU::STP, &NES::CPU::DCP, &NES::CPU::NOP, &NES::CPU::CMP, &NES::CPU::DEC, &NES::CPU::DCP, &NES::CPU::CLD, &NES::CPU::CMP, &NES::CPU::NOP, &NES::CPU::DCP, &NES::CPU::NOP, &NES::CPU::CMP, &NES::CPU::DEC, &NES::CPU::DCP,
        
        &NES::CPU::CPX, &NES::CPU::SBC, &NES::CPU::NOP, &NES::CPU::ISB, &NES::CPU::CPX, &NES::CPU::SBC, &NES::CPU::INC, &NES::CPU::ISB, &NES::CPU::INX, &NES::CPU::SBC, &NES::CPU::NOP, &NES::CPU::SBC, &NES::CPU::CPX, &NES::CPU::SBC, &NES::CPU::INC, &NES::CPU::ISB,
        &NES::CPU::BEQ, &NES::CPU::SBC, &NES::CPU::STP, &NES::CPU::ISB, &NES::CPU::NOP, &NES::CPU::SBC, &NES::CPU::INC, &NES::CPU::ISB, &NES::CPU::SED, &NES::CPU::SBC, &NES::CPU::NOP, &NES::CPU::ISB, &NES::CPU::NOP, &NES::CPU::SBC, &NES::CPU::INC, &NES::CPU::ISB
    },
    timingTable {
        7, 6, 2, 8, 3, 3, 5, 5, 3, 2, 2, 2, 4, 4, 6, 6,
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

NES::CPU::~CPU()
{
}

unsigned char NES::CPU::readProgram() {
    return memory.get(reg.PC++);
}

int NES::CPU::executeLoadedInstruction() {
	/*int currentCycles = cycles;
	cycles += timingTable[loadedInstruction];
	(this->*opTable[loadedInstruction])(loadedAddress);
	loadedInstruction = 0x0;
	loadedAddress = 0x0;
	return cycles - currentCycles;*/
	return 0;
}

int NES::CPU::loadNextInstruction()
{
	int currentCycles = cycles;

    unsigned char hi = 0x0;
    unsigned char lo = 0x0;
	unsigned short loadedAddress = 0x0;
	unsigned char loadedInstruction = readProgram();
    
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
			loadedAddress = reg.PC + (signed char)lo;
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
            //address = hi << 8 | lo;
            //address = memory[(hi << 8 | lo)];
			loadedAddress = (unsigned short)(memory.get(hi << 8 | lo) + memory.get((hi << 8 | (unsigned char)(lo + 1))) * 256);
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
			loadedAddress = (unsigned short)(memory.get((lo + reg.X) % 256) + memory.get((lo + reg.X + 1) % 256) * 256);
            break;
        case IndirectIndexY:
            // The value in Y is added to the address at the little-endian address stored at the two-byte pair of the
            // specified address (LSB) and the specified address plus one (MSB). The value at the sum address is used
            // to perform the computation. Indeed addressing mode actually repeats exactly the accumulator register's digits.
            lo = readProgram();
			loadedAddress = (unsigned short)(memory.get(lo) + memory.get((lo + 1) % 256) * 256 + reg.Y);
            break;
        default:
            break;
    }
	cycles += timingTable[loadedInstruction];
	(this->*opTable[loadedInstruction])(loadedAddress);
	checkInterrurpts();
	return cycles - currentCycles;
}

void NES::CPU::setNZStatus(unsigned char value) {
    reg.P.status.Zero = (value == 0);
    reg.P.status.Negative = ((value & 0x80) != 0);
}

void NES::CPU::branchOnCondition(bool condition, unsigned short address) {
    if (condition) {
        cycles++;
        oopsCycle(address);
        reg.PC = address;
    }
}

void NES::CPU::oopsCycle(unsigned short address) {
    unsigned short indexAddress;
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
        cycles++;
    }
}

void NES::CPU::checkInterrurpts() {
	if (requestNMI == true) {
		NMI();
		requestNMI = false;
	}
}

void NES::CPU::compareValues(unsigned char a, unsigned char b) {
    setNZStatus(a - b);
    reg.P.status.Carry = (a >= b);
}

void NES::CPU::push(unsigned char byte) {
    memory.set(0x100 | reg.S--, byte);
}

unsigned char NES::CPU::pull() {
    return memory.get(0x100 | ++reg.S);
}

void NES::CPU::pushAddress(unsigned short address) {
    unsigned char lo = address & 0x00FF;
    unsigned char hi = address >> 8;
    push(hi);
    push(lo);
}

unsigned  short NES::CPU::pullAddress() {
    unsigned char lo = pull();
    unsigned char hi = pull();
    return hi << 8 | lo;
}

void NES::CPU::NMI() {
    // Non-Maskable Interrupt
    pushAddress(reg.PC);
    PHP(0x0);
	reg.PC = memory.get(0xFFFB) << 8 | memory.get(0xFFFA);
    reg.P.status.Interrupt = true;
    cycles += 7;
}

void NES::CPU::BCC(unsigned  short address) {
    //Branch on Carry Clear
    branchOnCondition(reg.P.status.Carry == 0, address);
}

void NES::CPU::BCS(unsigned  short address) {
    //Branch on Carry Set
    branchOnCondition(reg.P.status.Carry == 1, address);
}

void NES::CPU::BEQ(unsigned short address) {
    //Branch on Result Zero
    branchOnCondition(reg.P.status.Zero == 1, address);
}

void NES::CPU::BIT(unsigned short address) {
    //Test Bits in Memory with Accumulator
	char value = memory.get(address);
    reg.P.status.Overflow = (value >> 6) & 1;
    reg.P.status.Zero = (value & reg.A) == 0;
    reg.P.status.Negative = (value & 0x80) != 0;
}

void NES::CPU::BMI(unsigned short address) {
    //Branch on Result Minus
    branchOnCondition(reg.P.status.Negative == 1, address);
}

void NES::CPU::BNE(unsigned short address) {
    //Branch on Result not Zero
    branchOnCondition(reg.P.status.Zero == 0, address);
}

void NES::CPU::BPL(unsigned short address) {
    //Branch on Result Plus
    branchOnCondition(reg.P.status.Negative == 0, address);
}

void NES::CPU::BRK(unsigned short address) {
    //Force an Interrupt
    reg.P.status.Interrupt = 1;
}

void NES::CPU::BVC(unsigned short address) {
    //Branch on Overflow Clear
    branchOnCondition(reg.P.status.Overflow == 0, address);
}

void NES::CPU::BVS(unsigned short address) {
    //Branch on Overflow Set
    branchOnCondition(reg.P.status.Overflow == 1, address);
}

void NES::CPU::CLC(unsigned short address) {
    //Clear Carry Flag
    reg.P.status.Carry = 0;
}

void NES::CPU::CLD(unsigned short address) {
    //Clear Decimal Mode
    reg.P.status.Decimal = 0;
}

void NES::CPU::CLI(unsigned short address) {
    //Clear Interrupt Disable Status
    reg.P.status.Interrupt = 0;
}

void NES::CPU::CLV(unsigned short address) {
    //Clear Overflow Flag
    reg.P.status.Overflow = 0;
}

void NES::CPU::CPX(unsigned short address) {
    //Compare Memory and Index X
    compareValues(reg.X, memory.get(address));
}

void NES::CPU::CPY(unsigned short address) {
    //Compare Memory with Index Y
    compareValues(reg.Y, memory.get(address));
}

void NES::CPU::DEX(unsigned short address) {
    //Decrement Index X by One
    //Flags: N, Z
    reg.X -= 1;
    setNZStatus(reg.X);
}

void NES::CPU::DEY(unsigned short address) {
    //Decrement Index Y by One
    //Flags: N, Z
    reg.Y -= 1;
    setNZStatus(reg.Y);
}

void NES::CPU::INX(unsigned short address) {
    //Increment Index X by One
    //Flags: N, Z
    reg.X += 1;
    setNZStatus(reg.X);
}

void NES::CPU::INY(unsigned short address) {
    //Increment Index Y by One
    //Flags: N, Z
    reg.Y += 1;
    setNZStatus(reg.Y);
}

void NES::CPU::LDX(unsigned short address) {
    //Load Index X with Memory
    //Flags: N, Z
    reg.X = memory.get(address);
    setNZStatus(reg.X);
    oopsCycle(address);
}

void NES::CPU::LDY(unsigned short address) {
    //Load Index Y with Memory
    //Flags: N, Z
    reg.Y = memory.get(address);
    setNZStatus(reg.Y);
    oopsCycle(address);
}

void NES::CPU::JMP(unsigned short address) {
    //Jump to New Location
    reg.PC = address;
}

void NES::CPU::JSR(unsigned short address) {
    //Jump to New Location Saving Return Address
    pushAddress(reg.PC - 1);
    reg.PC = address;
}

void NES::CPU::NOP(unsigned short address) {
    //No Operation
    oopsCycle(address);
}

void NES::CPU::PHA(unsigned short address) {
    //Push Accumulator on Stack
    push(reg.A);
}

void NES::CPU::PHP(unsigned short address) {
    //Push Processor Status on Stack
    char s = reg.P.byte;
    s |= 0x30;
    push(s);
}

void NES::CPU::PLA(unsigned short address) {
    //Pull Accumulator from Stack
    //Flags: N, Z
    reg.A = pull();
    setNZStatus(reg.A);
}

void NES::CPU::PLP(unsigned short address) {
    //Pull Processor Status from Stack
    unsigned char s = pull();
    reg.P.status.Negative  = (s & 128) != 0x0;
    reg.P.status.Overflow  = (s & 64)  != 0x0;
    reg.P.status.Decimal   = (s & 8)   != 0x0;
    reg.P.status.Interrupt = (s & 4)   != 0x0;
    reg.P.status.Zero      = (s & 2)   != 0x0;
    reg.P.status.Carry     = (s & 1)   != 0x0;
}

void NES::CPU::RTI(unsigned short address) {
    //Return from interrupt
    //Flags: all
    reg.P.byte = ((pull() & 0xEF) | 0x20);
    reg.PC = pullAddress();
}

void NES::CPU::RTS(unsigned short address) {
    //Return from Subroutines
    reg.PC = pullAddress() + 1;
}

void NES::CPU::SEC(unsigned short address) {
    //Set Carry Flag
    reg.P.status.Carry = 1;
}

void NES::CPU::SED(unsigned short address) {
    //Set Decimal Mode
    reg.P.status.Decimal = 1;
}

void NES::CPU::SEI(unsigned short address) {
    //Set Interrupt Disable Status
    reg.P.status.Interrupt = 1;
}

void NES::CPU::STX(unsigned short address) {
    //Store Index X in Memory
    memory.set(address, reg.X);
}

void NES::CPU::STY(unsigned short address) {
    //Store Index Y in Memory
    memory.set(address, reg.Y);
}

void NES::CPU::TAX(unsigned short address) {
    //Transfer Accumulator to Index X
    //Flags: N, Z
    reg.X = reg.A;
    setNZStatus(reg.X);
}

void NES::CPU::TAY(unsigned short address) {
    //Transfer Accumulator to Index Y
    //Flags: N, Z
    reg.Y = reg.A;
    setNZStatus(reg.Y);
}

void NES::CPU::TSX(unsigned short address) {
    //Transfer Stack Pointer to Index X
    //Flags: N, Z
    reg.X = reg.S;
    setNZStatus(reg.X);
}

void NES::CPU::TYA(unsigned short address) {
    //Transfer Index Y to Accumulator
    //Flags: N, Z
    reg.A = reg.Y;
    setNZStatus(reg.A);
}

void NES::CPU::TXA(unsigned short address) {
    //Transfer Index X to Accumulator
    //Flags: N, Z
    reg.A = reg.X;
    setNZStatus(reg.A);
}

void NES::CPU::TXS(unsigned short address) {
    //Transfer Index X to Stack Pointer
    reg.S = reg.X;
}

// ALU
void NES::CPU::ORA(unsigned short address) {
    //OR Memory with Accumulator
    //Flags: N, Z
    reg.A = reg.A | memory.get(address);
    setNZStatus(reg.A);
}

void NES::CPU::AND(unsigned short address) {
    //AND Memory with Accumulator
    //Flags: N, Z
    reg.A = reg.A & memory.get(address);
    setNZStatus(reg.A);
}

void NES::CPU::EOR(unsigned short address) {
    //Exclusive-OR Memory with Accumulator
    //Flags: N, Z
    reg.A = reg.A ^ memory.get(address);
    setNZStatus(reg.A);
}

void NES::CPU::ADC(unsigned short address) {
    //Add Memory to Accumulator with Carry
    //Flags: N, V, Z, C
    unsigned char a = reg.A;
    reg.A = reg.A + memory.get(address) + reg.P.status.Carry;
    
    reg.P.status.Carry = ((int)a + (int)memory.get(address) + (int)reg.P.status.Carry) > 0xFF;
    reg.P.status.Overflow = ((a ^ memory.get(address)) & 0x80) == 0 &&
    ((a ^ reg.A) & 0x80) != 0;
    
    setNZStatus(reg.A);
}

void NES::CPU::STA(unsigned short address) {
    //Store Accumulator in Memory
    memory.set(address, reg.A);
}

void NES::CPU::LDA(unsigned short address) {
    //Load Accumulator with Memory
    //Flags: N, Z
    reg.A = memory.get(address);
    setNZStatus(reg.A);
    oopsCycle(address);
}

void NES::CPU::CMP(unsigned short address) {
    //Compare Memory and Accumulator
    compareValues(reg.A, memory.get(address));
}

void NES::CPU::SBC(unsigned short address) {
    //Subtract Memory from Accumulator with Borrow
    //Flags: N, V, Z, C
    
    unsigned char a = reg.A;
    reg.A = reg.A - memory.get(address) - (1 - reg.P.status.Carry);
    
    reg.P.status.Carry = ((int)a - (int)memory.get(address) - (int)(1 - reg.P.status.Carry)) >= 0x0;
    reg.P.status.Overflow = ((a ^ memory.get(address)) & 0x80) != 0 &&
    ((a ^ reg.A) & 0x80) != 0;
    
    setNZStatus(reg.A);
}

// RMW
void NES::CPU::ASL(unsigned short address) {
    //Arithmetic Shift Left One Bit
    //Flags: N, Z, C
    unsigned char value;
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

void NES::CPU::DEC(unsigned short address) {
    //Decrement Memory by One
    //Flags: N, Z
    memory.set(address, memory.get(address) - 1);
    setNZStatus(memory.get(address));
}

void NES::CPU::INC(unsigned short address) {
    //Increment Memory by One
    //Flags: N, Z
	memory.set(address, memory.get(address) + 1);
	setNZStatus(memory.get(address));
}

void NES::CPU::LSR(unsigned short address) {
    //Logical Shift Right One Bit
    //Flags: N, Z, C
    unsigned char value;
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

void NES::CPU::ROL(unsigned short address) {
    //Rotate Left One Bit
    //Flags: N, Z, C
    unsigned char value;
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

void NES::CPU::ROR(unsigned short address) {
    //Rotate Right One Bit
    //Flags: N, Z, C
    unsigned char value;
    if (currentAddressMode == Accumulator) {
        value = reg.A;
    } else {
        value = memory.get(address);
    }
    
    char c = reg.P.status.Carry;
    reg.P.status.Carry = value & 0x1;
    value = (value >> 0x1) | (c << 0x7);

	if (currentAddressMode == Accumulator) {
		reg.A = value;
	}
	else {
		memory.set(address, value);
	}

    setNZStatus(value);
}

// Unimplemented
void NES::CPU::AHX(unsigned short address) {}
void NES::CPU::ALR(unsigned short address) {}
void NES::CPU::ANC(unsigned short address) {}
void NES::CPU::ARR(unsigned short address) {}
void NES::CPU::AXS(unsigned short address) {}
void NES::CPU::DCP(unsigned short address) {
    DEC(address);
    CMP(address);
}
void NES::CPU::ISB(unsigned short address) {
    INC(address);
    SBC(address);
}
void NES::CPU::LAS(unsigned short address) {}
void NES::CPU::LAX(unsigned short address) {
    reg.A = memory.get(address);
    setNZStatus(reg.A);
    reg.X = memory.get(address);
    setNZStatus(reg.X);
    oopsCycle(address);
}
void NES::CPU::RLA(unsigned short address) {
    ROL(address);
    AND(address);
}
void NES::CPU::RRA(unsigned short address) {
    ROR(address);
    ADC(address);
}
void NES::CPU::SLO(unsigned short address) {
    ASL(address);
    ORA(address);
}
void NES::CPU::SAX(unsigned short address) {
    memory.set(address, reg.A & reg.X);
}
void NES::CPU::SHX(unsigned short address) {}
void NES::CPU::SHY(unsigned short address) {}
void NES::CPU::SRE(unsigned short address) {
    LSR(address);
    EOR(address);
}
void NES::CPU::STP(unsigned short address) {}
void NES::CPU::TAS(unsigned short address) {}
void NES::CPU::XAA(unsigned short address) {}
