#include <stdio.h>
#include <stdlib.h>

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
void CPUrun(Cpu *cpu, Mem *mem, Byte step){
	
	if(mem->mem[cpu->PC] == 0xa9){
		printf("LDA #$%02X\n",mem->mem[(cpu->PC) +1] );
		cpu->PC++;
		}
	if(mem->mem[cpu->PC] == 0x8d){
		printf("STA $%02X%02X\n",mem->mem[(cpu->PC) +2], mem->mem[(cpu->PC) +1]);
		cpu->PC++;
		cpu->PC++;
		}
	cpu->PC++;
	///printf("%x",cpu->PC);
	return;
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
	mem.mem[0] = 0x42;
	mem.mem[1] = 0x43;
	mem.mem[2] = 's';
	mem.mem[3] = 0x45;
	mem.mem[4] = 0x46;
	mem.mem[5] = 0x47;
	mem.mem[6] = 0x48;
	mem.mem[7] = 0x49;

	mem.mem[10] = 0xa9;
	mem.mem[11] = 0x01;
	mem.mem[12] = 0x8d;
	mem.mem[13] = 0x00;
	mem.mem[14] = 0x02;
	mem.mem[15] = 0xa9;
	mem.mem[16] = 0x05;
	mem.mem[17] = 0x8d;
	mem.mem[18] = 0x01;
	mem.mem[19] = 0x02;

	printf("\n\t6502 Emu and d3comp\n");
	printf( "\t-------------------\n\n");

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
					CPUrun(&cpu, &mem, 1);
				   //	PrintStats(cpu, mem, cpu.PC, true);
					printf("Cycle: %d \n", Cycles);
			    	break;
	        case 'S':
	        case 's':
	            	PrintStats(cpu, mem, 0x00, true);
					printf("Cycle: %d \n", Cycles);
		       	break;
	        case 'Q':
	        case 'q':
				Cycles = CycleAmount;
	      		break;
			case 'R':
			case 'r':
				while (Cycles < CycleAmount){
					CPUrun(&cpu, &mem, 1);
					Cycles++;
				}
	      		break;
	        default:
	            	break;
		}

		if(Cycles >= CycleAmount){
			break;
		}
		str =0x0000; //strip away extra chars
	}


	return 0;
}
