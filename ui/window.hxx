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
    void drawCharacter(uint8_t value, int x, int y, int scale);

private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    int scale;
};

#endif // WINDOW_H