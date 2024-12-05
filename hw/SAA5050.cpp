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
    if (address >= startAddress && address <= endAddress) {
        // Call render function or handle memory changes
        render();
    }
    SDL_RenderPresent(renderer);
}

void SAA5050::render() {
    // Render implementation
}

int runEmulator() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("SAA5050 Emulator", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, SDL_WINDOW_SHOWN);
    if (window == nullptr) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    SAA5050 saa5050(window);

    // Example memory and address
    uint8_t memory[65536] = {0};
    uint16_t address = 0x8000;

    saa5050.checkAddressSpace(address, memory);

    SDL_Delay(2000); // Keep window open for 2 seconds

    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
