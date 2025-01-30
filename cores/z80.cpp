#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <string>
#include <iomanip>
#include <sstream>
#include <unordered_map>
#include <cstdint>
#include <algorithm>

// Function to decode 0xED prefixed opcodes
std::string decodeEDOpcode(uint8_t edOpcode) {
    static const std::unordered_map<uint8_t, std::string> edOpcodeMap = {
        {0x40, "IN B,(C)"},
        {0x41, "OUT (C),B"},
        {0x42, "SBC HL,BC"},
        {0x43, "LD (nn),BC"},
        {0x44, "NEG"},
        {0x45, "RETN"},
        {0x46, "IM 0"},
        {0x47, "LD I,A"},
        {0x48, "IN C,(C)"},
        {0x49, "OUT (C),C"},
        {0x4A, "ADC HL,BC"},
        {0x4B, "LD BC,(nn)"},
        {0x4D, "RETI"},
        {0x4F, "LD R,A"},
        {0x50, "IN D,(C)"},
        {0x51, "OUT (C),D"},
        {0x52, "SBC HL,DE"},
        {0x53, "LD (nn),DE"},
        {0x56, "IM 1"},
        {0x57, "LD A,I"},
        {0x58, "IN E,(C)"},
        {0x59, "OUT (C),E"},
        {0x5A, "ADC HL,DE"},
        {0x5B, "LD DE,(nn)"},
        {0x5E, "IM 2"},
        {0x5F, "LD A,R"},
        {0x60, "IN H,(C)"},
        {0x61, "OUT (C),H"},
        {0x62, "SBC HL,HL"},
        {0x63, "LD (nn),HL"},
        {0x67, "RRD"},
        {0x68, "IN L,(C)"},
        {0x69, "OUT (C),L"},
        {0x6A, "ADC HL,HL"},
        {0x6B, "LD HL,(nn)"},
        {0x6F, "RLD"},
        {0x70, "IN (C)"},
        {0x71, "OUT (C),0"},
        {0x72, "SBC HL,SP"},
        {0x73, "LD (nn),SP"},
        {0x78, "IN A,(C)"},
        {0x79, "OUT (C),A"},
        {0x7A, "ADC HL,SP"},
        {0x7B, "LD SP,(nn)"},
        {0xA0, "LDI"},
        {0xA1, "CPI"},
        {0xA2, "INI"},
        {0xA3, "OUTI"},
        {0xA8, "LDD"},
        {0xA9, "CPD"},
        {0xAA, "IND"},
        {0xAB, "OUTD"},
        {0xB0, "LDIR"},
        {0xB1, "CPIR"},
        {0xB2, "INIR"},
        {0xB3, "OTIR"},
        {0xB8, "LDDR"},
        {0xB9, "CPDR"},
        {0xBA, "INDR"},
        {0xBB, "OTDR"}
    };

    auto it = edOpcodeMap.find(edOpcode);
    if (it != edOpcodeMap.end()) {
        return it->second;
    } else {
        return "UNKNOWN";
    }
}


// Function to check if a byte is a printable ASCII character
bool is_printable_ascii(uint8_t byte) {
    return (0x20 <= byte && byte <= 0x7E) || byte == 0x0A || byte == 0x0D;
}

// Function to detect strings in the ROM dump
std::vector<std::pair<size_t, std::vector<uint8_t>>> detect_strings(const std::vector<uint8_t>& rom_dump, size_t min_length = 3) {
    std::vector<std::pair<size_t, std::vector<uint8_t>>> strings;
    std::vector<uint8_t> current_string;
    size_t start_index = 0;

    for (size_t i = 0; i < rom_dump.size(); ++i) {
        uint8_t byte = rom_dump[i];
        if (is_printable_ascii(byte)) {
            if (current_string.empty()) {
                start_index = i;
            }
            current_string.push_back(byte);
        } else if (byte == 0x00) {
            if (current_string.size() >= min_length) {
                current_string.push_back(byte);
                strings.push_back({start_index, current_string});
            }
            current_string.clear();
        } else {
            if (current_string.size() >= min_length) {
                strings.push_back({start_index, current_string});
            }
            current_string.clear();
        }
    }
    if (current_string.size() >= min_length) {
        current_string.push_back(0x00);
        strings.push_back({start_index, current_string});
    }
    return strings;
}

std::unordered_map<uint8_t, std::string> instructions = {
    {0x00, "NOP"}, {0x01, "LD BC,nn"}, {0x02, "LD (BC),A"}, {0x03, "INC BC"}, {0x04, "INC B"}, {0x05, "DEC B"}, {0x06, "LD B,n"}, {0x07, "RLCA"},
    {0x08, "EX AF,AF'"}, {0x09, "ADD HL,BC"}, {0x0A, "LD A,(BC)"}, {0x0B, "DEC BC"}, {0x0C, "INC C"}, {0x0D, "DEC C"}, {0x0E, "LD C,n"}, {0x0F, "RRCA"},
    {0x08, "EX AF,AF'"}, {0x09, "ADD HL,BC"}, {0x0A, "LD A,(BC)"}, {0x0B, "DEC BC"}, {0x0C, "INC C"}, {0x0D, "DEC C"}, {0x0E, "LD C,n"}, {0x0F, "RRCA"},
    {0x10, "DJNZ e"}, {0x11, "LD DE,nn"}, {0x12, "LD (DE),A"}, {0x13, "INC DE"}, {0x14, "INC D"}, {0x15, "DEC D"}, {0x16, "LD D,n"}, {0x17, "RLA"},
    {0x18, "JR e"}, {0x19, "ADD HL,DE"}, {0x1A, "LD A,(DE)"}, {0x1B, "DEC DE"}, {0x1C, "INC E"}, {0x1D, "DEC E"}, {0x1E, "LD E,n"}, {0x1F, "RRA"},
    {0x20, "JR NZ,e"}, {0x21, "LD HL,nn"}, {0x22, "LD (nn),HL"}, {0x23, "INC HL"}, {0x24, "INC H"}, {0x25, "DEC H"}, {0x26, "LD H,n"}, {0x27, "DAA"},
    {0x28, "JR Z,e"}, {0x29, "ADD HL,HL"}, {0x2A, "LD HL,(nn)"}, {0x2B, "DEC HL"}, {0x2C, "INC L"}, {0x2D, "DEC L"}, {0x2E, "LD L,n"}, {0x2F, "CPL"},
    {0x30, "JR NC,e"}, {0x31, "LD SP,nn"}, {0x32, "LD (nn),A"}, {0x33, "INC SP"}, {0x34, "INC (HL)"}, {0x35, "DEC (HL)"}, {0x36, "LD (HL),n"}, {0x37, "SCF"},
    {0x38, "JR C,e"}, {0x39, "ADD HL,SP"}, {0x3A, "LD A,(nn)"}, {0x3B, "DEC SP"}, {0x3C, "INC A"}, {0x3D, "DEC A"}, {0x3E, "LD A,n"}, {0x3F, "CCF"},
    {0x40, "LD B,B"}, {0x41, "LD B,C"}, {0x42, "LD B,D"}, {0x43, "LD B,E"}, {0x44, "LD B,H"}, {0x45, "LD B,L"}, {0x46, "LD B,(HL)"}, {0x47, "LD B,A"},
    {0x48, "LD C,B"}, {0x49, "LD C,C"}, {0x4A, "LD C,D"}, {0x4B, "LD C,E"}, {0x4C, "LD C,H"}, {0x4D, "LD C,L"}, {0x4E, "LD C,(HL)"}, {0x4F, "LD C,A"},
    {0x50, "LD D,B"}, {0x51, "LD D,C"}, {0x52, "LD D,D"}, {0x53, "LD D,E"}, {0x54, "LD D,H"}, {0x55, "LD D,L"}, {0x56, "LD D,(HL)"}, {0x57, "LD D,A"},
    {0x58, "LD E,B"}, {0x59, "LD E,C"}, {0x5A, "LD E,D"}, {0x5B, "LD E,E"}, {0x5C, "LD E,H"}, {0x5D, "LD E,L"}, {0x5E, "LD E,(HL)"}, {0x5F, "LD E,A"},
    {0x60, "LD H,B"}, {0x61, "LD H,C"}, {0x62, "LD H,D"}, {0x63, "LD H,E"}, {0x64, "LD H,H"}, {0x65, "LD H,L"}, {0x66, "LD H,(HL)"}, {0x67, "LD H,A"},
    {0x68, "LD L,B"}, {0x69, "LD L,C"}, {0x6A, "LD L,D"}, {0x6B, "LD L,E"}, {0x6C, "LD L,H"}, {0x6D, "LD L,L"}, {0x6E, "LD L,(HL)"}, {0x6F, "LD L,A"},
    {0x70, "LD (HL),B"}, {0x71, "LD (HL),C"}, {0x72, "LD (HL),D"}, {0x73, "LD (HL),E"}, {0x74, "LD (HL),H"}, {0x75, "LD (HL),L"}, {0x76, "HALT"}, {0x77, "LD (HL),A"},
    {0x78, "LD A,B"}, {0x79, "LD A,C"}, {0x7A, "LD A,D"}, {0x7B, "LD A,E"}, {0x7C, "LD A,H"}, {0x7D, "LD A,L"}, {0x7E, "LD A,(HL)"}, {0x7F, "LD A,A"},
    {0x80, "ADD A,B"}, {0x81, "ADD A,C"}, {0x82, "ADD A,D"}, {0x83, "ADD A,E"}, {0x84, "ADD A,H"}, {0x85, "ADD A,L"}, {0x86, "ADD A,(HL)"}, {0x87, "ADD A,A"},
    {0x88, "ADC A,B"}, {0x89, "ADC A,C"}, {0x8A, "ADC A,D"}, {0x8B, "ADC A,E"}, {0x8C, "ADC A,H"}, {0x8D, "ADC A,L"}, {0x8E, "ADC A,(HL)"}, {0x8F, "ADC A,A"},
    {0x90, "SUB B"}, {0x91, "SUB C"}, {0x92, "SUB D"}, {0x93, "SUB E"}, {0x94, "SUB H"}, {0x95, "SUB L"}, {0x96, "SUB (HL)"}, {0x97, "SUB A"},
    {0x98, "SBC A,B"}, {0x99, "SBC A,C"}, {0x9A, "SBC A,D"}, {0x9B, "SBC A,E"}, {0x9C, "SBC A,H"}, {0x9D, "SBC A,L"}, {0x9E, "SBC A,(HL)"}, {0x9F, "SBC A,A"},
    {0xA0, "AND B"}, {0xA1, "AND C"}, {0xA2, "AND D"}, {0xA3, "AND E"}, {0xA4, "AND H"}, {0xA5, "AND L"}, {0xA6, "AND (HL)"}, {0xA7, "AND A"},
    {0xA8, "XOR B"}, {0xA9, "XOR C"}, {0xAA, "XOR D"}, {0xAB, "XOR E"}, {0xAC, "XOR H"}, {0xAD, "XOR L"}, {0xAE, "XOR (HL)"}, {0xAF, "XOR A"},
    {0xB0, "OR B"}, {0xB1, "OR C"}, {0xB2, "OR D"}, {0xB3, "OR E"}, {0xB4, "OR H"}, {0xB5, "OR L"}, {0xB6, "OR (HL)"}, {0xB7, "OR A"},
    {0xB8, "CP B"}, {0xB9, "CP C"}, {0xBA, "CP D"}, {0xBB, "CP E"}, {0xBC, "CP H"}, {0xBD, "CP L"}, {0xBE, "CP (HL)"}, {0xBF, "CP A"},
    {0xC0, "RET NZ"}, {0xC1, "POP BC"}, {0xC2, "JP NZ,nn"}, {0xC3, "JP nn"}, {0xC4, "CALL NZ,nn"}, {0xC5, "PUSH BC"}, {0xC6, "ADD A,n"}, {0xC7, "RST 00H"},
    {0xC8, "RET Z"}, {0xC9, "RET"}, {0xCA, "JP Z,nn"}, {0xCB, "PREFIX CB"}, {0xCC, "CALL Z,nn"}, {0xCD, "CALL nn"}, {0xCE, "ADC A,n"}, {0xCF, "RST 08H"},
    {0xD0, "RET NC"}, {0xD1, "POP DE"}, {0xD2, "JP NC,nn"}, {0xD3, "OUT (n),A"}, {0xD4, "CALL NC,nn"}, {0xD5, "PUSH DE"}, {0xD6, "SUB n"}, {0xD7, "RST 10H"},
    {0xD8, "RET C"}, {0xD9, "EXX"}, {0xDA, "JP C,nn"}, {0xDB, "IN A,(n)"}, {0xDC, "CALL C,nn"}, {0xDD, "PREFIX DD"}, {0xDE, "SBC A,n"}, {0xDF, "RST 18H"},
    {0xE0, "RET PO"}, {0xE1, "POP HL"}, {0xE2, "JP PO,nn"}, {0xE3, "EX (SP),HL"}, {0xE4, "CALL PO,nn"}, {0xE5, "PUSH HL"}, {0xE6, "AND n"}, {0xE7, "RST 20H"},
    {0xE8, "RET PE"}, {0xE9, "JP (HL)"}, {0xEA, "JP PE,nn"}, {0xEB, "EX DE,HL"}, {0xEC, "CALL PE,nn"}, {0xED, "PREFIX ED"}, {0xEE, "XOR n"}, {0xEF, "RST 28H"},
    {0xF0, "RET P"}, {0xF1, "POP AF"}, {0xF2, "JP P,nn"}, {0xF3, "DI"}, {0xF4, "CALL P,nn"}, {0xF5, "PUSH AF"}, {0xF6, "OR n"}, {0xF7, "RST 30H"},
    {0xF8, "RET M"}, {0xF9, "LD SP,HL"}, {0xFA, "JP M,nn"}, {0xFB, "EI"}, {0xFC, "CALL M,nn"}, {0xFD, "PREFIX FD"}, {0xFE, "CP n"}, {0xFF, "RST 38H"}
};

std::string decodeCBInstruction(const std::vector<uint8_t>& data, size_t pos, size_t& size) {
    std::ostringstream result; // Create a string stream to store the decoded instruction
    uint8_t opcode = data[pos]; // Correctly get the second byte after CB prefix
    size = 2; // Set the size of the instruction to 2 bytes

    switch (opcode) {
        // Rotate and Shift Instructions
        case 0x00: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": RLC B"; break;
        case 0x01: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": RLC C"; break;
        case 0x02: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": RLC D"; break;
        case 0x03: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": RLC E"; break;
        case 0x04: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": RLC H"; break;
        case 0x05: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": RLC L"; break;
        case 0x06: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": RLC (HL)"; break;
        case 0x07: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": RLC A"; break;
        case 0x10: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": RL B"; break;
        case 0x11: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": RL C"; break;
        case 0x12: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": RL D"; break;
        case 0x13: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": RL E"; break;
        case 0x14: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": RL H"; break;
        case 0x15: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": RL L"; break;
        case 0x16: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": RL (HL)"; break;
        case 0x17: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": RL A"; break;
        case 0x18: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": RR B"; break;
        case 0x19: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": RR C"; break;
        case 0x1A: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": RR D"; break;
        case 0x1B: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": RR E"; break;
        case 0x1C: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": RR H"; break;
        case 0x1D: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": RR L"; break;
        case 0x1E: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": RR (HL)"; break;
        case 0x1F: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": RR A"; break;
        //BIT Instructions
        case 0x20: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": SLA B"; break;
        case 0x21: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": SLA C"; break;
        case 0x22: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": SLA D"; break;
        case 0x23: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": SLA E"; break;
        case 0x24: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": SLA H"; break;
        case 0x25: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": SLA L"; break;
        case 0x26: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": SLA (HL)"; break;
        case 0x27: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": SLA A"; break;
        case 0x28: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": SRA B"; break;
        case 0x29: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": SRA C"; break;
        case 0x2A: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": SRA D"; break;
        case 0x2B: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": SRA E"; break;
        case 0x2C: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": SRA H"; break;
        case 0x2D: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": SRA L"; break;
        case 0x2E: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": SRA (HL)"; break;
        case 0x2F: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": SRA A"; break;
        case 0x30: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": SLL B"; break;
        case 0x31: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": SLL C"; break;
        case 0x32: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": SLL D"; break;
        case 0x33: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": SLL E"; break;
        case 0x34: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": SLL H"; break;
        case 0x35: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": SLL L"; break;
        case 0x36: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": SLL (HL)"; break;
        case 0x37: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": SLL A"; break;
        case 0x38: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": SRL B"; break;
        case 0x39: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": SRL C"; break;
        case 0x3A: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": SRL D"; break;
        case 0x3B: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": SRL E"; break;
        case 0x3C: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": SRL H"; break;
        case 0x3D: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": SRL L"; break;
        case 0x3E: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": SRL (HL)"; break;
        case 0x3F: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": SRL A"; break;
        //BIT Instructions
        case 0x40: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": BIT 0, B"; break;
        case 0x41: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": BIT 0, C"; break;
        case 0x42: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": BIT 0, D"; break;
        case 0x43: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": BIT 0, E"; break;
        case 0x44: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": BIT 0, H"; break;
        case 0x45: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": BIT 0, L"; break;
        case 0x46: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": BIT 0, (HL)"; break;
        case 0x47: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": BIT 0, A"; break;
        case 0x48: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": BIT 1, B"; break;
        case 0x49: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": BIT 1, C"; break;
        case 0x4A: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": BIT 1, D"; break;
        case 0x4B: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": BIT 1, E"; break;
        case 0x4C: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": BIT 1, H"; break;
        case 0x4D: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": BIT 1, L"; break;
        case 0x4E: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": BIT 1, (HL)"; break;
        case 0x4F: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": BIT 1, A"; break;
        case 0x50: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": BIT 2, B"; break;
        case 0x51: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": BIT 2, C"; break;
        case 0x52: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": BIT 2, D"; break;
        case 0x53: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": BIT 2, E"; break;
        case 0x54: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": BIT 2, H"; break;
        case 0x55: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": BIT 2, L"; break;
        case 0x56: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": BIT 2, (HL)"; break;
        case 0x57: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": BIT 2, A"; break;
        case 0x58: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": BIT 3, B"; break;
        case 0x59: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": BIT 3, C"; break;
        case 0x5A: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": BIT 3, D"; break;
        case 0x5B: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": BIT 3, E"; break;
        case 0x5C: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": BIT 3, H"; break;
        case 0x5D: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": BIT 3, L"; break;
        case 0x5E: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": BIT 3, (HL)"; break;
        case 0x5F: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": BIT 3, A"; break;
        case 0x60: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": BIT 4, B"; break;
        case 0x61: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": BIT 4, C"; break;
        case 0x62: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": BIT 4, D"; break;
        case 0x63: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": BIT 4, E"; break;
        case 0x64: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": BIT 4, H"; break;
        case 0x65: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": BIT 4, L"; break;
        case 0x66: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": BIT 4, (HL)"; break;
        case 0x67: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": BIT 4, A"; break;
        case 0x68: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": BIT 5, B"; break;
        case 0x69: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": BIT 5, C"; break;
        case 0x6A: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": BIT 5, D"; break;
        case 0x6B: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": BIT 5, E"; break;
        case 0x6C: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": BIT 5, H"; break;
        case 0x6D: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": BIT 5, L"; break;
        case 0x6E: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": BIT 5, (HL)"; break;
        case 0x6F: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": BIT 5, A"; break;
        case 0x70: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": BIT 6, B"; break;
        case 0x71: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": BIT 6, C"; break;
        case 0x72: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": BIT 6, D"; break;
        case 0x73: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": BIT 6, E"; break;
        case 0x74: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": BIT 6, H"; break;
        case 0x75: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": BIT 6, L"; break;
        case 0x76: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": BIT 6, (HL)"; break;
        case 0x77: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": BIT 6, A"; break;
        case 0x78: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": BIT 7, B"; break;
        case 0x79: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": BIT 7, C"; break;
        case 0x7A: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": BIT 7, D"; break;
        case 0x7B: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": BIT 7, E"; break;
        case 0x7C: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": BIT 7, H"; break;
        case 0x7D: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": BIT 7, L"; break;
        case 0x7E: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": BIT 7, (HL)"; break;
        case 0x7F: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": BIT 7, A"; break;
        //RES Instructions
        case 0x80: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": RES 0, B"; break;
        case 0x81: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": RES 0, C"; break;
        case 0x82: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": RES 0, D"; break;
        case 0x83: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": RES 0, E"; break;
        case 0x84: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": RES 0, H"; break;
        case 0x85: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": RES 0, L"; break;
        case 0x86: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": RES 0, (HL)"; break;
        case 0x87: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": RES 0, A"; break;
        case 0x88: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": RES 1, B"; break;
        case 0x89: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": RES 1, C"; break;
        case 0x8A: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": RES 1, D"; break;
        case 0x8B: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": RES 1, E"; break;
        case 0x8C: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": RES 1, H"; break;
        case 0x8D: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": RES 1, L"; break;
        case 0x8E: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": RES 1, (HL)"; break;
        case 0x8F: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": RES 1, A"; break;
        case 0x90: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": RES 2, B"; break;
        case 0x91: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": RES 2, C"; break;
        case 0x92: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": RES 2, D"; break;
        case 0x93: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": RES 2, E"; break;
        case 0x94: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": RES 2, H"; break;
        case 0x95: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": RES 2, L"; break;
        case 0x96: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": RES 2, (HL)"; break;
        case 0x97: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": RES 2, A"; break;
        case 0x98: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": RES 3, B"; break;
        case 0x99: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": RES 3, C"; break;
        case 0x9A: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": RES 3, D"; break;
        case 0x9B: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": RES 3, E"; break;
        case 0x9C: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": RES 3, H"; break;
        case 0x9D: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": RES 3, L"; break;
        case 0x9E: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": RES 3, (HL)"; break;
        case 0x9F: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": RES 3, A"; break;
        case 0xA0: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": RES 4, B"; break;
        case 0xA1: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": RES 4, C"; break;
        case 0xA2: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": RES 4, D"; break;
        case 0xA3: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": RES 4, E"; break;
        case 0xA4: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": RES 4, H"; break;
        case 0xA5: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": RES 4, L"; break;
        case 0xA6: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": RES 4, (HL)"; break;
        case 0xA7: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": RES 4, A"; break;
        case 0xA8: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": RES 5, B"; break;
        case 0xA9: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": RES 5, C"; break;
        case 0xAA: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": RES 5, D"; break;
        case 0xAB: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": RES 5, E"; break;
        case 0xAC: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": RES 5, H"; break;
        case 0xAD: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": RES 5, L"; break;
        case 0xAE: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": RES 5, (HL)"; break;
        case 0xAF: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": RES 5, A"; break;
        case 0xB0: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": RES 6, B"; break;
        case 0xB1: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": RES 6, C"; break;
        case 0xB2: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": RES 6, D"; break;
        case 0xB3: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": RES 6, E"; break;
        case 0xB4: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": RES 6, H"; break;
        case 0xB5: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": RES 6, L"; break;
        case 0xB6: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": RES 6, (HL)"; break;
        case 0xB7: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": RES 6, A"; break;
        case 0xB8: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": RES 7, B"; break;
        case 0xB9: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": RES 7, C"; break;
        case 0xBA: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": RES 7, D"; break;
        case 0xBB: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": RES 7, E"; break;
        case 0xBC: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": RES 7, H"; break;
        case 0xBD: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": RES 7, L"; break;
        case 0xBE: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": RES 7, (HL)"; break;
        case 0xBF: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": RES 7, A"; break;
        //SET Instructions
        case 0xC0: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": SET 0, B"; break;
        case 0xC1: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": SET 0, C"; break;
        case 0xC2: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": SET 0, D"; break;
        case 0xC3: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": SET 0, E"; break;
        case 0xC4: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": SET 0, H"; break;
        case 0xC5: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": SET 0, L"; break;
        case 0xC6: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": SET 0, (HL)"; break;
        case 0xC7: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": SET 0, A"; break;
        case 0xC8: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": SET 1, B"; break;
        case 0xC9: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": SET 1, C"; break;
        case 0xCA: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": SET 1, D"; break;
        case 0xCB: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": SET 1, E"; break;
        case 0xCC: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": SET 1, H"; break;
        case 0xCD: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": SET 1, L"; break;
        case 0xCE: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": SET 1, (HL)"; break;
        case 0xCF: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": SET 1, A"; break;
        case 0xD0: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": SET 2, B"; break;
        case 0xD1: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": SET 2, C"; break;
        case 0xD2: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": SET 2, D"; break;
        case 0xD3: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": SET 2, E"; break;
        case 0xD4: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": SET 2, H"; break;
        case 0xD5: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": SET 2, L"; break;
        case 0xD6: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": SET 2, (HL)"; break;
        case 0xD7: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": SET 2, A"; break;
        case 0xD8: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": SET 3, B"; break;
        case 0xD9: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": SET 3, C"; break;
        case 0xDA: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": SET 3, D"; break;
        case 0xDB: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": SET 3, E"; break;
        case 0xDC: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": SET 3, H"; break;
        case 0xDD: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": SET 3, L"; break;
        case 0xDE: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": SET 3, (HL)"; break;
        case 0xDF: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": SET 3, A"; break;
        case 0xE0: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": SET 4, B"; break;
        case 0xE1: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": SET 4, C"; break;
        case 0xE2: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": SET 4, D"; break;
        case 0xE3: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": SET 4, E"; break;
        case 0xE4: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": SET 4, H"; break;
        case 0xE5: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": SET 4, L"; break;
        case 0xE6: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": SET 4, (HL)"; break;
        case 0xE7: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": SET 4, A"; break;
        case 0xE8: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": SET 5, B"; break;
        case 0xE9: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": SET 5, C"; break;
        case 0xEA: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": SET 5, D"; break;
        case 0xEB: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": SET 5, E"; break;
        case 0xEC: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": SET 5, H"; break;
        case 0xED: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": SET 5, L"; break;
        case 0xEE: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": SET 5, (HL)"; break;
        case 0xEF: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": SET 5, A"; break;
        case 0xF0: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": SET 6, B"; break;
        case 0xF1: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": SET 6, C"; break;
        case 0xF2: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": SET 6, D"; break;
        case 0xF3: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": SET 6, E"; break;
        case 0xF4: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": SET 6, H"; break;
        case 0xF5: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": SET 6, L"; break;
        case 0xF6: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": SET 6, (HL)"; break;
        case 0xF7: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": SET 6, A"; break;
        case 0xF8: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": SET 7, B"; break;
        case 0xF9: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": SET 7, C"; break;
        case 0xFA: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": SET 7, D"; break;
        case 0xFB: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": SET 7, E"; break;
        case 0xFC: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": SET 7, H"; break;
        case 0xFD: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": SET 7, L"; break;
        case 0xFE: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": SET 7, (HL)"; break;
        case 0xFF: result << std::hex << std::setw(4) << std::setfill('0') << pos << ": SET 7, A"; break;
        default:
            result << std::hex << std::setw(4) << std::setfill('0') << pos << ": DB 0xCB, 0x" << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(opcode) << "\n";
            break;
    }

    return result.str();
}
std::string decode_instruction(const std::vector<uint8_t>& data, size_t pos, size_t& size) {
    uint8_t op = data[pos];
    std::ostringstream result;

    if (instructions.find(op) != instructions.end()) {
        std::string ins = instructions[op];
        size = 1;

        // Instructions with 16-bit arguments
        if (op == 0x01 || op == 0x11 || op == 0x21 || op == 0x2A || op == 0x31 || op == 0x32 || op == 0x3A || op == 0xC2 || op == 0xCA || op == 0xCC || op == 0xD2 || op == 0xD4 || op == 0xDA || op == 0xE2 || op == 0xEA || op == 0xF2 || op == 0xF4 || op == 0xFA || op == 0xC3 || op == 0xCD) {
            uint16_t nn = data[pos + 1] + (data[pos + 2] << 8);
            result << ins.substr(0, ins.find("nn")) << "0x" << std::hex << std::setw(4) << std::setfill('0') << nn << ins.substr(ins.find("nn") + 2);
            size = 3;
        }
        // Instructions with 8-bit arguments
        else if (op == 0x06 || op == 0x0E || op == 0x16 || op == 0x1E || op == 0x26 || op == 0x2E || op == 0x36 || op == 0x3E || op == 0xC6 || op == 0xCE || op == 0xD3 || op == 0xD6 || op == 0xDB || op == 0xDE || op == 0xE6 || op == 0xEE || op == 0xF6 || op == 0xFE) {
            uint8_t n = data[pos + 1];
            result << ins.substr(0, ins.find("n")) << "0x" << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(n) << ins.substr(ins.find("n") + 1);
            size = 2;
        }
        // Relative jumps with 8-bit signed offset
        else if (op == 0x18 || op == 0x20 || op == 0x28 || op == 0x30 || op == 0x38 || op == 0x10) {
            int8_t offset = static_cast<int8_t>(data[pos + 1]);
            uint16_t target = pos + 2 + offset;
            result << ins.substr(0, ins.find("e")) << "0x" << std::hex << std::setw(4) << std::setfill('0') << target << ins.substr(ins.find("e") + 1);
             size = 2;
         // Handle PREFIX instructions
        } else if (op == 0xDD || op == 0xFD) {
            size_t prefix_size;
            std::string prefix_ins = decode_instruction(data, pos + 1, prefix_size);
            if (op == 0xDD) {
                // Replace HL with IX
                size_t hl_pos = prefix_ins.find("HL");
                if (hl_pos != std::string::npos) {
                    prefix_ins.replace(hl_pos, 2, "IX");
                }
            } else if (op == 0xFD) {
                // Replace HL with IY
                size_t hl_pos = prefix_ins.find("HL");
                if (hl_pos != std::string::npos) {
                    prefix_ins.replace(hl_pos, 2, "IY");
                }
            }
            result << prefix_ins;
            size = 1 + prefix_size;
        } else if (op == 0xCB) {
             result << decodeCBInstruction(data, pos + 1, size);
        } else if (op == 0xED) {
            
            uint8_t edOpcode = data[pos + 1];
            std::string edInstruction = decodeEDOpcode(edOpcode);
            result << edInstruction;
            size = 2; // Default size for ED prefixed instructions
    

            // Check if the instruction has additional operands
            if (edOpcode == 0x43 || edOpcode == 0x4B || edOpcode == 0x53 || edOpcode == 0x5B || edOpcode == 0x63 || edOpcode == 0x6B || edOpcode == 0x73 || edOpcode == 0x7B) {
                uint16_t nn = data[pos + 2] + (data[pos + 3] << 8);
                result << " 0x" << std::hex << std::setw(4) << std::setfill('0') << nn;
                size = 4;
            }            
        } else {
            result << ins;
        }
    } else {
        result << "NOP   ; unknown instruction";
        size = 1;
    }

    return result.str();
}

std::string convertToAssembler(const std::vector<uint8_t>& data, size_t org = 0) {
    size_t pos = 0;
    size_t end = data.size();
    std::ostringstream result;

   // auto strings = detect_strings(data);
    auto strings = detect_strings(data);


    while (pos < end) {
        size_t size;

          
        // Check if the current position is the start of a string
        auto string_at_pos = std::find_if(strings.begin(), strings.end(), [pos](const std::pair<size_t, std::vector<uint8_t>>& s) {
            return s.first == pos;
        });

        if (string_at_pos != strings.end()) {
            size_t start_index = string_at_pos->first;
            const std::vector<uint8_t>& string_bytes = string_at_pos->second;
            std::string string_label = "string_" + std::to_string(start_index + org);
            std::string string_content;
            bool is_null_terminated = false;

            for (size_t i = 0; i < string_bytes.size(); ++i) {
                uint8_t byte = string_bytes[i];
                if (byte == 0x0A) {
                    string_content += "\\n";
                } else if (byte == 0x0D) {
                    string_content += "\\r";
                } else if (byte == 0x00) {
                    is_null_terminated = true;
                    break;
                } else {
                    string_content += static_cast<char>(byte);
                }
            }

            result << string_label << ": defm \"" << string_content << "\"";
            if (is_null_terminated) {
                result << ", 0";
            }
            result << "\n";

            pos += string_bytes.size();
            continue;
        }

        std::string ins = decode_instruction(data, pos, size);

        // Print the instruction
        result << ins;

        // Align the comment
        char buffer[60];
        snprintf(buffer, sizeof(buffer), "%-*s", 20 - static_cast<int>(ins.length()), " ");
        result << buffer;

        // Add the comment with the original address and description
        snprintf(buffer, sizeof(buffer), "; %04x :", static_cast<int>(org + pos));
        result << buffer;


        for (size_t x = 0; x < size; ++x) {
            snprintf(buffer, sizeof(buffer), "%02X ", data[pos + x]);
            result << buffer;
        }
       
        result << "\n";
        pos += size;
    }

    return result.str();
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <filename>\n";
        return 1;
    }

    std::string filename = argv[1];
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "File not found: " << filename << "\n";
        return 1;
    }

    std::vector<uint8_t> data((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    std::cout << convertToAssembler(data, 0x0000);

    return 0;
}