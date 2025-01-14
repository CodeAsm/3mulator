/**
 * @file 8080.h
 * @author 
 * @version 0.2
 * @date 2024-10-04
 * 
 * @brief Header file for the 8080 CPU emulator.
 */

#ifndef _8080_H_
#define _8080_H_

#include <stdint.h>

// Define the size of the memory
#define MEMORY_SIZE 65536

// Declare the memory array
extern uint8_t memory[MEMORY_SIZE];

// Declare the registers
extern uint8_t registers[7];
extern uint16_t pc, sp;

// Declare the flags
extern uint8_t flags;

// Function declarations
void initialize_8080();
void load_program(const char *filename);
void execute_instruction();
void handle_interrupt(uint8_t interrupt_vector);

#endif // _8080_H_