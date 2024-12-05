#include <SDL2/SDL.h>
#include <iostream>
#include "SAA5050.hxx"

SAA5050::SAA5050(SDL_Window* window) {
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr) {
        std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        exit(1);
    }
}

SAA5050::~SAA5050() {
    SDL_DestroyRenderer(renderer);
}

void SAA5050::checkAddressSpace(uint16_t address, uint8_t* memory) {
    // Get the window size
    int windowWidth, windowHeight;
    if (SDL_GetRendererOutputSize(renderer, &windowWidth, &windowHeight) != 0) {
        std::cerr << "Failed to get renderer output size! SDL_Error: " << SDL_GetError() << std::endl;
        return;
    }

    // Calculate the scale factor
    int scaleX = windowWidth / (40 * 6); // 40 characters per line, each character is 5 pixels wide + 1 pixel space
    int scaleY = windowHeight / (24 * 10); // 24 lines, each character is 9 pixels high + 1 pixel space
    int scale = std::min(scaleX, scaleY);

    if (address >= startAddress && address <= endAddress) {
        // Call render function or handle memory changes
        for (int j = 0; j < 24; ++j) { // 24 lines of characters
            for (int i = 0; i < 40; ++i) { // 40 characters per line
                uint8_t value = memory[address + i + j * 40];
                renderCharacter(value, i * 6 * scale, j * 10 * scale, scale); // Each character is 5 pixels wide + 1 pixel space, 9 pixels high + 1 pixel space
            }
        }
    }
    SDL_RenderPresent(renderer);
}

void SAA5050::renderCharacter(uint8_t value, int x, int y, int scale) {
    // Define a simple 5x9 font for characters 0-9 and A-F
    uint8_t font[16][9] = {
        {0b01110, 0b10001, 0b10011, 0b10101, 0b11001, 0b10001, 0b01110, 0b00000, 0b00000}, // 0
        {0b00100, 0b01100, 0b00100, 0b00100, 0b00100, 0b00100, 0b01110, 0b00000, 0b00000}, // 1
        {0b01110, 0b10001, 0b00001, 0b00110, 0b01000, 0b10000, 0b11111, 0b00000, 0b00000}, // 2
        {0b01110, 0b10001, 0b00001, 0b00110, 0b00001, 0b10001, 0b01110, 0b00000, 0b00000}, // 3
        {0b00010, 0b00110, 0b01010, 0b10010, 0b11111, 0b00010, 0b00010, 0b00000, 0b00000}, // 4
        {0b11111, 0b10000, 0b11110, 0b00001, 0b00001, 0b10001, 0b01110, 0b00000, 0b00000}, // 5
        {0b01110, 0b10001, 0b10000, 0b11110, 0b10001, 0b10001, 0b01110, 0b00000, 0b00000}, // 6
        {0b11111, 0b00001, 0b00010, 0b00100, 0b01000, 0b10000, 0b10000, 0b00000, 0b00000}, // 7
        {0b01110, 0b10001, 0b10001, 0b01110, 0b10001, 0b10001, 0b01110, 0b00000, 0b00000}, // 8
        {0b01110, 0b10001, 0b10001, 0b01111, 0b00001, 0b10001, 0b01110, 0b00000, 0b00000}, // 9
        {0b01110, 0b10001, 0b10001, 0b11111, 0b10001, 0b10001, 0b10001, 0b00000, 0b00000}, // A
        {0b11110, 0b10001, 0b10001, 0b11110, 0b10001, 0b10001, 0b11110, 0b00000, 0b00000}, // B
        {0b01110, 0b10001, 0b10000, 0b10000, 0b10000, 0b10001, 0b01110, 0b00000, 0b00000}, // C
        {0b11110, 0b10001, 0b10001, 0b10001, 0b10001, 0b10001, 0b11110, 0b00000, 0b00000}, // D
        {0b11111, 0b10000, 0b10000, 0b11110, 0b10000, 0b10000, 0b11111, 0b00000, 0b00000}, // E
        {0b11111, 0b10000, 0b10000, 0b11110, 0b10000, 0b10000, 0b10000, 0b00000, 0b00000}  // F
    };

    // Set the draw color to black (background)
    //SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

    // Set the draw color to white (foreground)
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    // Draw the character to the screen with scaling
    for (int row = 0; row < 9; ++row) {
        for (int col = 0; col < 5; ++col) {
            if (font[value & 0x0F][row] & (1 << (4 - col))) {
                for (int dx = 0; dx < scale; ++dx) {
                    for (int dy = 0; dy < scale; ++dy) {
                        SDL_RenderDrawPoint(renderer, x + col * scale + dx, y + row * scale + dy);
                    }
                }
            }
        }
    }
}

void runEmulator() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        
    }
    int scale = 3;
     int charWidth = 5 + 1; // 5 pixels wide + 1 pixel space
    int charHeight = 9 + 1; // 9 pixels high + 1 pixel space
    int windowWidth = 40 * charWidth * scale;
    int windowHeight = 24 * charHeight * scale;

    SDL_Window* window = SDL_CreateWindow("SAA5050 Emulator", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, windowWidth, windowHeight, SDL_WINDOW_SHOWN);
    if (window == nullptr) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return;
    }


    SAA5050 saa5050(window);

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

    // Set a bunch more memory locations to 0xFF
    for (uint16_t i = 0x800b; i < 0x8100; ++i) {
        memory[i] = i;
    }

    bool running = true;
    SDL_Event event;
    while (running) {
        saa5050.checkAddressSpace(address, memory);
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            } else if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_q) {
                    running = false;
                }
            }
        }
    }

    SDL_DestroyWindow(window);
    SDL_Quit();
}

