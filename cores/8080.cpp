/**
 * @file 8080.c
 * @author codeasm, Nico Vijlbrief (nico.vijlbrief@outlook.com)
 * @version 0.2
 * @date 2024-10-04
 * 
 * @copyright Copyright (c) 2024 CodeAsm(Nico Vijlbrief)
 * This file contains a very basic 8080 cpu emulator.
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <string.h>
#include <signal.h>
#include <sys/select.h>
#include "8080.hxx"


// BIOS will sit at the beginning of memory
// Common reset vector is at address 0x0000
// Common IRQ vectors are at addresses 0x0008, 0x0010, 0x0018, and 0x0020
#define MEMORY_SIZE 65536
uint8_t memory[MEMORY_SIZE];

class i8080 {
public:
    static const int IO_PORTS = 256;

    i8080() : pc(0), sp(0), flags(0) {
        memset(memory, 0, MEMORY_SIZE);
        memset(registers, 0, sizeof(registers));
        pc = sp = 0;
        flags = 0;
        memset(io_ports, 0, IO_PORTS);
    }

    void load_memory(uint8_t* mem, size_t size) {
        if (size > MEMORY_SIZE) size = MEMORY_SIZE;
        memcpy(memory, mem, size);
    }

    void write_port(uint8_t port, uint8_t value) {
        if (port < IO_PORTS) {
            io_ports[port] = value;
        }
    }

    // Other methods for CPU operations can be added here

// registers[0] - B
// registers[1] - C
// registers[2] - D
// registers[3] - E
// registers[4] - H
// registers[5] - L
// registers[6] - A (Accumulator)
uint8_t registers[7];
uint16_t pc, sp;

// flags[0] - Carry
// flags[1] - Parity
// flags[2] - Auxiliary Carry
// flags[3] - Zero
// flags[4] - Sign
// flags[5] - Unused
// flags[6] - Unused
// flags[7] - Unused
uint8_t flags;

#define IO_PORTS 256
uint8_t io_ports[IO_PORTS];

void write_port(uint8_t port, uint8_t value) {
    if (port < IO_PORTS) {
        io_ports[port] = value;
    } else {
        fprintf(stderr, "Invalid port number: %02x\n", port);
    }
}

uint8_t read_port(uint8_t port) {
    if (port < IO_PORTS) {
        return io_ports[port];
    } else {
        fprintf(stderr, "Invalid port number: %02x\n", port);
        return 0xFF; // Return a default value for invalid port
    }
}

// Function to set a specific flag
void set_flag(uint8_t flag) {
    flags |= (1 << flag);
}

// Function to clear a specific flag
void clear_flag(uint8_t flag) {
    flags &= ~(1 << flag);
}

// Function to check if a specific flag is set
uint8_t is_flag_set(uint8_t flag) {
    return (flags & (1 << flag)) != 0;
}

// Function to reset the CPU state
void reset_cpu() {
    memset(registers, 0, sizeof(registers));
    pc = 0;
    sp = MEMORY_SIZE - 1;
    flags = 0;
    set_flag(7); // Set the 7th flag to enable interrupts
    memset(memory, 0, sizeof(memory));
}
struct termios orig_termios;

void reset_terminal_mode() {
    tcsetattr(0, TCSANOW, &orig_termios);
}
void handle_signal(int signal) {
    reset_terminal_mode();
    exit(1);
}

void set_conio_terminal_mode() {
    struct termios new_termios;

    /* take two copies - one for now, one for later */
    tcgetattr(0, &orig_termios);
    memcpy(&new_termios, &new_termios, sizeof(new_termios));

    /* register cleanup handler, and set the new terminal mode */
  //  atexit(reset_terminal_mode);
  //  signal(SIGINT, handle_signal); // Catch Ctrl-C
    cfmakeraw(&new_termios);
    tcsetattr(0, TCSANOW, &new_termios);
}


void print_debug_info(uint8_t opcode) {
    const char *opcode_names[] = {
        "NOP", "LXI B,word", "STAX B", "INX B", "INR B", "DCR B", "MVI B,byte", "RLC", "NOP", "DAD B", "LDAX B", "DCX B", "INR C", "DCR C", "MVI C,byte", "RRC",
        "NOP", "LXI D,word", "STAX D", "INX D", "INR D", "DCR D", "MVI D,byte", "RAL", "NOP", "DAD D", "LDAX D", "DCX D", "INR E", "DCR E", "MVI E,byte", "RAR",
        "NOP", "LXI H,word", "SHLD addr", "INX H", "INR H", "DCR H", "MVI H,byte", "DAA", "NOP", "DAD H", "LHLD addr", "DCX H", "INR L", "DCR L", "MVI L,byte", "CMA",
        "NOP", "LXI SP,word", "STA addr", "INX SP", "INR M", "DCR M", "MVI M,byte", "STC", "NOP", "DAD SP", "LDA addr", "DCX SP", "INR A", "DCR A", "MVI A,byte", "CMC",
        "MOV B,B", "MOV B,C", "MOV B,D", "MOV B,E", "MOV B,H", "MOV B,L", "MOV B,M", "MOV B,A", "MOV C,B", "MOV C,C", "MOV C,D", "MOV C,E", "MOV C,H", "MOV C,L", "MOV C,M", "MOV C,A",
        "MOV D,B", "MOV D,C", "MOV D,D", "MOV D,E", "MOV D,H", "MOV D,L", "MOV D,M", "MOV D,A", "MOV E,B", "MOV E,C", "MOV E,D", "MOV E,E", "MOV E,H", "MOV E,L", "MOV E,M", "MOV E,A",
        "MOV H,B", "MOV H,C", "MOV H,D", "MOV H,E", "MOV H,H", "MOV H,L", "MOV H,M", "MOV H,A", "MOV L,B", "MOV L,C", "MOV L,D", "MOV L,E", "MOV L,H", "MOV L,L", "MOV L,M", "MOV L,A",
        "MOV M,B", "MOV M,C", "MOV M,D", "MOV M,E", "MOV M,H", "MOV M,L", "HLT", "MOV M,A", "MOV A,B", "MOV A,C", "MOV A,D", "MOV A,E", "MOV A,H", "MOV A,L", "MOV A,M", "MOV A,A",
        "ADD B", "ADD C", "ADD D", "ADD E", "ADD H", "ADD L", "ADD M", "ADD A", "ADC B", "ADC C", "ADC D", "ADC E", "ADC H", "ADC L", "ADC M", "ADC A",
        "SUB B", "SUB C", "SUB D", "SUB E", "SUB H", "SUB L", "SUB M", "SUB A", "SBB B", "SBB C", "SBB D", "SBB E", "SBB H", "SBB L", "SBB M", "SBB A",
        "ANA B", "ANA C", "ANA D", "ANA E", "ANA H", "ANA L", "ANA M", "ANA A", "XRA B", "XRA C", "XRA D", "XRA E", "XRA H", "XRA L", "XRA M", "XRA A",
        "ORA B", "ORA C", "ORA D", "ORA E", "ORA H", "ORA L", "ORA M", "ORA A", "CMP B", "CMP C", "CMP D", "CMP E", "CMP H", "CMP L", "CMP M", "CMP A",
        "RNZ", "POP B", "JNZ addr", "JMP addr", "CNZ addr", "PUSH B", "ADI byte", "RST 0", "RZ", "RET", "JZ addr", "JMP addr", "CZ addr", "CALL addr", "ACI byte", "RST 1",
        "RNC", "POP D", "JNC addr", "OUT port", "CNC addr", "PUSH D", "SUI byte", "RST 2", "RC", "RET", "JC addr", "IN port", "CC addr", "CALL addr", "SBI byte", "RST 3",
        "RPO", "POP H", "JPO addr", "XTHL", "CPO addr", "PUSH H", "ANI byte", "RST 4", "RPE", "PCHL", "JPE addr", "XCHG", "CPE addr", "CALL addr", "XRI byte", "RST 5",
        "RP", "POP PSW", "JP addr", "DI", "CP addr", "PUSH PSW", "ORI byte", "RST 6", "RM", "SPHL", "JM addr", "EI", "CM addr", "CALL addr", "CPI byte", "RST 7"
    };

    printf("Opcode: %02x %s\n\r", opcode, opcode_names[opcode]);
    printf("Registers: A=%02x B=%02x C=%02x D=%02x E=%02x H=%02x L=%02x\n\r",
           registers[2], registers[0], registers[1], registers[3], registers[4], registers[5], registers[6]);
    printf("PC: %04x SP: %04x ", pc, sp);
    printf("Memory[PC]: %02x %02x %02x\n\r\n\r", memory[pc], memory[pc + 1], memory[pc + 2]);
}

int kbhit() {
    struct timeval tv = { 0L, 0L };
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(0, &fds);
    return select(1, &fds, NULL, NULL, &tv);
}

int getch() {
    int r;
    unsigned char c;
    if ((r = read(0, &c, sizeof(c))) < 0) {
        return r;
    } else {
        return c;
    }
}

void load_bios(const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        perror("Failed to open BIOS file");
        exit(1);
    }
    fread(memory, 1, MEMORY_SIZE, file);
    fclose(file);
}

void load_harddisk(const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        perror("Failed to open hard disk image");
        exit(1);
    }
    fread(memory + 0x10000, 1, MEMORY_SIZE, file);
    fclose(file);
}

void emulate_8080() {
    // Simple emulation loop (placeholder)
    while (1) {
        uint8_t opcode = memory[pc++];
        print_debug_info(opcode); // Print debug info before executing the instruction
        switch (opcode) {
            case 0x00: // NOP
                break;
            case 0x01: // LXI B, word
                registers[1] = memory[pc++];
                registers[0] = memory[pc++];
                break;
            case 0x02: // STAX B
                memory[(registers[0] << 8) | registers[1]] = registers[2];
                break;
            case 0x03: // INX B
                if (++registers[1] == 0) {
                    ++registers[0];
                }
                break;
            case 0x04: // INR B
                registers[0]++;
                // Set flags accordingly
                break;
            case 0x05: // DCR B
                registers[0]--;
                // Set flags accordingly
                break;
            case 0x06: // MVI B, byte
                registers[0] = memory[pc++];
                break;
            case 0x07: // RLC
                {
                    uint8_t msb = (registers[2] & 0x80) >> 7;
                    registers[2] = (registers[2] << 1) | msb;
                    flags = (flags & 0xFE) | msb; // Set carry flag
                }
                break;
            case 0x08: // NOP (undocumented)
                break;
            case 0x09: // DAD B
                {
                    uint32_t hl = (registers[4] << 8) | registers[5];
                    uint32_t bc = (registers[0] << 8) | registers[1];
                    uint32_t result = hl + bc;
                    registers[4] = (result >> 8) & 0xFF;
                    registers[5] = result & 0xFF;
                    flags = (flags & 0xFE) | ((result & 0x10000) ? 1 : 0); // Set carry flag
                }
                break;
            case 0x0A: // LDAX B
                registers[2] = memory[(registers[0] << 8) | registers[1]];
                break;
            case 0x0B: // DCX B
                if (registers[1]-- == 0) {
                    --registers[0];
                }
                break;
            case 0x0C: // INR C
                registers[1]++;
                // Set flags accordingly
                break;
            case 0x0D: // DCR C
                registers[1]--;
                // Set flags accordingly
                break;
            case 0x0E: // MVI C, byte
                registers[1] = memory[pc++];
                break;
            case 0x0F: // RRC
                {
                    uint8_t lsb = registers[2] & 0x01;
                    registers[2] = (registers[2] >> 1) | (lsb << 7);
                    flags = (flags & 0xFE) | lsb; // Set carry flag
                }
                break;
            case 0x10: // NOP (undocumented)
                break;
            case 0x11: // LXI D, word
                registers[3] = memory[pc++];
                registers[2] = memory[pc++];
                break;
            case 0x12: // STAX D
                memory[(registers[2] << 8) | registers[3]] = registers[4];
                break;
            case 0x13: // INX D
                if (++registers[3] == 0) {
                    ++registers[2];
                }
                break;
            case 0x14: // INR D
                registers[2]++;
                // Set flags accordingly
                break;
            case 0x15: // DCR D
                registers[2]--;
                // Set flags accordingly
                break;
            case 0x16: // MVI D, byte
                registers[2] = memory[pc++];
                break;
            case 0x17: // RAL
                {
                    uint8_t msb = (registers[4] & 0x80) >> 7;
                    uint8_t carry = flags & 0x01;
                    registers[4] = (registers[4] << 1) | carry;
                    flags = (flags & 0xFE) | msb; // Set carry flag
                }
                break;
            case 0x18: // NOP (undocumented)
                break;
            case 0x19: // DAD D
                {
                    uint32_t hl = (registers[4] << 8) | registers[5];
                    uint32_t de = (registers[2] << 8) | registers[3];
                    uint32_t result = hl + de;
                    registers[4] = (result >> 8) & 0xFF;
                    registers[5] = result & 0xFF;
                    flags = (flags & 0xFE) | ((result & 0x10000) ? 1 : 0); // Set carry flag
                }
                break;
            case 0x1A: // LDAX D
                registers[4] = memory[(registers[2] << 8) | registers[3]];
                break;
            case 0x1B: // DCX D
                if (registers[3]-- == 0) {
                    --registers[2];
                }
                break;
            case 0x1C: // INR E
                registers[3]++;
                // Set flags accordingly
                break;
            case 0x1D: // DCR E
                registers[3]--;
                // Set flags accordingly
                break;
            case 0x1E: // MVI E, byte
                registers[3] = memory[pc++];
                break;
            case 0x1F: // RAR
                {
                    uint8_t lsb = registers[4] & 0x01;
                    uint8_t carry = flags & 0x01;
                    registers[4] = (registers[4] >> 1) | (carry << 7);
                    flags = (flags & 0xFE) | lsb; // Set carry flag
                }
                break;
            case 0x20: // NOP (undocumented)
                break;
            case 0x21: // LXI H, word
                registers[5] = memory[pc++];
                registers[4] = memory[pc++];
                break;
            case 0x22: // SHLD addr
                {
                    uint16_t addr = memory[pc++] | (memory[pc++] << 8);
                    memory[addr] = registers[5];
                    memory[addr + 1] = registers[4];
                }
                break;
            case 0x23: // INX H
                if (++registers[5] == 0) {
                    ++registers[4];
                }
                break;
            case 0x24: // INR H
                registers[4]++;
                // Set flags accordingly
                break;
            case 0x25: // DCR H
                registers[4]--;
                // Set flags accordingly
                break;
            case 0x26: // MVI H, byte
                registers[4] = memory[pc++];
                break;
            case 0x27: // DAA
                // Decimal Adjust Accumulator
                break;
            case 0x28: // NOP (undocumented)
                break;
            case 0x29: // DAD H
                {
                    uint32_t hl = (registers[4] << 8) | registers[5];
                    uint32_t hl2 = (registers[4] << 8) | registers[5];
                    uint32_t result = hl + hl2;
                    registers[4] = (result >> 8) & 0xFF;
                    registers[5] = result & 0xFF;
                    flags = (flags & 0xFE) | ((result & 0x10000) ? 1 : 0); // Set carry flag
                }
                break;
            case 0x2A: // LHLD addr
                {
                    uint16_t addr = memory[pc++] | (memory[pc++] << 8);
                    registers[5] = memory[addr];
                    registers[4] = memory[addr + 1];
                }
                break;
            case 0x2B: // DCX H
                if (registers[5]-- == 0) {
                    --registers[4];
                }
                break;
            case 0x2C: // INR L
                registers[5]++;
                // Set flags accordingly
                break;
            case 0x2D: // DCR L
                registers[5]--;
                // Set flags accordingly
                break;
            case 0x2E: // MVI L, byte
                registers[5] = memory[pc++];
                break;
            case 0x2F: // CMA
                registers[2] = ~registers[2];
                break;
            case 0x30: // NOP (undocumented)
                break;
            case 0x31: // LXI SP, word
                sp = memory[pc++] | (memory[pc++] << 8);
                break;
            case 0x32: // STA addr
                {
                    uint16_t addr = memory[pc++] | (memory[pc++] << 8);
                    memory[addr] = registers[2];
                }
                break;
            case 0x33: // INX SP
                sp++;
                break;
            case 0x34: // INR M
                {
                    uint16_t addr = (registers[4] << 8) | registers[5];
                    memory[addr]++;
                    // Set flags accordingly
                }
                break;
            case 0x35: // DCR M
                {
                    uint16_t addr = (registers[4] << 8) | registers[5];
                    memory[addr]--;
                    // Set flags accordingly
                }
                break;
            case 0x36: // MVI M, byte
                {
                    uint16_t addr = (registers[4] << 8) | registers[5];
                    memory[addr] = memory[pc++];
                }
                break;
            case 0x37: // STC
                flags |= 0x01; // Set carry flag
                break;
            case 0x38: // NOP (undocumented)
                break;
            case 0x39: // DAD SP
                {
                    uint32_t hl = (registers[4] << 8) | registers[5];
                    uint32_t result = hl + sp;
                    registers[4] = (result >> 8) & 0xFF;
                    registers[5] = result & 0xFF;
                    flags = (flags & 0xFE) | ((result & 0x10000) ? 1 : 0); // Set carry flag
                }
                break;
            case 0x3A: // LDA addr
                {
                    uint16_t addr = memory[pc++] | (memory[pc++] << 8);
                    registers[2] = memory[addr];
                }
                break;
            case 0x3B: // DCX SP
                sp--;
                break;
            case 0x3C: // INR A
                registers[2]++;
                // Set flags accordingly
                break;
            case 0x3D: // DCR A
                registers[2]--;
                // Set flags accordingly
                break;
            case 0x3E: // MVI A, byte
                registers[2] = memory[pc++];
                break;
            case 0x3F: // CMC
                flags ^= 0x01; // Complement carry flag
                break;
            case 0x40: // MOV B, B
                // No operation needed
                break;
            case 0x41: // MOV B, C
                registers[0] = registers[1];
                break;
            case 0x42: // MOV B, D
                registers[0] = registers[2];
                break;
            case 0x43: // MOV B, E
                registers[0] = registers[3];
                break;
            case 0x44: // MOV B, H
                registers[0] = registers[4];
                break;
            case 0x45: // MOV B, L
                registers[0] = registers[5];
                break;
            case 0x46: // MOV B, M
                {
                    uint16_t addr = (registers[4] << 8) | registers[5];
                    registers[0] = memory[addr];
                }
                break;
            case 0x47: // MOV B, A
                registers[0] = registers[2];
                break;
            case 0x48: // MOV C, B
                registers[1] = registers[0];
                break;
            case 0x49: // MOV C, C
                // No operation needed
                break;
            case 0x4A: // MOV C, D
                registers[1] = registers[2];
                break;
            case 0x4B: // MOV C, E
                registers[1] = registers[3];
                break;
            case 0x4C: // MOV C, H
                registers[1] = registers[4];
                break;
            case 0x4D: // MOV C, L
                registers[1] = registers[5];
                break;
            case 0x4E: // MOV C, M
                {
                    uint16_t addr = (registers[4] << 8) | registers[5];
                    registers[1] = memory[addr];
                }
                break;
            case 0x4F: // MOV C, A
                registers[1] = registers[2];
                break;
            case 0x50: // MOV D, B
                registers[2] = registers[0];
                break;
            case 0x51: // MOV D, C
                registers[2] = registers[1];
                break;
            case 0x52: // MOV D, D
                // No operation needed
                break;
            case 0x53: // MOV D, E
                registers[2] = registers[3];
                break;
            case 0x54: // MOV D, H
                registers[2] = registers[4];
                break;
            case 0x55: // MOV D, L
                registers[2] = registers[5];
                break;
            case 0x56: // MOV D, M
                {
                    uint16_t addr = (registers[4] << 8) | registers[5];
                    registers[2] = memory[addr];
                }
                break;
            case 0x57: // MOV D, A
                registers[2] = registers[2];
                break;
            case 0x58: // MOV E, B
                registers[3] = registers[0];
                break;
            case 0x59: // MOV E, C
                registers[3] = registers[1];
                break;
            case 0x5A: // MOV E, D
                registers[3] = registers[2];
                break;
            case 0x5B: // MOV E, E
                // No operation needed
                break;
            case 0x5C: // MOV E, H
                registers[3] = registers[4];
                break;
            case 0x5D: // MOV E, L
                registers[3] = registers[5];
                break;
            case 0x5E: // MOV E, M
                {
                    uint16_t addr = (registers[4] << 8) | registers[5];
                    registers[3] = memory[addr];
                }
                break;
            case 0x5F: // MOV E, A
                registers[3] = registers[2];
                break;
            case 0x60: // MOV H, B
                registers[4] = registers[0];
                break;
            case 0x61: // MOV H, C
                registers[4] = registers[1];
                break;
            case 0x62: // MOV H, D
                registers[4] = registers[2];
                break;
            case 0x63: // MOV H, E
                registers[4] = registers[3];
                break;
            case 0x64: // MOV H, H
                // No operation needed
                break;
            case 0x65: // MOV H, L
                registers[4] = registers[5];
                break;
            case 0x66: // MOV H, M
                {
                    uint16_t addr = (registers[4] << 8) | registers[5];
                    registers[4] = memory[addr];
                }
                break;
            case 0x67: // MOV H, A
                registers[4] = registers[2];
                break;
            case 0x68: // MOV L, B
                registers[5] = registers[0];
                break;
            case 0x69: // MOV L, C
                registers[5] = registers[1];
                break;
            case 0x6A: // MOV L, D
                registers[5] = registers[2];
                break;
            case 0x6B: // MOV L, E
                registers[5] = registers[3];
                break;
            case 0x6C: // MOV L, H
                registers[5] = registers[4];
                break;
            case 0x6D: // MOV L, L
                // No operation needed
                break;
            case 0x6E: // MOV L, M
                {
                    uint16_t addr = (registers[4] << 8) | registers[5];
                    registers[5] = memory[addr];
                }
                break;
            case 0x6F: // MOV L, A
                registers[5] = registers[2];
                break;
            case 0x70: // MOV M, B
                {
                    uint16_t addr = (registers[4] << 8) | registers[5];
                    memory[addr] = registers[0];
                }
                break;
            case 0x71: // MOV M, C
                {
                    uint16_t addr = (registers[4] << 8) | registers[5];
                    memory[addr] = registers[1];
                }
                break;
            case 0x72: // MOV M, D
                {
                    uint16_t addr = (registers[4] << 8) | registers[5];
                    memory[addr] = registers[2];
                }
                break;
            case 0x73: // MOV M, E
                {
                    uint16_t addr = (registers[4] << 8) | registers[5];
                    memory[addr] = registers[3];
                }
                break;
            case 0x74: // MOV M, H
                {
                    uint16_t addr = (registers[4] << 8) | registers[5];
                    memory[addr] = registers[4];
                }
                break;
            case 0x75: // MOV M, L
                {
                    uint16_t addr = (registers[4] << 8) | registers[5];
                    memory[addr] = registers[5];
                }
                break;
            case 0x76: // HLT
                return;
            case 0x77: // MOV M, A
                {
                    uint16_t addr = (registers[4] << 8) | registers[5];
                    memory[addr] = registers[2];
                }
                break;
            case 0x78: // MOV A, B
                registers[2] = registers[0];
                break;
            case 0x79: // MOV A, C
                registers[2] = registers[1];
                break;
            case 0x7A: // MOV A, D
                registers[2] = registers[3];
                break;
            case 0x7B: // MOV A, E
                registers[2] = registers[3];
                break;
            case 0x7C: // MOV A, H
                registers[2] = registers[4];
                break;
            case 0x7D: // MOV A, L
                registers[2] = registers[5];
                break;
            case 0x7E: // MOV A, M
                {
                    uint16_t addr = (registers[4] << 8) | registers[5];
                    registers[2] = memory[addr];
                }
                break;
            case 0x7F: // MOV A, A
                // No operation needed
                break;            
            case 0x80: // ADD B
                {
                    uint16_t result = registers[2] + registers[0];
                    registers[2] = result & 0xFF;
                    flags = (flags & 0xFE) | ((result & 0x100) ? 1 : 0); // Set carry flag
                    // Set other flags (zero, sign, parity, auxiliary carry)
                }
                break;
            case 0x81: // ADD C
                {
                    uint16_t result = registers[2] + registers[1];
                    registers[2] = result & 0xFF;
                    flags = (flags & 0xFE) | ((result & 0x100) ? 1 : 0); // Set carry flag
                    // Set other flags (zero, sign, parity, auxiliary carry)
                }
                break;
            case 0x82: // ADD D
                {
                    uint16_t result = registers[2] + registers[2];
                    registers[2] = result & 0xFF;
                    flags = (flags & 0xFE) | ((result & 0x100) ? 1 : 0); // Set carry flag
                    // Set other flags (zero, sign, parity, auxiliary carry)
                }
                break;
            case 0x83: // ADD E
                {
                    uint16_t result = registers[2] + registers[3];
                    registers[2] = result & 0xFF;
                    flags = (flags & 0xFE) | ((result & 0x100) ? 1 : 0); // Set carry flag
                    // Set other flags (zero, sign, parity, auxiliary carry)
                }
                break;
            case 0x84: // ADD H
                {
                    uint16_t result = registers[2] + registers[4];
                    registers[2] = result & 0xFF;
                    flags = (flags & 0xFE) | ((result & 0x100) ? 1 : 0); // Set carry flag
                    // Set other flags (zero, sign, parity, auxiliary carry)
                }
                break;
            case 0x85: // ADD L
                {
                    uint16_t result = registers[2] + registers[5];
                    registers[2] = result & 0xFF;
                    flags = (flags & 0xFE) | ((result & 0x100) ? 1 : 0); // Set carry flag
                    // Set other flags (zero, sign, parity, auxiliary carry)
                }
                break;
            case 0x86: // ADD M
                {
                    uint16_t addr = (registers[4] << 8) | registers[5];
                    uint16_t result = registers[2] + memory[addr];
                    registers[2] = result & 0xFF;
                    flags = (flags & 0xFE) | ((result & 0x100) ? 1 : 0); // Set carry flag
                    // Set other flags (zero, sign, parity, auxiliary carry)
                }
                break;
            case 0x87: // ADD A
                {
                    uint16_t result = registers[2] + registers[2];
                    registers[2] = result & 0xFF;
                    flags = (flags & 0xFE) | ((result & 0x100) ? 1 : 0); // Set carry flag
                    // Set other flags (zero, sign, parity, auxiliary carry)
                }
                break;
            case 0x88: // ADC B
                {
                    uint16_t result = registers[2] + registers[0] + (flags & 0x01);
                    registers[2] = result & 0xFF;
                    flags = (flags & 0xFE) | ((result & 0x100) ? 1 : 0); // Set carry flag
                    // Set other flags (zero, sign, parity, auxiliary carry)
                }
                break;
            case 0x89: // ADC C
                {
                    uint16_t result = registers[2] + registers[1] + (flags & 0x01);
                    registers[2] = result & 0xFF;
                    flags = (flags & 0xFE) | ((result & 0x100) ? 1 : 0); // Set carry flag
                    // Set other flags (zero, sign, parity, auxiliary carry)
                }
                break;
            case 0x8A: // ADC D
                {
                    uint16_t result = registers[2] + registers[2] + (flags & 0x01);
                    registers[2] = result & 0xFF;
                    flags = (flags & 0xFE) | ((result & 0x100) ? 1 : 0); // Set carry flag
                    // Set other flags (zero, sign, parity, auxiliary carry)
                }
                break;
            case 0x8B: // ADC E
                {
                    uint16_t result = registers[2] + registers[3] + (flags & 0x01);
                    registers[2] = result & 0xFF;
                    flags = (flags & 0xFE) | ((result & 0x100) ? 1 : 0); // Set carry flag
                    // Set other flags (zero, sign, parity, auxiliary carry)
                }
                break;
            case 0x8C: // ADC H
                {
                    uint16_t result = registers[2] + registers[4] + (flags & 0x01);
                    registers[2] = result & 0xFF;
                    flags = (flags & 0xFE) | ((result & 0x100) ? 1 : 0); // Set carry flag
                    // Set other flags (zero, sign, parity, auxiliary carry)
                }
                break;
            case 0x8D: // ADC L
                {
                    uint16_t result = registers[2] + registers[5] + (flags & 0x01);
                    registers[2] = result & 0xFF;
                    flags = (flags & 0xFE) | ((result & 0x100) ? 1 : 0); // Set carry flag
                    // Set other flags (zero, sign, parity, auxiliary carry)
                }
                break;
            case 0x8E: // ADC M
                {
                    uint16_t addr = (registers[4] << 8) | registers[5];
                    uint16_t result = registers[2] + memory[addr] + (flags & 0x01);
                    registers[2] = result & 0xFF;
                    flags = (flags & 0xFE) | ((result & 0x100) ? 1 : 0); // Set carry flag
                    // Set other flags (zero, sign, parity, auxiliary carry)
                }
                break;
            case 0x8F: // ADC A
                {
                    uint16_t result = registers[2] + registers[2] + (flags & 0x01);
                    registers[2] = result & 0xFF;
                    flags = (flags & 0xFE) | ((result & 0x100) ? 1 : 0); // Set carry flag
                    // Set other flags (zero, sign, parity, auxiliary carry)
                }
                break;
            case 0x90: // SUB B
                {
                    uint16_t result = registers[2] - registers[0];
                    registers[2] = result & 0xFF;
                    flags = (flags & 0xFE) | ((result & 0x100) ? 1 : 0); // Set carry flag
                    // Set other flags (zero, sign, parity, auxiliary carry)
                }
                break;
            case 0x91: // SUB C
                {
                    uint16_t result = registers[2] - registers[1];
                    registers[2] = result & 0xFF;
                    flags = (flags & 0xFE) | ((result & 0x100) ? 1 : 0); // Set carry flag
                    // Set other flags (zero, sign, parity, auxiliary carry)
                }
                break;
            case 0x92: // SUB D
                {
                    uint16_t result = registers[2] - registers[2];
                    registers[2] = result & 0xFF;
                    flags = (flags & 0xFE) | ((result & 0x100) ? 1 : 0); // Set carry flag
                    // Set other flags (zero, sign, parity, auxiliary carry)
                }
                break;
            case 0x93: // SUB E
                {
                    uint16_t result = registers[2] - registers[3];
                    registers[2] = result & 0xFF;
                    flags = (flags & 0xFE) | ((result & 0x100) ? 1 : 0); // Set carry flag
                    // Set other flags (zero, sign, parity, auxiliary carry)
                }
                break;
            case 0x94: // SUB H
                {
                    uint16_t result = registers[2] - registers[4];
                    registers[2] = result & 0xFF;
                    flags = (flags & 0xFE) | ((result & 0x100) ? 1 : 0); // Set carry flag
                    // Set other flags (zero, sign, parity, auxiliary carry)
                }
                break;
            case 0x95: // SUB L
                {
                    uint16_t result = registers[2] - registers[5];
                    registers[2] = result & 0xFF;
                    flags = (flags & 0xFE) | ((result & 0x100) ? 1 : 0); // Set carry flag
                    // Set other flags (zero, sign, parity, auxiliary carry)
                }
                break;
            case 0x96: // SUB M
                {
                    uint16_t addr = (registers[4] << 8) | registers[5];
                    uint16_t result = registers[2] - memory[addr];
                    registers[2] = result & 0xFF;
                    flags = (flags & 0xFE) | ((result & 0x100) ? 1 : 0); // Set carry flag
                    // Set other flags (zero, sign, parity, auxiliary carry)
                }
                break;
            case 0x97: // SUB A
                {
                    uint16_t result = registers[2] - registers[2];
                    registers[2] = result & 0xFF;
                    flags = (flags & 0xFE) | ((result & 0x100) ? 1 : 0); // Set carry flag
                    // Set other flags (zero, sign, parity, auxiliary carry)
                }
                break;
            case 0x98: // SBB B
                {
                    uint16_t result = registers[2] - registers[0] - (flags & 0x01);
                    registers[2] = result & 0xFF;
                    flags = (flags & 0xFE) | ((result & 0x100) ? 1 : 0); // Set carry flag
                    // Set other flags (zero, sign, parity, auxiliary carry)
                }
                break;
            case 0x99: // SBB C
                {
                    uint16_t result = registers[2] - registers[1] - (flags & 0x01);
                    registers[2] = result & 0xFF;
                    flags = (flags & 0xFE) | ((result & 0x100) ? 1 : 0); // Set carry flag
                    // Set other flags (zero, sign, parity, auxiliary carry)
                }
                break;
            case 0x9A: // SBB D
                {
                    uint16_t result = registers[2] - registers[2] - (flags & 0x01);
                    registers[2] = result & 0xFF;
                    flags = (flags & 0xFE) | ((result & 0x100) ? 1 : 0); // Set carry flag
                    // Set other flags (zero, sign, parity, auxiliary carry)
                }
                break;
            case 0x9B: // SBB E
                {
                    uint16_t result = registers[2] - registers[3] - (flags & 0x01);
                    registers[2] = result & 0xFF;
                    flags = (flags & 0xFE) | ((result & 0x100) ? 1 : 0); // Set carry flag
                    // Set other flags (zero, sign, parity, auxiliary carry)
                }
                break;
            case 0x9C: // SBB H
                {
                    uint16_t result = registers[2] - registers[4] - (flags & 0x01);
                    registers[2] = result & 0xFF;
                    flags = (flags & 0xFE) | ((result & 0x100) ? 1 : 0); // Set carry flag
                    // Set other flags (zero, sign, parity, auxiliary carry)
                }
                break;
            case 0x9D: // SBB L
                {
                    uint16_t result = registers[2] - registers[5] - (flags & 0x01);
                    registers[2] = result & 0xFF;
                    flags = (flags & 0xFE) | ((result & 0x100) ? 1 : 0); // Set carry flag
                    // Set other flags (zero, sign, parity, auxiliary carry)
                }
                break;
            case 0x9E: // SBB M
                {
                    uint16_t addr = (registers[4] << 8) | registers[5];
                    uint16_t result = registers[2] - memory[addr] - (flags & 0x01);
                    registers[2] = result & 0xFF;
                    flags = (flags & 0xFE) | ((result & 0x100) ? 1 : 0); // Set carry flag
                    // Set other flags (zero, sign, parity, auxiliary carry)
                }
                break;
            case 0x9F: // SBB A
                {
                    uint16_t result = registers[2] - registers[2] - (flags & 0x01);
                    registers[2] = result & 0xFF;
                    flags = (flags & 0xFE) | ((result & 0x100) ? 1 : 0); // Set carry flag
                    // Set other flags (zero, sign, parity, auxiliary carry)
                }
                break;
            case 0xA0: // ANA B
                {
                    registers[2] &= registers[0];
                    // Set flags accordingly
                }
                break;
            case 0xA1: // ANA C
                {
                    registers[2] &= registers[1];
                    // Set flags accordingly
                }
                break;
            case 0xA2: // ANA D
                {
                    registers[2] &= registers[2];
                    // Set flags accordingly
                }
                break;
            case 0xA3: // ANA E
                {
                    registers[2] &= registers[3];
                    // Set flags accordingly
                }
                break;
            case 0xA4: // ANA H
                {
                    registers[2] &= registers[4];
                    // Set flags accordingly
                }
                break;
            case 0xA5: // ANA L
                {
                    registers[2] &= registers[5];
                    // Set flags accordingly
                }
                break;
            case 0xA6: // ANA M
                {
                    uint16_t addr = (registers[4] << 8) | registers[5];
                    registers[2] &= memory[addr];
                    // Set flags accordingly
                }
                break;
            case 0xA7: // ANA A
                {
                    registers[2] &= registers[2];
                    // Set flags accordingly
                }
                break;
            case 0xA8: // XRA B
                {
                    registers[2] ^= registers[0];
                    // Set flags accordingly
                }
                break;
            case 0xA9: // XRA C
                {
                    registers[2] ^= registers[1];
                    // Set flags accordingly
                }
                break;
            case 0xAA: // XRA D
                {
                    registers[2] ^= registers[2];
                    // Set flags accordingly
                }
                break;
            case 0xAB: // XRA E
                {
                    registers[2] ^= registers[3];
                    // Set flags accordingly
                }
                break;
            case 0xAC: // XRA H
                {
                    registers[2] ^= registers[4];
                    // Set flags accordingly
                }
                break;
            case 0xAD: // XRA L
                {
                    registers[2] ^= registers[5];
                    // Set flags accordingly
                }
                break;
            case 0xAE: // XRA M
                {
                    uint16_t addr = (registers[4] << 8) | registers[5];
                    registers[2] ^= memory[addr];
                    // Set flags accordingly
                }
                break;
            case 0xAF: // XRA A
                {
                    registers[2] ^= registers[2];
                    // Set flags accordingly
                }
                break;
            case 0xB0: // ORA B
                {
                    registers[2] |= registers[0];
                    // Set flags accordingly
                }
                break;
            case 0xB1: // ORA C
                {
                    registers[2] |= registers[1];
                    // Set flags accordingly
                }
                break;
            case 0xB2: // ORA D
                {
                    registers[2] |= registers[2];
                    // Set flags accordingly
                }
                break;
            case 0xB3: // ORA E
                {
                    registers[2] |= registers[3];
                    // Set flags accordingly
                }
                break;
            case 0xB4: // ORA H
                {
                    registers[2] |= registers[4];
                    // Set flags accordingly
                }
                break;
            case 0xB5: // ORA L
                {
                    registers[2] |= registers[5];
                    // Set flags accordingly
                }
                break;
            case 0xB6: // ORA M
                {
                    uint16_t addr = (registers[4] << 8) | registers[5];
                    registers[2] |= memory[addr];
                    // Set flags accordingly
                }
                break;
            case 0xB7: // ORA A
                {
                    registers[2] |= registers[2];
                    // Set flags accordingly
                }
                break;
            case 0xB8: // CMP B
                {
                    uint16_t result = registers[2] - registers[0];
                    // Set flags accordingly
                }
                break;
            case 0xB9: // CMP C
                {
                    uint16_t result = registers[2] - registers[1];
                    // Set flags accordingly
                }
                break;
            case 0xBA: // CMP D
                {
                    uint16_t result = registers[2] - registers[2];
                    // Set flags accordingly
                }
                break;
            case 0xBB: // CMP E
                {
                    uint16_t result = registers[2] - registers[3];
                    // Set flags accordingly
                }
                break;
            case 0xBC: // CMP H
                {
                    uint16_t result = registers[2] - registers[4];
                    // Set flags accordingly
                }
                break;
            case 0xBD: // CMP L
                {
                    uint16_t result = registers[2] - registers[5];
                    // Set flags accordingly
                }
                break;
            case 0xBE: // CMP M
                {
                    uint16_t addr = (registers[4] << 8) | registers[5];
                    uint16_t result = registers[2] - memory[addr];
                    // Set flags accordingly
                }
                break;
            case 0xBF: // CMP A
                {
                    uint16_t result = registers[2] - registers[2];
                    // Set flags accordingly
                }
                break;
            case 0xC0: // RNZ (Return if Not Zero)
                if (!(flags & 0x40)) {
                    pc = memory[sp++] | (memory[sp++] << 8);
                }
                break;
            case 0xC1: // POP B
                registers[1] = memory[sp++];
                registers[0] = memory[sp++];
                break;
            case 0xC2: // JNZ addr (Jump if Not Zero)
            {
                uint16_t addr = memory[pc++] | (memory[pc++] << 8);
                if (!(flags & 0x40)) {
                    pc = addr;
                }
            }
                break;            
            case 0xC3: // JMP addr
            {
                uint16_t addr = memory[pc++] | (memory[pc++] << 8);
                pc = addr;
            }
            break;
            case 0xC5: // PUSH B
                memory[--sp] = registers[0];
                memory[--sp] = registers[1];
                break;
            case 0xC6: // ADI byte
                {
                    uint16_t result = registers[2] + memory[pc++];
                    registers[2] = result & 0xFF;
                    flags = (flags & 0xFE) | ((result & 0x100) ? 1 : 0); // Set carry flag
                    // Set other flags (zero, sign, parity, auxiliary carry)
                }
                break;
            case 0xC7: // RST 0
                memory[--sp] = (pc >> 8) & 0xFF; // Push high byte of PC
                memory[--sp] = pc & 0xFF;        // Push low byte of PC
                pc = 0x00;                       // Set PC to 0x00
                break;
            case 0xC8: // RZ (Return if Zero)
                if (flags & 0x40) {
                    pc = memory[sp++] | (memory[sp++] << 8);
                }
                break;
            case 0xC9: // RET
                pc = memory[sp++] | (memory[sp++] << 8);
                break;      
            
            case 0xCD: // CALL addr
            {
                uint16_t addr = memory[pc++] | (memory[pc++] << 8);
                memory[--sp] = (pc >> 8) & 0xFF; // Push high byte of PC
                memory[--sp] = pc & 0xFF;        // Push low byte of PC
                pc = addr;                       // Set PC to the address
            }
            break;
            case 0xCE: // ACI byte
                {
                    uint16_t result = registers[2] + memory[pc++] + (flags & 0x01);
                    registers[2] = result & 0xFF;
                    flags = (flags & 0xFE) | ((result & 0x100) ? 1 : 0); // Set carry flag
                    // Set other flags (zero, sign, parity, auxiliary carry)
                }
                break;
            case 0xD1: // POP D
                registers[3] = memory[sp++];
                registers[2] = memory[sp++];
                break;
            case 0xD2: // JNC addr
            {
                uint16_t addr = memory[pc++] | (memory[pc++] << 8);
                if (!(flags & 0x01)) {
                    pc = addr;
                }
            }
            break;
            case 0xD3: // OUT port
            {
                uint8_t port = memory[pc++];
                uint8_t value = registers[2];
                write_port(port, value);
                if (port == 0x80) {
                    printf("Debug port write: %02x\n\r", value);
                }
                // Example: Handle port 0x02 for output
                if (port == 0x02) {
                    // Example: Write output to a display or other device
                    // This is just a placeholder for demonstration purposes
                    printf("Output to port 0x02: %02x\n\r", value);
                }
            }
            break;
            case 0xD4: // CNC addr
            {
                uint16_t addr = memory[pc++] | (memory[pc++] << 8);
                if (!(flags & 0x01)) {
                    memory[--sp] = (pc >> 8) & 0xFF; // Push high byte of PC
                    memory[--sp] = pc & 0xFF;        // Push low byte of PC
                    pc = addr;                       // Set PC to the address
                }
            }
            break;
            case 0xD5: // PUSH D
                memory[--sp] = registers[2];
                memory[--sp] = registers[3];
                break;
            case 0xD6: // SUI byte
                {
                    uint16_t result = registers[2] - memory[pc++];
                    registers[2] = result & 0xFF;
                    flags = (flags & 0xFE) | ((result & 0x100) ? 1 : 0); // Set carry flag
                    // Set other flags (zero, sign, parity, auxiliary carry)
                }
                break;
            case 0xD7: // RST 2
                memory[--sp] = (pc >> 8) & 0xFF; // Push high byte of PC
                memory[--sp] = pc & 0xFF;        // Push low byte of PC
                pc = 0x10;                       // Set PC to 0x10
                break;
            case 0xD8: // RC (Return if Carry)
                if (flags & 0x01) {
                    pc = memory[sp++] | (memory[sp++] << 8);
                }
                break;
            case 0xDA: // JC addr
            {
                uint16_t addr = memory[pc++] | (memory[pc++] << 8);
                if (flags & 0x01) {
                    pc = addr;
                }
            }
            break;
            case 0xDB: // IN port
                uint8_t port = memory[pc++];
                registers[2] = read_port(port);
                if (port == 0x80) {
                    printf("Debug port read\n\r");
                }
                else if (port == 0x01) {// Example: Handle port 0x01 for input
                    // Example: Read input from a keyboard or other device
                    // This is just a placeholder for demonstration purposes
                    uint8_t input_value = 0x00; // Replace with actual input reading logic
                    registers[2] = input_value;
                }
                else if (port == 0x02) {// Example: Handle port 0x02 for output
                    // Example: Write output to a display or other device
                    // This is just a placeholder for demonstration purposes
                    write_port(port, registers[2]);
                    // Replace with actual output writing logic
                    printf("Output to port 0x02: %02x\n\r", registers[2]);
                }else
                {
                    printf("Port: %02x, Value: %02x\n\r", port, registers[2]);
                }
                
                
                break;
            case 0xDC: // CC addr
            {
                uint16_t addr = memory[pc++] | (memory[pc++] << 8);
                if (flags & 0x01) {
                    memory[--sp] = (pc >> 8) & 0xFF; // Push high byte of PC
                    memory[--sp] = pc & 0xFF;        // Push low byte of PC
                    pc = addr;                       // Set PC to the address
                }
            }
            break;
            case 0xDE: // SBI byte
                {
                    uint16_t result = registers[2] - memory[pc++] - (flags & 0x01);
                    registers[2] = result & 0xFF;
                    flags = (flags & 0xFE) | ((result & 0x100) ? 1 : 0); // Set carry flag
                    // Set other flags (zero, sign, parity, auxiliary carry)
                }
                break;
            case 0xDF: // RST 3 
                memory[--sp] = (pc >> 8) & 0xFF; // Push high byte of PC
                memory[--sp] = pc & 0xFF;        // Push low byte of PC
                pc = 0x18;                       // Set PC to 0x18
                break;
            case 0xE0: // RPO (Return if Parity Odd)
                if (!(flags & 0x04)) {
                    pc = memory[sp++] | (memory[sp++] << 8);
                }
                break;
            case 0xE1: // POP H
                registers[5] = memory[sp++];
                registers[4] = memory[sp++];
                break;
            case 0xE2: // JPO addr
            {
                uint16_t addr = memory[pc++] | (memory[pc++] << 8);
                if (!(flags & 0x04)) {
                    pc = addr;
                }
            }
            break;
            case 0xE3: // XTHL
            {
                uint8_t tempL = registers[5];
                uint8_t tempH = registers[4];
                registers[5] = memory[sp];
                registers[4] = memory[sp + 1];
                memory[sp] = tempL;
                memory[sp + 1] = tempH;
            }
            break;
            case 0xE5: // PUSH H
                memory[--sp] = registers[4];
                memory[--sp] = registers[5];
                break;
            case 0xE6: // ANI byte
            {
                registers[2] &= memory[pc++];
                // Set flags accordingly
            }
            break;
            case 0xE7: // RST 4
                memory[--sp] = (pc >> 8) & 0xFF; // Push high byte of PC
                memory[--sp] = pc & 0xFF;        // Push low byte of PC
                pc = 0x20;                       // Set PC to 0x20
                break;
            case 0xE8: // RPE (Return if Parity Even)
                if (flags & 0x04) {
                    pc = memory[sp++] | (memory[sp++] << 8);
                }
                break;
            case 0xE9: // PCHL
                pc = (registers[4] << 8) | registers[5];
                break;
            case 0xEB: // XCHG
            {
                uint8_t tempL = registers[5];
                uint8_t tempH = registers[4];
                registers[5] = registers[3];
                registers[4] = registers[2];
                registers[3] = tempL;
                registers[2] = tempH;
            }
            break;
            case 0xEC: // CPE addr
            {
                uint16_t addr = memory[pc++] | (memory[pc++] << 8);
                if (flags & 0x04) {
                    memory[--sp] = (pc >> 8) & 0xFF; // Push high byte of PC
                    memory[--sp] = pc & 0xFF;        // Push low byte of PC
                    pc = addr;                       // Set PC to the address
                }
            }
            break;
            case 0xED: // NOP
                // No operation needed
                break;
            case 0xEE: // XRI byte
            {
                registers[2] ^= memory[pc++];
                // Set flags accordingly
            }
            break;
            case 0xEF: // RST 5
                memory[--sp] = (pc >> 8) & 0xFF; // Push high byte of PC
                memory[--sp] = pc & 0xFF;        // Push low byte of PC
                pc = 0x28;                       // Set PC to 0x28
                break;


            


//---------------------------------------------------

            case 0xF1: // POP PSW
            {
                registers[2] = memory[sp++]; // Pop accumulator
                flags = memory[sp++];        // Pop flags
            }
            break; 
            case 0xF3: // DI (Disable Interrupts)
                clear_flag(7); // CLear the 7th flag to disable interrupts
                break;
            case 0xF5: // PUSH PSW
            {
                memory[--sp] = flags;
                memory[--sp] = registers[2]; // Push accumulator
            }
            break;
            case 0xF6: // ORI byte
            {
                registers[2] |= memory[pc++];
                // Set flags accordingly
            }
            break;
            case 0xF7: // RST 6
                memory[--sp] = (pc >> 8) & 0xFF; // Push high byte of PC
                memory[--sp] = pc & 0xFF;        // Push low byte of PC
                pc = 0x30;                       // Set PC to 0x30
                break;
            case 0xF9: // SPHL
            {
                sp = (registers[4] << 8) | registers[5];
            }
            break;
            case 0xFA: // JM addr
            {
                uint16_t addr = memory[pc++] | (memory[pc++] << 8);
                if (flags & 0x80) { // Check sign flag
                    pc = addr;
                }
            }
            case 0xFB: // EI (Enable Interrupts)
                set_flag(7); // Set the 7th flag to enable interrupts
                break;
            case 0xFC: // CM addr
            {
                uint16_t addr = memory[pc++] | (memory[pc++] << 8);
                if (flags & 0x80) { // Check sign flag
                    pc = addr;
                }
            }
            break;  
            case 0xFD: // NOP
                // No operation needed
                break;
            case 0xFE: // CPI byte
                {
                    uint16_t result = registers[2] - memory[pc++];
                    // Set flags accordingly
                }
                break;
            
            break;
            case 0xFF: // RST 7
                memory[--sp] = (pc >> 8) & 0xFF; // Push high byte of PC
                memory[--sp] = pc & 0xFF;        // Push low byte of PC
                pc = 0x38;                       // Set PC to 0x38
                break;


            default:
                printf("Unknown opcode: %02x\n", opcode);
                exit(1);
        }

        if (kbhit()) {
            int ch = getch();
            if (ch == 'q') {
                break;
            }
            // Handle other input
        }

        usleep(1000); // Slow down the emulation
    }
}

private:
    uint8_t memory[MEMORY_SIZE];
    uint8_t registers[7];
    uint16_t pc, sp;
    uint8_t flags;
    uint8_t io_ports[IO_PORTS];
};


int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <bios> [harddisk]\n", argv[0]);
        return 1;
    }

   // set_conio_terminal_mode();

   // load_bios(argv[1]);
    if (argc > 2) {
     //   load_harddisk(argv[2]);
    }

    //emulate_8080();
    //reset_terminal_mode();
    return 0;
}

// So i may have taken some inspiration from: https://nanochess.org/emulator.html
// But after many hours of trying to decode it and make sense...
// I decided to just make my own, cause i got nowhere.
// End of file