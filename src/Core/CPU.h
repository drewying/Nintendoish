#pragma once

#ifdef __cplusplus

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
            uint8_t A; // Accumulator
            uint8_t X; // General
            uint8_t Y; // General
            uint16_t PC; // Program Counter
            uint8_t S; // Stack Pointer
            union {
                struct {
                    unsigned Carry:1,
                    Zero:1,
                    Interrupt:1,
                    Decimal:1,
                    Break:1,
                    Unused:1,
                    Overflow:1,
                    Negative:1;
                } status;
                uint8_t byte;
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
        uint32_t addressTable[256];
        uint32_t timingTable[256];
        void(NES::CPU::*opTable[256])(uint16_t);
        
        Memory& memory;
        
        uint32_t totalCycles = 0;
        uint32_t stallCycles = 0;
        bool requestNMI = false;
        bool requestIRQ = false;
        
        bool pageBoundryCross = false;
        
        unsigned int step();

        void setNZStatus(uint8_t value);
        void checkForPageCross(uint16_t a, uint16_t b);
        void oopsCycle(uint16_t address);
        void checkInterrurpts();
        void branchOnCondition(bool condition, uint16_t address);
        void compareValues(uint8_t a, uint8_t b);
        void push(uint8_t byte);
        void pushAddress(uint16_t address);
        uint8_t pull();
        uint16_t pullAddress();
        uint8_t readProgram();
        
        typedef void (CPU::*Op)(uint16_t);
        
        void NMI();
        void IRQ();
        
        // Control Flow
        void BCC(uint16_t address);
        void BCS(uint16_t address);
        void BEQ(uint16_t address);
        void BIT(uint16_t address);
        void BMI(uint16_t address);
        void BNE(uint16_t address);
        void BPL(uint16_t address);
        void BRK(uint16_t address);
        void BVC(uint16_t address);
        void BVS(uint16_t address);
        void CLC(uint16_t address);
        void CLD(uint16_t address);
        void CLI(uint16_t address);
        void CLV(uint16_t address);
        void CPX(uint16_t address);
        void CPY(uint16_t address);
        void DEX(uint16_t address);
        void DEY(uint16_t address);
        void INX(uint16_t address);
        void INY(uint16_t address);
        void LDX(uint16_t address);
        void LDY(uint16_t address);
        void JMP(uint16_t address);
        void JSR(uint16_t address);
        void NOP(uint16_t address);
        void PHA(uint16_t address);
        void PHP(uint16_t address);
        void PLA(uint16_t address);
        void PLP(uint16_t address);
        void RTI(uint16_t address);
        void RTS(uint16_t address);
        void SEC(uint16_t address);
        void SED(uint16_t address);
        void SEI(uint16_t address);
        void STX(uint16_t address);
        void STY(uint16_t address);
        void TAX(uint16_t address);
        void TAY(uint16_t address);
        void TSX(uint16_t address);
        void TYA(uint16_t address);
        void TXA(uint16_t address);
        void TXS(uint16_t address);
        
        // ALU
        void ORA(uint16_t address);
        void AND(uint16_t address);
        void EOR(uint16_t address);
        void ADC(uint16_t address);
        void STA(uint16_t address);
        void LDA(uint16_t address);
        void CMP(uint16_t address);
        void SBC(uint16_t address);
        
        // RMW
        void ASL(uint16_t address);
        void DEC(uint16_t address);
        void INC(uint16_t address);
        void LSR(uint16_t address);
        void ROL(uint16_t address);
        void ROR(uint16_t address);
        
        // Unimplemented
        void AHX(uint16_t address);
        void ALR(uint16_t address);
        void ANC(uint16_t address);
        void ARR(uint16_t address);
        void AXS(uint16_t address);
        void DCP(uint16_t address);
        void ISB(uint16_t address);
        void LAS(uint16_t address);
        void LAX(uint16_t address);
        void RLA(uint16_t address);
        void RRA(uint16_t address);
        void SLO(uint16_t address);
        void SAX(uint16_t address);
        void SHX(uint16_t address);
        void SHY(uint16_t address);
        void SRE(uint16_t address);
        void STP(uint16_t address);
        void TAS(uint16_t address);
        void XAA(uint16_t address);
    };
};

#endif
