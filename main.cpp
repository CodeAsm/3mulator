#include <stdio.h>
#include <stdlib.h>
#include "main.hxx"
#include "cores/6502.hxx"
#include "ui/window.hxx"

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

void checkAddressSpace(uint16_t address, uint8_t* memory, gui* guii) {
	int scale = 3;
	int charWidth = 5 + 1; // 5 pixels wide + 1 pixel space
	int charHeight = 9 + 1; // 9 pixels high + 1 pixel space
	int windowWidth = 40 * charWidth * scale;
	int windowHeight = 24 * charHeight * scale;
	// Calculate the scale factor
	int scaleX = windowWidth / (40 * 6); // 40 characters per line, each character is 5 pixels wide + 1 pixel space
	int scaleY = windowHeight / (24 * 10); // 24 lines, each character is 9 pixels high + 1 pixel space
	//int scale = std::min(scaleX, scaleY);

	const uint16_t startAddress = 0x8000; // Example start address
	const uint16_t endAddress = 0x87FF;   // Example end address

	if (address >= startAddress && address <= endAddress) {
		// Call render function or handle memory changes
		for (int j = 0; j < 24; ++j) { // 24 lines of characters
			for (int i = 0; i < 40; ++i) { // 40 characters per line
				guii->drawCharacter(memory[address + i + j * 40], i * 6 * scale, j * 10 * scale, scale); // Each character is 5 pixels wide + 1 pixel space, 9 pixels high + 1 pixel space
			}
		}
	}
	//SDL_RenderPresent(guii->renderer);
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
	
	/// gui
	int scale = 3;
    int charWidth = 5 + 1; // 5 pixels wide + 1 pixel space
    int charHeight = 9 + 1; // 9 pixels high + 1 pixel space
    int windowWidth = 40 * charWidth * scale;
    int windowHeight = 24 * charHeight * scale;

    gui gui(windowWidth, windowHeight, scale);
 // Example memory and address
    uint8_t memory[65536] = {0};
    uint16_t address = 0x8000;

	memory[0x8000] = 0xa9;
    memory[0x8001] = 0xaa;
    memory[0x8002] = 0xe8;
    memory[0x8003] = 0x69;
    memory[0x8004] = 0xc4;
    memory[0x8005] = 0x4c;
    memory[0x8006] = 0x00;
    memory[0x8007] = 0x00;
    memory[0x8008] = 0x4c;  // jmp to (0x0000)
    memory[0x8009] = 0x00;  // address low byte
    memory[0x800a] = 0x00;  // address high byte
    // Main loop
    bool running = true;
    SDL_Event event;
    while (running) {
         while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            } else if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_q) {
                    running = false;
                }
            }
        }
		

        gui.clear();

        // Example drawing
        //gui.drawCharacter(memory[address], 10, 10, scale);
		checkAddressSpace(0x8000, memory, &gui);

        gui.present();
        SDL_Delay(100); // Delay to limit frame rate
    }

    return 0;
	/// gui

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
