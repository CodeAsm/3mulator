#include <stdio.h>
#include <stdlib.h>
#include "main.hxx"


#ifndef VERSION
#define VERSION "1.1"//; //TODO: this isnt working if we forget to define version.
#endif


using SByte = char;		//1Byte,  8bits
using Byte = unsigned char;	//1Byte,  8bits
using Word = unsigned short;	//2Bytes, 16bits
using u32 = unsigned int;	//4Bytes, 32bits
using s32 = signed int;		//4Bytes, 32bits

// Memory of system
struct Mem {
		//CPU Memspace
		Byte mem[ 1024 * 64 ]; 	//65.536bytes
	};

// 6502 CPU 
// Require memory
struct Cpu {

		//CPU Registers
		Word PC;		//Program Counter
		Word SP;		//Stack Pointer
		Byte A,X,Y;		//Registers, Accumelator, X and Y.

		//CPU Flags
		Byte C : 1;	//0: Carry flag
		Byte Z : 1;	//1: Zero flag
		Byte I : 1;	//2: Interupt disable
		Byte D : 1;	//3: Decimal mode
		Byte B : 1;	//4: Break
		Byte Unused : 1;//5: Unused
		Byte V : 1;	//6: Overflow
		Byte N : 1;	//7: Negative

		//CPU state, this is usefull for cycle accuracy
		Byte State; //0 fetch, 1 execute, 2 store
					//during 2

	void Reset(Mem& mem){

		for (int i=0; i <= 1024;i++){
		mem.mem[i] = 0x00;
			}
		PC = 0xFFFC;	//and FFFD?!
		SP = 0x0100;	//Not accurate?

	}
};
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

//cpu for the cpu in use
//loc should be a hexvalue of the memory location to be seen
//status boolean if you want the registers to be printed or not
void PrintStats(Cpu cpu, Mem mem, int loc, bool status){
	//Hexdump view of memory
	//For now, only displays first 125 bytes
	int hexlength = 10;	//amount of bytes per line
	int hcounter = 0;	//horizontal counter
	for (int i=loc; i <=loc+129;i++){
		//show adress per line
		if(hcounter ==0)printf("0x%05X ", i);

		printf("%02X ", mem.mem[i]);
		hcounter++;

		//Ascii representation of previous hexvallues
		if(hcounter== hexlength){
			printf("|");
			hcounter++;
			for (int y=i-(hexlength -1); y <=i;y++){
				//only print visible characters tho.
				char printable = mem.mem[y];
				if(printable < ' ' || printable > '~'){
					printable = '.';
				}
				printf("%c", printable);
			}
		hcounter=0; printf("|\n"); //done
		}

	}

	if(status){
		//Display CPU stats, definitly needs rework.
		printf("\n");

		printf("PC:\t%#04X ", cpu.PC);
		printf("SP:\t%#04X ", cpu.SP);
		printf("\n");
		printf("X:\t%#02X ", cpu.X);
		printf("Y:\t%#02X ", cpu.Y);
		printf("\n");
		printf("Accumelator:\t%#02X ", cpu.A);
		printf("\n");
		printf("C:%d Z:%d I:%d D:%d B:%d V:%d N:%d \n",cpu.C, cpu.Z, cpu.I, cpu.D, cpu.B, cpu.V,  cpu.N );
		printf("\n");
	}
}

int main(){

	Cpu cpu;
	Mem mem;
	cpu.Reset(mem);
	mem.mem[0] = 0xa9;
	mem.mem[1] = 0xaa;
	mem.mem[2] = 0xe8;
	mem.mem[3] = 0x69;
	mem.mem[4] = 0xc4;
	mem.mem[5] = 0x4c;
	mem.mem[6] = 0x00;
	mem.mem[7] = 0x00;
	mem.mem[65532] = 0x4c;	//jmp to (0x0000)
	mem.mem[65533] = 0x00;	//adress lowbyte
	mem.mem[65534] = 0x00; 	//adress highbyte


	printf("\n\t6502 Emu and d3comp\n");
	printf( "\t-------------------\n");
    printf("\tV: %s\n\n", VERSION);

	int CycleAmount = 20;
	int Cycles = 0;
	while(1){
		char str;
		printf("Commands: C for continue, Q to quit, S 0x??? for show memory, R run for the remaining cycles\n");
		scanf(" %c", &str);
		switch (str) {
            case 'C':
	        case 'c':
	 	    		Cycles++;
					//do cpu step
					if (!CPUrun(&cpu, &mem, &Cycles)){	
						Cycles++;
					}else{
						printf("-- System halted --\n");
						printf("Try changing PC or alter code\n");
						Cycles = CycleAmount;
					}
				   //	PrintStats(cpu, mem, cpu.PC, true);
					printf("Cycle: %d \n", Cycles);
			    	break;
	        case 'S':
	        case 's':
					if(str)
	            	PrintStats(cpu, mem, 0x00, true);
					printf("Cycle: %d \n", Cycles);
		       	break;
	        case 'Q':
	        case 'q':
				Cycles = -1;
	      		break;
			case 'R':
			case 'r':
				CycleAmount = 200;
				while (Cycles < CycleAmount){
					if (!CPUrun(&cpu, &mem, &Cycles)){
						//Cycles++;
					}else{
						printf("-- System halted --\n");
						Cycles = CycleAmount;
					}
				}
	      		break;
	        default:
	            	break;
		}

		if(Cycles == -1){
			break;
		}
		str =0x0000; //strip away extra chars
	}


	return 0;
}
