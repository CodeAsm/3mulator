#include <stdio.h>
#include <stdlib.h>
#include "main.hxx"
#include "cores/6502.hxx"


#ifndef VERSION
#define VERSION "1.1"//; //TODO: this isnt working if we forget to define version.
#endif


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
	Reset(mem, cpu);
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
