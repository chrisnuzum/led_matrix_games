#ifndef _myserial_
#define _myserial_

#include <cstdint>
#include "Print.h"
#include <SDL.h>
#include <iostream>

class MySerial : public Print
{
private:
    bool enabled = false;
    uint16_t w = 600;
    uint16_t h = 200;
    SDL_Window *window_;
    SDL_Renderer *renderer_;

public:
    MySerial();
    size_t write(uint8_t);
    void begin(unsigned long);
    // void print(uint8_t num);
    // void print(const char *str);
    // void println(uint8_t num);
    // void println(const char *str);
    // void printf(const char *format, ...);
};

extern MySerial Serial;

#endif