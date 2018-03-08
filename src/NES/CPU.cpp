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

CPU::~CPU()
{
}

unsigned char CPU::readProgram() {
    return memory.get(reg.PC++);
}

unsigned int CPU::step()
{
	if (stallCycles > 0x0) {
		stallCycles--;
		return 1;
	}

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

void CPU::setNZStatus(unsigned char value) {
    reg.P.status.Zero = (value == 0x0);
    reg.P.status.Negative = ((value & 0x80) != 0);
}

void CPU::branchOnCondition(bool condition, unsigned short address) {
    if (condition) {
        cycles++;
        oopsCycle(address);
        reg.PC = address;
    }
}

void CPU::oopsCycle(unsigned short address) {
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

void CPU::checkInterrurpts() {
	if (requestNMI == true) {
		NMI();
		requestNMI = false;
	}
}

void CPU::compareValues(unsigned char a, unsigned char b) {
    setNZStatus(a - b);
    reg.P.status.Carry = (a >= b);
}

void CPU::push(unsigned char byte) {
    memory.set(0x100 | reg.S--, byte);
}

unsigned char CPU::pull() {
    return memory.get(0x100 | ++reg.S);
}

void CPU::pushAddress(unsigned short address) {
    unsigned char lo = address & 0x00FF;
    unsigned char hi = address >> 8;
    push(hi);
    push(lo);
}

unsigned short CPU::pullAddress() {
    unsigned char lo = pull();
    unsigned char hi = pull();
    return hi << 8 | lo;
}

void CPU::NMI() {
    // Non-Maskable Interrupt
    pushAddress(reg.PC);
    PHP(0x0);
	reg.PC = memory.get(0xFFFB) << 8 | memory.get(0xFFFA);
    reg.P.status.Interrupt = true;
    cycles += 7;
}

void CPU::BCC(unsigned  short address) {
    //Branch on Carry Clear
    branchOnCondition(reg.P.status.Carry == 0, address);
}

void CPU::BCS(unsigned  short address) {
    //Branch on Carry Set
    branchOnCondition(reg.P.status.Carry == 1, address);
}

void CPU::BEQ(unsigned short address) {
    //Branch on Result Zero
    branchOnCondition(reg.P.status.Zero == 1, address);
}

void CPU::BIT(unsigned short address) {
    //Test Bits in Memory with Accumulator
	char value = memory.get(address);
    reg.P.status.Overflow = (value >> 6) & 1;
    reg.P.status.Zero = (value & reg.A) == 0;
    reg.P.status.Negative = (value & 0x80) != 0;
}

void CPU::BMI(unsigned short address) {
    //Branch on Result Minus
    branchOnCondition(reg.P.status.Negative == 1, address);
}

void CPU::BNE(unsigned short address) {
    //Branch on Result not Zero
    branchOnCondition(reg.P.status.Zero == 0, address);
}

void CPU::BPL(unsigned short address) {
    //Branch on Result Plus
    branchOnCondition(reg.P.status.Negative == 0, address);
}

void CPU::BRK(unsigned short address) {
    //Force an Interrupt
    reg.P.status.Interrupt = 1;
}

void CPU::BVC(unsigned short address) {
    //Branch on Overflow Clear
    branchOnCondition(reg.P.status.Overflow == 0, address);
}

void CPU::BVS(unsigned short address) {
    //Branch on Overflow Set
    branchOnCondition(reg.P.status.Overflow == 1, address);
}

void CPU::CLC(unsigned short address) {
    //Clear Carry Flag
    reg.P.status.Carry = 0;
}

void CPU::CLD(unsigned short address) {
    //Clear Decimal Mode
    reg.P.status.Decimal = 0;
}

void CPU::CLI(unsigned short address) {
    //Clear Interrupt Disable Status
    reg.P.status.Interrupt = 0;
}

void CPU::CLV(unsigned short address) {
    //Clear Overflow Flag
    reg.P.status.Overflow = 0;
}

void CPU::CPX(unsigned short address) {
    //Compare Memory and Index X
    compareValues(reg.X, memory.get(address));
}

void CPU::CPY(unsigned short address) {
    //Compare Memory with Index Y
    compareValues(reg.Y, memory.get(address));
}

void CPU::DEX(unsigned short address) {
    //Decrement Index X by One
    //Flags: N, Z
    reg.X -= 1;
    setNZStatus(reg.X);
}

void CPU::DEY(unsigned short address) {
    //Decrement Index Y by One
    //Flags: N, Z
    reg.Y -= 1;
    setNZStatus(reg.Y);
}

void CPU::INX(unsigned short address) {
    //Increment Index X by One
    //Flags: N, Z
    reg.X += 1;
    setNZStatus(reg.X);
}

void CPU::INY(unsigned short address) {
    //Increment Index Y by One
    //Flags: N, Z
    reg.Y += 1;
    setNZStatus(reg.Y);
}

void CPU::LDX(unsigned short address) {
    //Load Index X with Memory
    //Flags: N, Z
    reg.X = memory.get(address);
    setNZStatus(reg.X);
    oopsCycle(address);
}

void CPU::LDY(unsigned short address) {
    //Load Index Y with Memory
    //Flags: N, Z
    reg.Y = memory.get(address);
    setNZStatus(reg.Y);
    oopsCycle(address);
}

void CPU::JMP(unsigned short address) {
    //Jump to New Location
    reg.PC = address;
}

void CPU::JSR(unsigned short address) {
    //Jump to New Location Saving Return Address
    pushAddress(reg.PC - 1);
    reg.PC = address;
}

void CPU::NOP(unsigned short address) {
    //No Operation
    oopsCycle(address);
}

void CPU::PHA(unsigned short address) {
    //Push Accumulator on Stack
    push(reg.A);
}

void CPU::PHP(unsigned short address) {
    //Push Processor Status on Stack
    char s = reg.P.byte;
    s |= 0x30;
    push(s);
}

void CPU::PLA(unsigned short address) {
    //Pull Accumulator from Stack
    //Flags: N, Z
    reg.A = pull();
    setNZStatus(reg.A);
}

void CPU::PLP(unsigned short address) {
    //Pull Processor Status from Stack
    unsigned char s = pull();
    reg.P.status.Negative  = (s & 128) != 0x0;
    reg.P.status.Overflow  = (s & 64)  != 0x0;
    reg.P.status.Decimal   = (s & 8)   != 0x0;
    reg.P.status.Interrupt = (s & 4)   != 0x0;
    reg.P.status.Zero      = (s & 2)   != 0x0;
    reg.P.status.Carry     = (s & 1)   != 0x0;
}

void CPU::RTI(unsigned short address) {
    //Return from interrupt
    //Flags: all
    reg.P.byte = ((pull() & 0xEF) | 0x20);
    reg.PC = pullAddress();
}

void CPU::RTS(unsigned short address) {
    //Return from Subroutines
    reg.PC = pullAddress() + 1;
}

void CPU::SEC(unsigned short address) {
    //Set Carry Flag
    reg.P.status.Carry = 1;
}

void CPU::SED(unsigned short address) {
    //Set Decimal Mode
    reg.P.status.Decimal = 1;
}

void CPU::SEI(unsigned short address) {
    //Set Interrupt Disable Status
    reg.P.status.Interrupt = 1;
}

void CPU::STX(unsigned short address) {
    //Store Index X in Memory
    memory.set(address, reg.X);
}

void CPU::STY(unsigned short address) {
    //Store Index Y in Memory
    memory.set(address, reg.Y);
}

void CPU::TAX(unsigned short address) {
    //Transfer Accumulator to Index X
    //Flags: N, Z
    reg.X = reg.A;
    setNZStatus(reg.X);
}

void CPU::TAY(unsigned short address) {
    //Transfer Accumulator to Index Y
    //Flags: N, Z
    reg.Y = reg.A;
    setNZStatus(reg.Y);
}

void CPU::TSX(unsigned short address) {
    //Transfer Stack Pointer to Index X
    //Flags: N, Z
    reg.X = reg.S;
    setNZStatus(reg.X);
}

void CPU::TYA(unsigned short address) {
    //Transfer Index Y to Accumulator
    //Flags: N, Z
    reg.A = reg.Y;
    setNZStatus(reg.A);
}

void CPU::TXA(unsigned short address) {
    //Transfer Index X to Accumulator
    //Flags: N, Z
    reg.A = reg.X;
    setNZStatus(reg.A);
}

void CPU::TXS(unsigned short address) {
    //Transfer Index X to Stack Pointer
    reg.S = reg.X;
}

// ALU
void CPU::ORA(unsigned short address) {
    //OR Memory with Accumulator
    //Flags: N, Z
    reg.A = reg.A | memory.get(address);
    setNZStatus(reg.A);
}

void CPU::AND(unsigned short address) {
    //AND Memory with Accumulator
    //Flags: N, Z
    reg.A = reg.A & memory.get(address);
    setNZStatus(reg.A);
}

void CPU::EOR(unsigned short address) {
    //Exclusive-OR Memory with Accumulator
    //Flags: N, Z
    reg.A = reg.A ^ memory.get(address);
    setNZStatus(reg.A);
}

void CPU::ADC(unsigned short address) {
    //Add Memory to Accumulator with Carry
    //Flags: N, V, Z, C
    unsigned char a = reg.A;
    reg.A = reg.A + memory.get(address) + reg.P.status.Carry;
    
    reg.P.status.Carry = ((int)a + (int)memory.get(address) + (int)reg.P.status.Carry) > 0xFF;
    reg.P.status.Overflow = ((a ^ memory.get(address)) & 0x80) == 0 &&
    ((a ^ reg.A) & 0x80) != 0;
    
    setNZStatus(reg.A);
}

void CPU::STA(unsigned short address) {
    //Store Accumulator in Memory
    memory.set(address, reg.A);
}

void CPU::LDA(unsigned short address) {
    //Load Accumulator with Memory
    //Flags: N, Z
    reg.A = memory.get(address);
    setNZStatus(reg.A);
    oopsCycle(address);
}

void CPU::CMP(unsigned short address) {
    //Compare Memory and Accumulator
    compareValues(reg.A, memory.get(address));
}

void CPU::SBC(unsigned short address) {
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
void CPU::ASL(unsigned short address) {
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

void CPU::DEC(unsigned short address) {
    //Decrement Memory by One
    //Flags: N, Z
    memory.set(address, memory.get(address) - 1);
    setNZStatus(memory.get(address));
}

void CPU::INC(unsigned short address) {
    //Increment Memory by One
    //Flags: N, Z
	memory.set(address, memory.get(address) + 1);
	setNZStatus(memory.get(address));
}

void CPU::LSR(unsigned short address) {
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

void CPU::ROL(unsigned short address) {
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

void CPU::ROR(unsigned short address) {
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
void CPU::AHX(unsigned short address) {}
void CPU::ALR(unsigned short address) {}
void CPU::ANC(unsigned short address) {}
void CPU::ARR(unsigned short address) {}
void CPU::AXS(unsigned short address) {}
void CPU::DCP(unsigned short address) {
    DEC(address);
    CMP(address);
}
void CPU::ISB(unsigned short address) {
    INC(address);
    SBC(address);
}
void CPU::LAS(unsigned short address) {}
void CPU::LAX(unsigned short address) {
    reg.A = memory.get(address);
    setNZStatus(reg.A);
    reg.X = memory.get(address);
    setNZStatus(reg.X);
    oopsCycle(address);
}
void CPU::RLA(unsigned short address) {
    ROL(address);
    AND(address);
}
void CPU::RRA(unsigned short address) {
    ROR(address);
    ADC(address);
}
void CPU::SLO(unsigned short address) {
    ASL(address);
    ORA(address);
}
void CPU::SAX(unsigned short address) {
    memory.set(address, reg.A & reg.X);
}
void CPU::SHX(unsigned short address) {}
void CPU::SHY(unsigned short address) {}
void CPU::SRE(unsigned short address) {
    LSR(address);
    EOR(address);
}
void CPU::STP(unsigned short address) {}
void CPU::TAS(unsigned short address) {}
void CPU::XAA(unsigned short address) {}
