# Programmer notes

For writing the emulator and decompiler, I needed code snippets and test code.
Altho not complete, here are some notes I made along the way.

## test codes

$0600    a9 01     LDA #$01
$0602    8d 00 02  STA $0200
$0605    a9 05     LDA #$05
$0607    8d 01 02  STA $0201
$060a    a9 08     LDA #$08
$060c    8d 02 02  STA $0202


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

Address  Hexdump   Dissassembly
-------------------------------
$0600    a9 c0     LDA #$c0    ;Load the hex value $c0 into the A register
$0602    aa        TAX         ;Transfer the value in the A register to X
$0603    e8        INX         ;Increment the value in the X register
$0604    69 c4     ADC #$c4    ;Add the hex value $c4 to the A register
$0606    00        BRK         ;Break - we're done

In the simulator below type (don’t paste) the following code:

LDA #$80
STA $01
ADC $01




	mem.mem[1] = 0x43;
	mem.mem[2] = 's';
	mem.mem[3] = 0x45;
	mem.mem[4] = 0x46;
	mem.mem[5] = 0x47;
	mem.mem[6] = 0x48;
	mem.mem[7] = 0x49;



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



## Video

1. Monochrome Display Adapter (MDA):

    Memory Range: 0xB0000 - 0xB0FFF (4 KB)
    Text Mode: The MDA adapter only supported text mode with a resolution of 80x25 characters.
    Usage: Each character in the text mode occupied 2 bytes:
        The first byte stored the ASCII character code.
        The second byte stored the attribute (e.g., intensity, underline).

2. Color Graphics Adapter (CGA):

    Memory Range: 0xB8000 - 0xBFFFF (32 KB)
    Text Mode: CGA's text modes included 40x25 and 80x25 characters.
    Usage: Similar to MDA, text modes used 2 bytes per character:
        The first byte for the character code.
        The second byte for foreground and background color attributes.

3. Hercules Graphics Card:

    Memory Range: 0xB0000 - 0xB7FFF (32 KB)
    Modes Supported: Text and high-resolution monochrome graphics (720x348 pixels).
    Usage in Text Mode: Hercules used the same mapping as MDA for text.

4. VGA and Later:

    VGA maintained backward compatibility with CGA and MDA, keeping the 0xB8000 memory range for text modes in color and 0xB0000 for monochrome.

Example Code in Assembly (Text Mode Write):

This example writes "Hello" in white text on a black background at the top-left corner of the screen in 80x25 mode:

mov ax, 0xB800        ; Base segment for CGA text mode
mov es, ax            ; Load segment into ES
mov di, 0             ; Start at the top-left corner
mov byte [es:di], 'H' ; Character 'H'
mov byte [es:di+1], 0x0F ; White text on black background
mov byte [es:di+2], 'e'
mov byte [es:di+3], 0x0F
mov byte [es:di+4], 'l'
mov byte [es:di+5], 0x0F
mov byte [es:di+6], 'l'
mov byte [es:di+7], 0x0F
mov byte [es:di+8], 'o'
mov byte [es:di+9], 0x0F

Access in Higher-Level Languages:

In C (DOS), you can directly access these memory locations using pointers:

#include <dos.h>

void write_text() {
    unsigned char far* video_memory = (unsigned char far*)0xB8000000;
    video_memory[0] = 'H';  // Character
    video_memory[1] = 0x0F; // White text on black
}

These mappings and examples illustrate how text modes were implemented using specific video memory regions, enabling the CPU to directly control the display. Let me know if you'd like help experimenting with this in an emulator!

