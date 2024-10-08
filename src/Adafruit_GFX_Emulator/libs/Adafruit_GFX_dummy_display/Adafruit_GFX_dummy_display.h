/*
 * Copyright (c) 2019, Vincent Hervieux vincent.hervieux@gmail.com
 * All rights reserved.
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 * * Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 * * Neither the name of the author Vincent Hervieux, nor the
 *   names of its contributors may be used to endorse or promote products
 *   derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE REGENTS AND CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef Adafruit_GFX_dummy_display_h
#define Adafruit_GFX_dummy_display_h

#include "Arduino.h"
#include "Adafruit_GFX.h"
#include <SDL.h>
// Added next 2 to include path in .vscode/c_cpp_properties.json
// ^PlatformIO recreates this file so they get removed, now added them to platformio.ini
// "C:/msys64/ucrt64/include/SDL2",
// "C:/msys64/ucrt64/lib/gcc/x86_64-w64-mingw32/14.1.0/include",

// check setRotation()?

/*
SDL_Renderer by default points to SDL_Window. SDL's rendering functions operate on a backbuffer.
After SDL_RenderPresent, the backbuffer should be considered invalidated after each present; do not
assume that previous contents will exist between frames. You are strongly encouraged to call
SDL_RenderClear() to initialize the backbuffer before starting each new frame's drawing, even if you
plan to overwrite every pixel.

SDL_Texture is used to maintain the active frame. SDL_SetRenderTarget() makes the renderer point to
this texture so that the rendering functions operate on the texture instead of the backbuffer.
*/

class Adafruit_GFX_dummy_display : public Adafruit_GFX
{
public:
    Adafruit_GFX_dummy_display(int16_t w, int16_t h, uint8_t zoom = 1);
    ~Adafruit_GFX_dummy_display();
    void display(void);
    void clearDisplay(void);
    void drawPixel(int16_t x, int16_t y, uint16_t color);
    void drawPixelRGB888(int16_t x, int16_t y, uint8_t r, uint8_t g, uint8_t b);
    void drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color);
    void drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color);
    void drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
    void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
    void fillScreen(uint16_t color);
    uint16_t color565(uint8_t r, uint8_t g, uint8_t b);

private:
    void initializer(int16_t w, int16_t h);
    void setColor(uint16_t color);
    SDL_Window *window_;
    SDL_Renderer *renderer_;
    SDL_Texture *texture_;
    uint8_t zoom_ratio_;
};

#endif
