#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <iomanip>
#include <sstream>
#include <unordered_map>
#include <cstdint>

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

std::string decode_instruction(const std::vector<uint8_t>& data, size_t pos, size_t& size) {
    uint8_t op = data[pos];
    std::ostringstream result;

    if (instructions.find(op) != instructions.end()) {
        std::string ins = instructions[op];
        size = 1;

        // Instructions with 16-bit arguments
        if (op == 0x01 || op == 0x11 || op == 0x21 || op == 0x31 || op == 0xC2 || op == 0xCA || op == 0xD2 || op == 0xDA || op == 0xE2 || op == 0xEA || op == 0xF2 || op == 0xFA || op == 0xC3 || op == 0xCD) {
            uint16_t nn = data[pos + 1] + (data[pos + 2] << 8);
            result << ins.substr(0, ins.find("nn")) << "0x" << std::hex << std::setw(4) << std::setfill('0') << nn << ins.substr(ins.find("nn") + 2);
            size = 3;
        }
        // Instructions with 8-bit arguments
        else if (op == 0x06 || op == 0x0E || op == 0x16 || op == 0x1E || op == 0x26 || op == 0x2E || op == 0x36 || op == 0x3E || op == 0xC6 || op == 0xCE || op == 0xD3 || op == 0xD6 || op == 0xDE || op == 0xE6 || op == 0xEE || op == 0xF6 || op == 0xFE) {
            uint8_t n = data[pos + 1];
            result << ins.substr(0, ins.find("n")) << "0x" << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(n) << ins.substr(ins.find("n") + 1);
            size = 2;
        }
        // Relative jumps with 8-bit signed offset
        else if (op == 0x18 || op == 0x20 || op == 0x28 || op == 0x30 || op == 0x38 || op == 0x10) {
            int8_t offset = static_cast<int8_t>(data[pos + 1]);
            uint16_t target = pos + 2 + offset;
            result << ins.substr(0, ins.find("e")) << "$" << std::hex << std::setw(4) << std::setfill('0') << target << ins.substr(ins.find("e") + 1);
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
            size_t prefix_size;
            std::string prefix_ins = decode_instruction(data, pos + 1, prefix_size);
            result << "CB " << prefix_ins;
            size = 1 + prefix_size;
        } else if (op == 0xED) {
            size_t prefix_size;
            std::string prefix_ins = decode_instruction(data, pos + 1, prefix_size);
            result << "ED " << prefix_ins;
            size = 1 + prefix_size;
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

    while (pos < end) {
        size_t size;
        std::string ins = decode_instruction(data, pos, size);

        // Print the instruction
        result << ins;

        // Print the arguments if any
        /*if (size > 1) {
            result << " ";
            for (size_t x = 1; x < size; ++x) {
                //result << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(data[pos + x]);
                if (x < size - 1) {
                    result << " ";
                }
            }
        }
        */
        // Print the arguments if any
        /*
        if (size > 1) {
            result << " ";
            for (size_t x = 1; x < size; ++x) {
                result << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(data[pos + x]);
                if (x < size - 1) {
                    result << " ";
                }
            }
        }*/

        // Align the comment
        char buffer[60];
        snprintf(buffer, sizeof(buffer), "%-*s", 20 - static_cast<int>(ins.length()), " ");
        result << buffer;

        // Add the comment with the original address and description
        snprintf(buffer, sizeof(buffer), "; %04x ", static_cast<int>(org + pos));
        result << buffer;

        // Add a description based on the instruction
      /*  if (ins.find("LD A,") != std::string::npos) {
            result << "loads " << std::stoi(ins.substr(6), nullptr, 16);
        } else if (ins.find("OUT (") != std::string::npos) {
            result << "A onto io " << ins.substr(5, 1);
        } else if (ins.find("XOR A") != std::string::npos) {
            result << "zero A";
        } else if (ins.find("OUT (") != std::string::npos) {
            result << "show A on " << ins.substr(5, 1);
        } else if (ins.find("LD A,") != std::string::npos) {
            result << "loads " << std::stoi(ins.substr(6), nullptr, 16);
        }*/

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