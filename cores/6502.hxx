#ifndef CPU_6502_HXX
#define CPU_6502_HXX

#include <stdio.h>
#include "main.hxx"

// Memory structure
struct Mem {
    Byte mem[1024 * 64]; // 65,536 bytes
};

// CPU structure
struct Cpu {
    // CPU Registers
    Word PC;      // Program Counter
    Word SP;      // Stack Pointer
    Byte A, X, Y; // Accumulator, X and Y registers

    // CPU Flags
    Byte C : 1;    // Carry flag
    Byte Z : 1;    // Zero flag
    Byte I : 1;    // Interrupt disable
    Byte D : 1;    // Decimal mode
    Byte B : 1;    // Break command
    Byte Unused : 1; // Unused flag
    Byte V : 1;    // Overflow flag
    Byte N : 1;    // Negative flag

    // CPU State (for cycle accuracy)
    Byte State; // 0 fetch, 1 execute, 2 store
};

// Function declarations
void Reset(Mem& mem,Cpu& cpu);
int CPUrun(Cpu* cpu, Mem* mem, int* cycle);

#endif // CPU_6502_HXX
