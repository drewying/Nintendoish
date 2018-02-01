#pragma once
#include "NES.h"
#include "Memory.h"
#include <string>

using namespace std;

namespace NES {
    class Memory;
    class CPU {
    public:
        CPU(Memory &memory);
        ~CPU();
        
        struct Registers {
            unsigned char A; // Accumulator
            unsigned char X; // General
            unsigned char Y; // General
            unsigned short PC; // Program Counter
            unsigned char S; // Stack Pointer
            union {
                struct {
                    unsigned Carry:1,
                    Zero:1,
                    Interrupt:1,
                    Decimal:1,
                    T:1,
                    T2:1,
                    Overflow:1,
                    Negative:1;
                } status;
                unsigned char byte;
            } P;
        } reg;
        
        typedef enum AddressMode {
            Implicit       = 0,  //
            Immediate      = 1,  // #v
            Indirect       = 2,  // (a)
            Relative       = 3,  // label
            Accumulator    = 4,  // A
            ZeroPage       = 5,  // d
            ZeroPageIndexX = 6,  // d,x
            ZeroPageIndexY = 7,  // d,y
            Absolute       = 8,  // a
            AbsoluteIndexX = 9,  // a,x
            AbsoluteIndexY = 10, // a,y
            IndexIndirectX = 11, // (d,x)
            IndirectIndexY = 12  // (d),y
        } AddressMode;
        
        AddressMode currentAddressMode;
        
        string debugTable[256];
        unsigned int addressTable[256];
        unsigned int timingTable[256];
        void(NES::CPU::*opTable[256])(unsigned short);
        
        Memory& memory;
		
		unsigned int cycles = 0x0;
		unsigned int stallCycles = 0x0;
		bool requestNMI = false;

        int loadNextInstruction();
		int executeLoadedInstruction();

        void setNZStatus(unsigned char value);
        void oopsCycle(unsigned short address);
		void checkInterrurpts();
        void branchOnCondition(bool condition, unsigned short address);
        void compareValues(unsigned char a, unsigned char b);
        void push(unsigned char byte);
        void pushAddress(unsigned short address);
        unsigned char pull();
        unsigned short pullAddress();
        unsigned char readProgram();
        
        typedef void (CPU::*Op)(unsigned short);
        
        void NMI();
        
        // Control Flow
        void BCC(unsigned short address);
        void BCS(unsigned short address);
        void BEQ(unsigned short address);
        void BIT(unsigned short address);
        void BMI(unsigned short address);
        void BNE(unsigned short address);
        void BPL(unsigned short address);
        void BRK(unsigned short address);
        void BVC(unsigned short address);
        void BVS(unsigned short address);
        void CLC(unsigned short address);
        void CLD(unsigned short address);
        void CLI(unsigned short address);
        void CLV(unsigned short address);
        void CPX(unsigned short address);
        void CPY(unsigned short address);
        void DEX(unsigned short address);
        void DEY(unsigned short address);
        void INX(unsigned short address);
        void INY(unsigned short address);
        void LDX(unsigned short address);
        void LDY(unsigned short address);
        void JMP(unsigned short address);
        void JSR(unsigned short address);
        void NOP(unsigned short address);
        void PHA(unsigned short address);
        void PHP(unsigned short address);
        void PLA(unsigned short address);
        void PLP(unsigned short address);
        void RTI(unsigned short address);
        void RTS(unsigned short address);
        void SEC(unsigned short address);
        void SED(unsigned short address);
        void SEI(unsigned short address);
        void STX(unsigned short address);
        void STY(unsigned short address);
        void TAX(unsigned short address);
        void TAY(unsigned short address);
        void TSX(unsigned short address);
        void TYA(unsigned short address);
        void TXA(unsigned short address);
        void TXS(unsigned short address);
        
        // ALU
        void ORA(unsigned short address);
        void AND(unsigned short address);
        void EOR(unsigned short address);
        void ADC(unsigned short address);
        void STA(unsigned short address);
        void LDA(unsigned short address);
        void CMP(unsigned short address);
        void SBC(unsigned short address);
        
        // RMW
        void ASL(unsigned short address);
        void DEC(unsigned short address);
        void INC(unsigned short address);
        void LSR(unsigned short address);
        void ROL(unsigned short address);
        void ROR(unsigned short address);
        
        // Unimplemented
        void AHX(unsigned short address);
        void ALR(unsigned short address);
        void ANC(unsigned short address);
        void ARR(unsigned short address);
        void AXS(unsigned short address);
        void DCP(unsigned short address);
        void ISB(unsigned short address);
        void LAS(unsigned short address);
        void LAX(unsigned short address);
        void RLA(unsigned short address);
        void RRA(unsigned short address);
        void SLO(unsigned short address);
        void SAX(unsigned short address);
        void SHX(unsigned short address);
        void SHY(unsigned short address);
        void SRE(unsigned short address);
        void STP(unsigned short address);
        void TAS(unsigned short address);
        void XAA(unsigned short address);
    };
};
