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

void gui::drawHexCharacter(char value, int x, int y, int scale) {
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
    
    //first byte
    for (int row = 0; row < 9; ++row) {
        for (int col = 0; col < 5; ++col) {
            if (font[(value >> 4) & 0x0F][row] & (1 << (4 - col))) {
                for (int dx = 0; dx < scale; ++dx) {
                    for (int dy = 0; dy < scale; ++dy) {
                        drawPoint(x + col * scale + dx, y + row * scale + dy);
                    }
                }
            }
        }
    }
    //second byte
    for (int row = 0; row < 9; ++row) {
        for (int col = 0; col < 5; ++col) {
            if (font[value & 0x0F][row] & (1 << (4 - col))) {
                for (int dx = 0; dx < scale; ++dx) {
                    for (int dy = 0; dy < scale; ++dy) {
                        drawPoint(x + 6 * scale + col * scale + dx, y + row * scale + dy);
                    }
                }
            }
        }
    }
}

void gui::drawAsciiCharacter(char value, int x, int y, int scale) {
    // Assuming 'font' is a predefined 2D array containing character bitmaps
    uint8_t font[128][9] = {
        // Define bitmaps for ASCII characters here
        // For simplicity, only a few characters are defined

        {0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000}, // 0x00
        {0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000}, // 0x01
        {0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000}, // 0x02
        {0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000}, // 0x03
        {0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000}, // 0x04
        {0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000}, // 0x05
        {0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000}, // 0x06
        {0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000}, // 0x07
        {0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000}, // 0x08
        {0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000}, // 0x09
        {0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000}, // 0x0A
        {0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000}, // 0x0B
        {0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000}, // 0x0C
        {0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000}, // 0x0D
        {0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000}, // 0x0E
        {0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000}, // 0x0F
        {0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000}, // 0x10
        {0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000}, // 0x11
        {0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000}, // 0x12
        {0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000}, // 0x13
        {0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000}, // 0x14
        {0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000}, // 0x15
        {0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000}, // 0x16
        {0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000}, // 0x17
        {0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000}, // 0x18
        {0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000}, // 0x19
        {0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000}, // 0x1A
        {0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000}, // 0x1B
        {0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000}, // 0x1C
        {0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000}, // 0x1D
        {0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000}, // 0x1E
        {0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000}, // 0x1F
        {0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000}, // Space
        {0b00100, 0b00100, 0b00100, 0b00100, 0b00100, 0b00000, 0b00100, 0b00000, 0b00000}, // !
        {0b01010, 0b01010, 0b01010, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000}, // "
        {0b01010, 0b11110, 0b01010, 0b01010, 0b11110, 0b01010, 0b00000, 0b00000, 0b00000}, // #
        {0b00100, 0b01110, 0b10100, 0b01110, 0b00101, 0b01110, 0b00100, 0b00000, 0b00000}, // $
        {0b11001, 0b11010, 0b00100, 0b01000, 0b10011, 0b10011, 0b00000, 0b00000, 0b00000}, // %
        {0b01100, 0b10010, 0b10100, 0b01000, 0b10101, 0b10010, 0b01101, 0b00000, 0b00000}, // &
        {0b00100, 0b00100, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000}, // '
        {0b00010, 0b00100, 0b01000, 0b01000, 0b01000, 0b00100, 0b00010, 0b00000, 0b00000}, // (
        {0b01000, 0b00100, 0b00010, 0b00010, 0b00010, 0b00100, 0b01000, 0b00000, 0b00000}, // )
        {0b00000, 0b00000, 0b01010, 0b01010, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000}, // *
        {0b00000, 0b00000, 0b00000, 0b00100, 0b00000, 0b11111, 0b00000, 0b00100, 0b00000}, // +
        {0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00100, 0b01000}, // ,
        {0b00000, 0b00000, 0b00000, 0b00000, 0b11111, 0b00000, 0b00000, 0b00000, 0b00000}, // -
        {0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b01000}, // .
        {0b00000, 0b00000, 0b00000, 0b01000, 0b10100, 0b00000, 0b00000, 0b00000, 0b00000}, // /
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
        {0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000}, // :
        {0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000}, // ;
        {0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000}, // <
        {0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000}, // =
        {0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000}, // >
        {0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000}, // ?
        {0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000}, // @
        {0b01110, 0b10001, 0b10001, 0b11111, 0b10001, 0b10001, 0b10001, 0b00000, 0b00000}, // A
        {0b11110, 0b10001, 0b10001, 0b11110, 0b10001, 0b10001, 0b11110, 0b00000, 0b00000}, // B
        {0b01110, 0b10001, 0b10000, 0b10000, 0b10000, 0b10001, 0b01110, 0b00000, 0b00000}, // C
        {0b11110, 0b10001, 0b10001, 0b10001, 0b10001, 0b10001, 0b11110, 0b00000, 0b00000}, // D
        {0b11111, 0b10000, 0b10000, 0b11110, 0b10000, 0b10000, 0b11111, 0b00000, 0b00000}, // E
        {0b11111, 0b10000, 0b10000, 0b11110, 0b10000, 0b10000, 0b10000, 0b00000, 0b00000}, // F
        {0b01110, 0b10001, 0b10000, 0b10111, 0b10001, 0b10001, 0b01110, 0b00000, 0b00000}, // G
        {0b10001, 0b10001, 0b10001, 0b11111, 0b10001, 0b10001, 0b10001, 0b00000, 0b00000}, // H
        {0b01110, 0b00100, 0b00100, 0b00100, 0b00100, 0b00100, 0b01110, 0b00000, 0b00000}, // I
        {0b00001, 0b00001, 0b00001, 0b00001, 0b00001, 0b10001, 0b01110, 0b00000, 0b00000}, // J
        {0b10001, 0b10010, 0b10100, 0b11000, 0b10100, 0b10010, 0b10001, 0b00000, 0b00000}, // K
        {0b10000, 0b10000, 0b10000, 0b10000, 0b10000, 0b10000, 0b11111, 0b00000, 0b00000}, // L
        {0b10001, 0b11011, 0b10101, 0b10101, 0b10001, 0b10001, 0b10001, 0b00000, 0b00000}, // M
        {0b10001, 0b11001, 0b10101, 0b10011, 0b10001, 0b10001, 0b10001, 0b00000, 0b00000}, // N
        {0b01110, 0b10001, 0b10001, 0b10001, 0b10001, 0b10001, 0b01110, 0b00000, 0b00000}, // O
        {0b11110, 0b10001, 0b10001, 0b11110, 0b10000, 0b10000, 0b10000, 0b00000, 0b00000}, // P
        {0b01110, 0b10001, 0b10001, 0b10001, 0b10101, 0b10010, 0b01101, 0b00000, 0b00000}, // Q
        {0b11110, 0b10001, 0b10001, 0b11110, 0b10100, 0b10010, 0b10001, 0b00000, 0b00000}, // R
        {0b01110, 0b10001, 0b10000, 0b01110, 0b00001, 0b10001, 0b01110, 0b00000, 0b00000}, // S
        {0b11111, 0b00100, 0b00100, 0b00100, 0b00100, 0b00100, 0b00100, 0b00000, 0b00000}, // T
        {0b10001, 0b10001, 0b10001, 0b10001, 0b10001, 0b10001, 0b01110, 0b00000, 0b00000}, // U
        {0b10001, 0b10001, 0b10001, 0b10001, 0b10001, 0b01010, 0b00100, 0b00000, 0b00000}, // V
        {0b10001, 0b10001, 0b10001, 0b10101, 0b10101, 0b11011, 0b10001, 0b00000, 0b00000}, // W
        {0b10001, 0b10001, 0b01010, 0b00100, 0b01010, 0b10001, 0b10001, 0b00000, 0b00000}, // X
        {0b10001, 0b10001, 0b10001, 0b01010, 0b00100, 0b00100, 0b00100, 0b00000, 0b00000}, // Y
        {0b11111, 0b00001, 0b00010, 0b00100, 0b01000, 0b10000, 0b11111, 0b00000, 0b00000}, // Z
        {0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000}, // [
        {0b10000, 0b10000, 0b01000, 0b00100, 0b00010, 0b00001, 0b00001, 0b00000, 0b00000}, // backslash
        {0b00000, 0b01100, 0b00100, 0b00010, 0b00100, 0b00100, 0b00100, 0b01100, 0b00000}, // ]
        {0b00100, 0b01010, 0b10001, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000}, // ^
        {0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b11110, 0b00000}, // _
        {0b00100, 0b00100, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000}, // `
        {0b00000, 0b00000, 0b01110, 0b00001, 0b01111, 0b10001, 0b01111, 0b00000, 0b00000}, // a
        {0b10000, 0b10000, 0b11110, 0b10001, 0b10001, 0b10001, 0b11110, 0b00000, 0b00000}, // b
        {0b00000, 0b00000, 0b01110, 0b10001, 0b10000, 0b10001, 0b01110, 0b00000, 0b00000}, // c
        {0b00001, 0b00001, 0b01111, 0b10001, 0b10001, 0b10001, 0b01111, 0b00000, 0b00000}, // d
        {0b00000, 0b00000, 0b01110, 0b10001, 0b11111, 0b10000, 0b01110, 0b00000, 0b00000}, // e
        {0b00110, 0b01001, 0b01000, 0b11100, 0b01000, 0b01000, 0b01000, 0b00000, 0b00000}, // f
        {0b00000, 0b00000, 0b01111, 0b10001, 0b10001, 0b01111, 0b00001, 0b01110, 0b00000}, // g
        {0b10000, 0b10000, 0b11110, 0b10001, 0b10001, 0b10001, 0b10001, 0b00000, 0b00000}, // h
        {0b00100, 0b00000, 0b01100, 0b00100, 0b00100, 0b00100, 0b01110, 0b00000, 0b00000}, // i
        {0b00010, 0b00000, 0b00110, 0b00010, 0b00010, 0b00010, 0b10010, 0b01100, 0b00000}, // j
        {0b10000, 0b10000, 0b10010, 0b10100, 0b11000, 0b10100, 0b10010, 0b00000, 0b00000}, // k
        {0b01100, 0b00100, 0b00100, 0b00100, 0b00100, 0b00100, 0b01110, 0b00000, 0b00000}, // l
        {0b00000, 0b00000, 0b11010, 0b10101, 0b10101, 0b10101, 0b10101, 0b00000, 0b00000}, // m
        {0b00000, 0b00000, 0b11110, 0b10001, 0b10001, 0b10001, 0b10001, 0b00000, 0b00000}, // n
        {0b00000, 0b00000, 0b01110, 0b10001, 0b10001, 0b10001, 0b01110, 0b00000, 0b00000}, // o
        {0b00000, 0b00000, 0b11110, 0b10001, 0b10001, 0b11110, 0b10000, 0b10000, 0b00000}, // p
        {0b00000, 0b00000, 0b01111, 0b10001, 0b10001, 0b01111, 0b00001, 0b00001, 0b00000}, // q
        {0b00000, 0b00000, 0b10110, 0b11001, 0b10000, 0b10000, 0b10000, 0b00000, 0b00000}, // r
        {0b00000, 0b00000, 0b01111, 0b10000, 0b01110, 0b00001, 0b11110, 0b00000, 0b00000}, // s
        {0b01000, 0b01000, 0b11100, 0b01000, 0b01000, 0b01001, 0b00110, 0b00000, 0b00000}, // t
        {0b00000, 0b00000, 0b10001, 0b10001, 0b10001, 0b10001, 0b01111, 0b00000, 0b00000}, // u
        {0b00000, 0b00000, 0b10001, 0b10001, 0b10001, 0b01010, 0b00100, 0b00000, 0b00000}, // v
        {0b00000, 0b00000, 0b10001, 0b10001, 0b10101, 0b10101, 0b01010, 0b00000, 0b00000}, // w
        {0b00000, 0b00000, 0b10001, 0b01010, 0b00100, 0b01010, 0b10001, 0b00000, 0b00000}, // x
        {0b00000, 0b00000, 0b10001, 0b10001, 0b10001, 0b01111, 0b00001, 0b01110, 0b00000}, // y
        {0b00000, 0b00000, 0b11111, 0b00010, 0b00100, 0b01000, 0b11111, 0b00000, 0b00000} // z
    };
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    for (int row = 0; row < 9; ++row) {
        for (int col = 0; col < 5; ++col) {
            if (font[value][row] & (1 << (4 - col))) {
                for (int dx = 0; dx < scale; ++dx) {
                    for (int dy = 0; dy < scale; ++dy) {
                        drawPoint(x + col * scale + dx, y + row * scale + dy);
                    }
                }
            }
        }
    }
   /* for (int row = 0; row < 9; ++row) {
        for (int col = 0; col < 5; ++col) {
            if (font[value][row] & (1 << (4 - col))) {
                for (int dx = 0; dx < scale; ++dx) {
                    for (int dy = 0; dy < scale; ++dy) {
                        drawPoint(x + col * scale + dx, y + row * scale + dy);
                    }
                }
            }
        }
    }*/
}

void gui::checkAddressSpace(uint16_t address, char* memory, gui* guii) {
	int scale = 3;
	int charWidth = 5 + 1; // 5 pixels wide + 1 pixel space
	int charHeight = 9 + 1; // 9 pixels high + 1 pixel space
	int windowWidth = 40 * charWidth * scale;
	int windowHeight = 24 * charHeight * scale;
	// Calculate the scale factor
	int scaleX = windowWidth / (40 * 6); // 40 characters per line, each character is 5 pixels wide + 1 pixel space
	int scaleY = windowHeight / (24 * 10); // 24 lines, each character is 9 pixels high + 1 pixel space
	//int scale = std::min(scaleX, scaleY);

	//const uint16_t startAddress = 0x8000; // Example start address
	//const uint16_t endAddress = 0x87FF;   // Example end address

	//if (address >= startAddress && address <= endAddress) {
		// Call render function or handle memory changes
		for (int j = 0; j < 24; ++j) { // 24 lines of characters
			for (int i = 0; i < 40; ++i) { // 40 characters per line
				uint16_t byte = memory[address + i + j * 40];
				//guii->drawHexCharacter(memory[address + i + j * 40], i * 6 * scale, j * 10 * scale, scale); // Each character is 5 pixels wide + 1 pixel space, 9 pixels high + 1 pixel space
				// //guii->drawHexCharacter(byte, (i * 12 * scale), j * 10 * scale, scale);
                guii->drawHexCharacter((unsigned char)memory[address + i + j * 40], (i * 12 * scale), j * 10 * scale, scale);
        
				//printf("t: 0x%04X, 0x%02X\n", address + i + j * 40, (unsigned char)memory[address + i + j * 40]);
			}
		}
	//}
	//SDL_RenderPresent(guii->renderer);
}


void gui::drawMem(uint16_t address, char* memory, gui* guii) {
	int scale = 3;
	int charWidth = 5 + 1; // 5 pixels wide + 1 pixel space
	int charHeight = 9 + 1; // 9 pixels high + 1 pixel space
	int windowWidth = 40 * charWidth * scale;
	int windowHeight = 24 * charHeight * scale;
	// Calculate the scale factor
	int scaleX = windowWidth / (40 * 6); // 40 characters per line, each character is 5 pixels wide + 1 pixel space
	int scaleY = windowHeight / (24 * 10); // 24 lines, each character is 9 pixels high + 1 pixel space
	//int scale = std::min(scaleX, scaleY);

	const uint16_t startAddress = 0x8000; // Example start address
	const uint16_t endAddress = 0x87FF;   // Example end address


	int memcount = 0;
	if (address >= startAddress && address <= endAddress) {
		// Call render function or handle memory changes
		for (int j = 0; j < 24; ++j) { // 24 lines of characters
			for (int i = 0; i < 40; ++i) { // 40 characters per line
				
				char charByte1 = memory[address + memcount];
				char charByte2 = memory[address + 1 + memcount];
				char charBytes = (charByte1 << 8) | charByte2; // Combine the two bytes
				//char printable = charBytes & 0xFF; // Extract the lower byte for the character
				memcount += 1;
				//	printf("%02X ", (unsigned char)memory[address + i + j * 40]);
			
				guii->drawAsciiCharacter(charByte1, i * 6 * scale, j * 10 * scale, scale); // Each character is 5 pixels wide + 1 pixel space, 9 pixels high + 1 pixel space
				
			}
		}
	}
	//SDL_RenderPresent(guii->renderer);
}
