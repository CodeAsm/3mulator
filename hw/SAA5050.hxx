// SAA5050.h
#ifndef SAA5050_HXX
#define SAA5050_HXX

#include <cstdint>
#include <SDL2/SDL.h>

class SAA5050 {
public:
    SAA5050(SDL_Window* window);
    ~SAA5050();
    void checkAddressSpace(uint16_t address, uint8_t* memory);

void renderCharacter(uint8_t value, int x, int y, int scale);

private:
    SDL_Renderer* renderer;
    void render();
    const uint16_t startAddress = 0x8000; // Example start address
    const uint16_t endAddress = 0x87FF;   // Example end address
};

// Declare runEmulator() function
void runEmulator();

#endif // SAA5050_HXX
