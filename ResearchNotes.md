# Research notes document

This document is mostly intended for myself.

## SAA5050

https://forums.bannister.org/ubbthreads.php?ubb=showflat&Number=98898&page=2

## Capstone would be cool

But sadly not all target arch, ISA are supported

### Z80 based on a python project and copilot

For z80 ive had multiple attempts, for now, i started to dissasble romcode and make it write sourcecode that should compile.
I hope this helps make a "accurate" emulator later.

## Terminal

First ofcourse we need a somewhat true to real hardware. but then we want fancy looks:
* <https://stackoverflow.com/questions/63841904/sdl-old-crt-scanlines-without-waste-cpu-time>
or
* <>

## Terminal for my terminal?

I dunno, maybe if I need one:
* <https://github.com/timmoorhouse/imgui-terminal>

### Shaders:
* <https://www.reddit.com/r/emulation/comments/74hzx3/what_is_the_most_realistic_crt_shader_in_your/>
* <https://forums.libretro.com/t/a-better-snes-hi-res-blend-shader/1800/7>

the best?
* <http://emulation.gametechwiki.com/index.php/CRT-Royale>

## MITS Altair/Imsai 8080

Altair, i want 3 variants?:
MITS 8800B CPU BD REV 0
ALTAIR 8800B TURNKEY MODULE REV 0
MITS 16K RAM (?)

The second one:
MITS 8800B CPU BD REV 0
PCC TURNKEY MODULE
PCC S-100 INTERFACE
CEPC 4-SIO-1

third be configurable, preferably with the switches and lights.
https://forum.vcfed.org/index.php?threads/what-are-the-correct-boards-for-an-altair-turnkey.51128/
images:
http://madrona.ca/e/altair8800b/index.html

### Interface eprom
"The front-panel uses a 1702 EPROM to control a small sequencing engine for execution of front-panel operations. This EPROM turned out to be faulty. Rather than obtaining and programming another 1702, an adapter for a 2716 was constructed - the 2716 being far more prevalent and easier to program. The data to be programmed into the EPROM is fortunately available in the 8800b manual. "
(the madrona source)
I might want this, to allow custom hardware variants to test and such

## cp/m

https://bryceautomation.com/index.php/2022/12/10/compiling-cp-m-for-z80/
https://bryceautomation.com/index.php/2023/05/16/z80asm-on-imsai-8080/
http://cpuville.com/Kits/Bus-display-kit.html

## classes idea:
Yes, it is definitely possible to create a system where different CPU objects with their own memory and devices communicate with each other over a serial port or other interfaces. The object-oriented nature of C++ makes it well-suited for this kind of modular design.

Here's a high-level plan to achieve this:

1. **Define a base `Device` class**: This will be the base class for all devices (CPUs, memory, serial interfaces, etc.).
2. **Define a `Memory` class**: This will handle memory operations.
3. **Define a `SerialInterface` class**: This will handle serial communication between devices.
4. **Define a base `CPU` class**: This will be the base class for all CPU types.
5. **Define specific CPU classes (e.g., [`i8080`](command:_github.copilot.openSymbolFromReferences?%5B%22%22%2C%5B%7B%22uri%22%3A%7B%22scheme%22%3A%22file%22%2C%22authority%22%3A%22%22%2C%22path%22%3A%22%2Fhome%2Fcodeasm%2FProjects%2F8bit%2F3mulator%2Fcores%2F8080.cpp%22%2C%22query%22%3A%22%22%2C%22fragment%22%3A%22%22%7D%2C%22pos%22%3A%7B%22line%22%3A27%2C%22character%22%3A6%7D%7D%5D%2C%22f5fd85a5-8afb-43e5-b75c-8c972f3df52e%22%5D "Go to definition"), `CPU8088`, `Z80`)**: These will inherit from the base `CPU` class and implement specific CPU behavior.
6. **Connect devices**: Create instances of these classes and connect them as needed.

Here's a basic implementation to illustrate this:

### Device Class
```cpp
class Device {
public:
    virtual void cycle() = 0;
};
```

### Memory Class
```cpp
#include <vector>
#include <cstdint>

class Memory : public Device {
public:
    Memory(size_t size) : mem(size, 0) {}

    uint8_t read(uint16_t address) const {
        return mem[address];
    }

    void write(uint16_t address, uint8_t value) {
        mem[address] = value;
    }

    void cycle() override {
        // Memory-specific cycle operations (if any)
    }

private:
    std::vector<uint8_t> mem;
};
```

### Serial Interface Class
```cpp
class SerialInterface : public Device {
public:
    SerialInterface() : connectedDevice(nullptr) {}

    void connect(SerialInterface* device) {
        connectedDevice = device;
    }

    void send(uint8_t data) {
        if (connectedDevice) {
            connectedDevice->receive(data);
        }
    }

    void receive(uint8_t data) {
        // Handle received data
    }

    void cycle() override {
        // Serial interface-specific cycle operations (if any)
    }

private:
    SerialInterface* connectedDevice;
};
```

### Base CPU Class
```cpp
class CPU : public Device {
public:
    virtual void cycle() = 0;
    virtual void connectMemory(Memory* memory) {
        this->memory = memory;
    }

protected:
    Memory* memory;
};
```

### i8080 CPU Class
```cpp
class i8080 : public CPU {
public:
    static const int IO_PORTS = 256;

    i8080() : pc(0), sp(0), flags(0) {
        memset(registers, 0, sizeof(registers));
    }

    void cycle() override {
        // Implement the 8080 CPU cycle logic here
    }

private:
    uint16_t pc, sp;
    uint8_t flags;
    uint8_t registers[7];
};
```

### Main Program
```cpp
#include <iostream>

int main() {
    Memory memory1(65536); // 64KB of memory for i8080
    Memory memory2(65536); // 64KB of memory for another CPU

    i8080 cpu1;
    cpu1.connectMemory(&memory1);

    // Example of another CPU, e.g., CPU8088
    // CPU8088 cpu2;
    // cpu2.connectMemory(&memory2);

    SerialInterface sio1;
    SerialInterface sio2;
    sio1.connect(&sio2);

    // Connect sio1 to cpu1 and sio2 to cpu2 if needed
    // cpu1.connectSerial(&sio1);
    // cpu2.connectSerial(&sio2);

    // Example of writing to memory
    memory1.write(0x0000, 0x42);

    // Example of CPU cycle
    cpu1.cycle();

    // Example of reading from memory
    uint8_t value = memory1.read(0x0000);
    std::cout << "Value at 0x0000: " << static_cast<int>(value) << std::endl;

    return 0;
}
```

### Explanation
1. **Device Class**: A base class for all devices, including CPUs, memory, and serial interfaces.
2. **Memory Class**: Manages a block of memory and provides methods to read and write to it.
3. **Serial Interface Class**: Manages serial communication between devices.
4. **Base CPU Class**: Defines a virtual `cycle` method and a method to connect to memory.
5. **i8080 CPU Class**: Inherits from the base `CPU` class and implements the `cycle` method.
6. **Main Program**: Creates instances of `Memory`, [`i8080`](command:_github.copilot.openSymbolFromReferences?%5B%22%22%2C%5B%7B%22uri%22%3A%7B%22scheme%22%3A%22file%22%2C%22authority%22%3A%22%22%2C%22path%22%3A%22%2Fhome%2Fcodeasm%2FProjects%2F8bit%2F3mulator%2Fcores%2F8080.cpp%22%2C%22query%22%3A%22%22%2C%22fragment%22%3A%22%22%7D%2C%22pos%22%3A%7B%22line%22%3A27%2C%22character%22%3A6%7D%7D%5D%2C%22f5fd85a5-8afb-43e5-b75c-8c972f3df52e%22%5D "Go to definition"), and `SerialInterface`, connects them, and demonstrates basic operations.

This structure allows you to easily add other CPU types and devices, and connect them in various configurations. Each device can interact with the shared memory and communicate with other devices through the serial interface.