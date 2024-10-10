// cores/cpu_6502.cpp
#include "6502.hxx"
#include "main.hxx"
#include <stdio.h>


void Reset(Mem& mem,Cpu& cpu){

		for (int i=0; i <= 1024;i++){
		mem.mem[i] = 0x00;
			}
		cpu.PC = 0xFFFC;	//and FFFD?!
		cpu.SP = 0x0100;	//Not accurate?

	}


//Current cpu
int CPUrun(Cpu *cpu, Mem *mem, int* cycle){
	//printf("%d \n", *cycle);
	switch (mem->mem[cpu->PC])
	{
	case 0xa9:
		printf("LDA #$%02X\n",mem->mem[(cpu->PC) +1] );
		cpu->A=mem->mem[(cpu->PC) +1];
		cpu->PC++;
		cpu->PC++;
		*cycle+=2;
		break;

	case 0x8d:
	printf("STA $%02X%02X\n",mem->mem[(cpu->PC) +2], mem->mem[(cpu->PC) +1]);
		cpu->PC++;
		cpu->PC++;
		cpu->PC++;
		*cycle+=4;
	break;

	case 0x4c:
	printf("JMP $%02X%02X\n",mem->mem[(cpu->PC) +2], mem->mem[(cpu->PC) +1]);
		cpu->PC = mem->mem[(cpu->PC) +2];
		*cycle+=3;
	break;

	case 0x69:
		printf("ADC $%02X\n",mem->mem[(cpu->PC) +1]);
		cpu->A=cpu->A+mem->mem[(cpu->PC) +1];
		cpu->PC++;
		cpu->PC++;
		*cycle+=3;
	break;

	case 0xaa:
	printf("TAX\n");
		cpu->X=cpu->A;
		cpu->PC++;
		*cycle+=2;
	break;

	case 0xe8:
		printf("INX\n");
		cpu->X++;
		cpu->PC++;
		*cycle+=2;
	break;

	case 0x00:
		printf("BRK\n");
		*cycle+=7;
	return 1;

	default:
		printf("Unkown instruction %#02x\n", mem->mem[cpu->PC]);
		break;
	}
	///printf("%x",cpu->PC);
	return 0;
}
