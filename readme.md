# 8bit Emu and d3com

My personal small emulation and decompilation project for smaller 8bit like cpu systems.
It probably wont work, doesnt provide what you where looking for and programmed the way you like.
Any suggestions or ideas are welcome.

## How to compile

Grab your own gcc compiler or similar and run make. If this doesnt work, RTFM of your distro on how to compile programs.
it shouldnt be too hard for now

## How to run

``./main`` should do it.
once running, a few commands should work:
C to continue
Q to quit
S to show a certain portion of memory (not implemented yet)

## Ideas

Some basic ideas I have that I might be able to achieve:

* Add more cpu types like a z80, 8052 and maybe 8086
* load roms
* GPU's for various cpu (simple first hehe)
* decompile for various roms/pieces of code
* NES (ow boy, probably buggy and non working)

But no particular order and I might migrate parts or incorperate this into other projects earlier.

## Resources and helpfull links

* <https://www.programiz.com/cpp-programming/library-function/cstdio/fscanf>
* <https://stackoverflow.com/a/28979413/1671029> had some scanf issues, this is pretty smart.
* <https://www.bellard.org/tinyemu/>

### 6502 specific

* <https://www.youtube.com/watch?v=qJgsuQoy9bc&list=PLLwK93hM93Z13TRzPx9JqTIn33feefl37>
* <https://github.com/davepoo/6502Emulator/blob/master/6502/6502Lib/src/public/m6502.h>
* <https://github.com/Klaus2m5/6502_65C02_functional_tests/blob/master/6502_functional_test.a65>
* <http://archive.6502.org/datasheets/synertek_programming_manual.pdf>
* <https://github.com/f0lg0/6502/tree/main/src/peripherals>
* <https://github.com/caseif/c6502/blob/master/src/c6502/cpu.c>
* <https://web.archive.org/web/20110113133322/http://impulzus.sch.bme.hu/6502/6502/> (use translate maybe)

#### Decoding

* <https://www.pagetable.com/?p=39>
* <http://forum.6502.org/viewtopic.php?t=1494>

### Pointers

* <https://www.w3schools.blog/how-to-print-value-of-pointer-in-c>
