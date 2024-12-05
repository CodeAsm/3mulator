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
    SDL_GetRendererOutputSize(renderer, &windowWidth, &windowHeight);

    // Calculate the scale factor
    int scaleX = windowWidth / 320; // Assuming 40 characters per line and each character is 8 pixels wide
    int scaleY = windowHeight / 240; // Assuming 30 lines and each character is 8 pixels high
    int scale = std::min(scaleX, scaleY);


    if (address >= startAddress && address <= endAddress) {
        // Call render function or handle memory changes
        for (int j = 0; j < 30; ++j) { // Assuming 30 lines of characters
            for (int i = 0; i < 40; ++i) { // Assuming SAA5050 can draw 40 characters per line
            uint8_t value = memory[address + i + j * 40];
            renderCharacter(value, i * 8 * scale, j * 8 * scale, scale); // Assuming each character is 8 pixels wide and 8 pixels high
            }
        }
    }
    SDL_RenderPresent(renderer);
}

void SAA5050::renderCharacter(uint8_t value, int x, int y, int scale) {
    // Define a simple 8x8 font for characters 0-9 and A-F
    uint8_t font[16][8] = {
        {0b00111100, 0b01100110, 0b01101110, 0b01110110, 0b01100110, 0b01100110, 0b00111100, 0b00000000}, // 0
        {0b00011000, 0b00111000, 0b00011000, 0b00011000, 0b00011000, 0b00011000, 0b01111110, 0b00000000}, // 1
        {0b00111100, 0b01100110, 0b00000110, 0b00001100, 0b00110000, 0b01100000, 0b01111110, 0b00000000}, // 2
        {0b00111100, 0b01100110, 0b00000110, 0b00011100, 0b00000110, 0b01100110, 0b00111100, 0b00000000}, // 3
        {0b00001100, 0b00011100, 0b00101100, 0b01001100, 0b01111110, 0b00001100, 0b00001100, 0b00000000}, // 4
        {0b01111110, 0b01100000, 0b01111100, 0b00000110, 0b00000110, 0b01100110, 0b00111100, 0b00000000}, // 5
        {0b00111100, 0b01100110, 0b01100000, 0b01111100, 0b01100110, 0b01100110, 0b00111100, 0b00000000}, // 6
        {0b01111110, 0b01100110, 0b00001100, 0b00011000, 0b00110000, 0b00110000, 0b00110000, 0b00000000}, // 7
        {0b00111100, 0b01100110, 0b01100110, 0b00111100, 0b01100110, 0b01100110, 0b00111100, 0b00000000}, // 8
        {0b00111100, 0b01100110, 0b01100110, 0b00111110, 0b00000110, 0b01100110, 0b00111100, 0b00000000}, // 9
        {0b00111100, 0b01100110, 0b01100110, 0b01111110, 0b01100110, 0b01100110, 0b01100110, 0b00000000}, // A
        {0b01111100, 0b01100110, 0b01100110, 0b01111100, 0b01100110, 0b01100110, 0b01111100, 0b00000000}, // B
        {0b00111100, 0b01100110, 0b01100000, 0b01100000, 0b01100000, 0b01100110, 0b00111100, 0b00000000}, // C
        {0b01111100, 0b01100110, 0b01100110, 0b01100110, 0b01100110, 0b01100110, 0b01111100, 0b00000000}, // D
        {0b01111110, 0b01100000, 0b01100000, 0b01111100, 0b01100000, 0b01100000, 0b01111110, 0b00000000}, // E
        {0b01111110, 0b01100000, 0b01100000, 0b01111100, 0b01100000, 0b01100000, 0b01100000, 0b00000000}  // F
    };

    // Set the draw color to black (background)
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

    // Set the draw color to white (foreground)
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

  
    // Draw the character to the screen with scaling
    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            if (font[value & 0x0F][row] & (1 << (7 - col))) {
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
    SDL_Window* window = SDL_CreateWindow("SAA5050 Emulator", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1280, 960, SDL_WINDOW_SHOWN);
    if (window == nullptr) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
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
    memory[8008] = 0x4c;  // jmp to (0x0000)
    memory[8009] = 0x00;  // address low byte
    memory[8010] = 0x00;  // address high byte

    saa5050.checkAddressSpace(address, memory);
    std::cout << "Press Enter to continue...";
    std::cin.ignore();

    SDL_DestroyWindow(window);
    SDL_Quit();
}

