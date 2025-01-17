#ifndef WINDOW_H
#define WINDOW_H

#include <SDL2/SDL.h>
#include <iostream>

class gui {

public:

    gui(int width, int height, int scale);

    ~gui();

    void clear();
    void present();
    void drawPoint(int x, int y);
    void drawHexCharacter(char value, int x, int y, int scale);
    void drawAsciiCharacter(char value, int x, int y, int scale);
    void checkAddressSpace(uint16_t address, char* memory, gui* guii);
    void drawMem(uint16_t address, char* memory, gui* guii);

private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    int scale;
};

#endif // WINDOW_H