#include "MySerial.h"

MySerial::MySerial()
{
    // bool _error = false;
    // if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0)
    // {
    //     fprintf(stderr, "Error SDL_Init : %s", SDL_GetError());
    //     _error = true;
    // }
    // window_ = SDL_CreateWindow("Serial Emulator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w, h, 0);
    // if (window_ == NULL)
    // {
    //     fprintf(stderr, "Error SDL_CreateWindow : %s", SDL_GetError());
    //     _error = true;
    // }
    // renderer_ = SDL_CreateRenderer(window_, -1, SDL_RENDERER_ACCELERATED);
    // if (renderer_ == NULL)
    // {
    //     fprintf(stderr, "Error SDL_CreateRenderer : %s", SDL_GetError());
    //     _error = true;
    // }
    // if (_error)
    // {
    //     if (renderer_ != NULL)
    //         SDL_DestroyRenderer(renderer_);
    //     if (window_ != NULL)
    //         SDL_DestroyWindow(window_);
    //     SDL_Quit();
    // }
    // return;
}

size_t MySerial::write(uint8_t c)
{
    std::cout << c;
    return 1;
}

void MySerial::begin(unsigned long baud)
{
    enabled = true;
}

MySerial Serial;