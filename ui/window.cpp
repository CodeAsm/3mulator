#include "window.hxx"

gui::gui(int width, int height, int scale) : scale(scale) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        exit(1);
    }

    window = SDL_CreateWindow("Emulator", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN);
    if (window == nullptr) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        exit(1);
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr) {
        std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        exit(1);
    }
}

gui::~gui() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void gui::clear() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
}

void gui::present() {
    SDL_RenderPresent(renderer);
}

void gui::drawPoint(int x, int y) {
    SDL_RenderDrawPoint(renderer, x, y);
}

void gui::drawCharacter(uint8_t value, int x, int y, int scale) {
    // Assuming 'font' is a predefined 2D array containing character bitmaps
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
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    for (int row = 0; row < 9; ++row) {
        for (int col = 0; col < 5; ++col) {
            if (font[value & 0x0F][row] & (1 << (4 - col))) {
                for (int dx = 0; dx < scale; ++dx) {
                    for (int dy = 0; dy < scale; ++dy) {
                        drawPoint(x + col * scale + dx, y + row * scale + dy);
                    }
                }
            }
        }
    }
}