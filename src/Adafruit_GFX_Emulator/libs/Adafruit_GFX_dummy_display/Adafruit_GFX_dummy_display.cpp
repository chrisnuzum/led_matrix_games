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
#include "Adafruit_GFX_dummy_display.h"

Adafruit_GFX_dummy_display::Adafruit_GFX_dummy_display(int16_t w, int16_t h, uint8_t zoom) : Adafruit_GFX(w, h),
                                                                                             window_(NULL),
                                                                                             renderer_(NULL),
                                                                                             zoom_ratio_(zoom)
{
   initializer(w, h);
}

Adafruit_GFX_dummy_display::~Adafruit_GFX_dummy_display()
{
   if (NULL != renderer_)
      SDL_DestroyRenderer(renderer_);
   if (NULL != window_)
      SDL_DestroyWindow(window_);
   SDL_Quit();
}

void Adafruit_GFX_dummy_display::display(void)
{
   SDL_SetRenderTarget(renderer_, NULL);
   SDL_RenderCopy(renderer_, texture_, NULL, NULL); // copies entire texture to the window, stretches to fill
   SDL_RenderPresent(renderer_);
   // clearDisplay();
   SDL_SetRenderTarget(renderer_, texture_);
}

void Adafruit_GFX_dummy_display::clearDisplay(void)
{
   SDL_SetRenderDrawColor(renderer_, 0, 0, 0, 255);
   SDL_RenderClear(renderer_);
   // display();
}

void Adafruit_GFX_dummy_display::drawPixel(int16_t x, int16_t y, uint16_t color)
{
   // Copied display rotation from Adafruit GFX
   // Set Rotation via setRotation, as usual.
   int16_t t;
   switch (rotation)
   {
   case 1:
      t = x;
      x = WIDTH - 1 - y;
      y = t;
      break;
   case 2:
      x = WIDTH - 1 - x;
      y = HEIGHT - 1 - y;
      break;
   case 3:
      t = x;
      x = y;
      y = HEIGHT - 1 - t;
      break;
   }

   setColor(color);
   if (zoom_ratio_ == 1)
   {
      if (SDL_RenderDrawPoint(renderer_, x, y) != 0)
      {
         fprintf(stderr, "Error during SDL_SetRenderDrawPoint : %s", SDL_GetError());
      }
   }
   else
   {
      SDL_Rect r;
      // r.x = x * zoom_ratio_;
      // r.y = y * zoom_ratio_;
      // r.w = zoom_ratio_;
      // r.h = zoom_ratio_;
      r.x = x;
      r.y = y;
      r.w = 1;
      r.h = 1;
      if (SDL_RenderFillRect(renderer_, &r) != 0)
      {
         fprintf(stderr, "Error during SDL_SetRenderDrawRect : %s", SDL_GetError());
      }
   }
   // display();
}

void Adafruit_GFX_dummy_display::drawPixelRGB888(int16_t x, int16_t y, uint8_t r, uint8_t g, uint8_t b)
{
   drawPixel(x, y, color565(r, g, b));
}

void Adafruit_GFX_dummy_display::drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color)
{
   setColor(color);
   SDL_Rect r;
   int16_t w = 1;
   int16_t t;
   switch (rotation)
   {
   case 1:
      t = x;
      x = WIDTH - 1 - y - h;
      y = t;
      w = h;
      h = 1;
      break;
   case 2:
      x = WIDTH - 1 - x;
      y = HEIGHT - y - h;
      break;
   case 3:
      t = x;
      x = y;
      y = HEIGHT - 1 - t;
      w = h;
      h = 1;
      break;
   }
   // r.x = x * zoom_ratio_;
   // r.y = y * zoom_ratio_;
   // r.w = w * zoom_ratio_;
   // r.h = h * zoom_ratio_;
   r.x = x;
   r.y = y;
   r.w = w;
   r.h = h;
   if (SDL_RenderFillRect(renderer_, &r) != 0)
   {
      fprintf(stderr, "Error during SDL_SetRenderDrawRect : %s", SDL_GetError());
   }
   // display();
}

void Adafruit_GFX_dummy_display::drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color)
{
   setColor(color);
   SDL_Rect r;
   int16_t h = 1;
   int16_t t;
   switch (rotation)
   {
   case 1:
      t = x;
      x = WIDTH - 1 - y;
      y = t;
      h = w;
      w = 1;
      break;
   case 2:
      x = WIDTH - x - w;
      y = HEIGHT - 1 - y;
      break;
   case 3:
      t = x;
      x = y;
      y = HEIGHT - 1 - t - w;
      h = w;
      w = 1;
      break;
   }
   // r.x = x * zoom_ratio_;
   // r.y = y * zoom_ratio_;
   // r.w = w * zoom_ratio_;
   // r.h = h * zoom_ratio_;
   r.x = x;
   r.y = y;
   r.w = w;
   r.h = h;
   if (SDL_RenderFillRect(renderer_, &r) != 0)
   {
      fprintf(stderr, "Error during SDL_SetRenderDrawRect : %s", SDL_GetError());
   }
   // display();
}

void Adafruit_GFX_dummy_display::drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
   drawFastHLine(x, y, w, color);
   drawFastHLine(x, y + h - 1, w, color);
   drawFastVLine(x, y, h, color);
   drawFastVLine(x + w - 1, y, h, color);
}

void Adafruit_GFX_dummy_display::fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
   int16_t t1;
   int16_t t2;
   switch (rotation)
   {
   case 1:
      t1 = x;
      x = WIDTH - 1 - y - h;
      y = t1;
      t2 = w;
      w = h;
      h = t2;
      break;
   case 2:
      x = WIDTH - x - w;
      y = HEIGHT - y - h;
      break;
   case 3:
      t1 = x;
      x = y;
      y = HEIGHT - 1 - t1 - w;
      t2 = w;
      w = h;
      h = t2;
      break;
   }
   setColor(color);
   SDL_Rect r;
   // r.x = x * zoom_ratio_;
   // r.y = y * zoom_ratio_;
   // r.w = w * zoom_ratio_;
   // r.h = h * zoom_ratio_;
   r.x = x;
   r.y = y;
   r.w = w;
   r.h = h;
   if (SDL_RenderFillRect(renderer_, &r) != 0)
   {
      fprintf(stderr, "Error during SDL_SetRenderDrawRect : %s", SDL_GetError());
   }
   // display();
}

void Adafruit_GFX_dummy_display::fillScreen(uint16_t color) // can switch to using default implementation (Adafruit_GFX)
{                                                           // as long as _width and _height are set in constructor (WIDTH/HEIGHT?)
   setColor(color);

   // display();
}

// Pass 8-bit (each) R,G,B, get back 16-bit packed color
uint16_t Adafruit_GFX_dummy_display::color565(uint8_t r, uint8_t g, uint8_t b)
{
   return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}

void Adafruit_GFX_dummy_display::initializer(int16_t w, int16_t h)
{
   bool _error = false;
   if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0)
   {
      fprintf(stderr, "Error during SDL_Init : %s", SDL_GetError());
      _error = true;
   }
   window_ = SDL_CreateWindow("Adafruit_GFX dummy display", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              w * zoom_ratio_, h * zoom_ratio_, SDL_WINDOW_SHOWN);
   if (window_ == NULL)
   {
      fprintf(stderr, "Error SDL_CreateWindow : %s", SDL_GetError());
      _error = true;
   }
   renderer_ = SDL_CreateRenderer(window_, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);
   if (renderer_ == NULL)
   {
      fprintf(stderr, "Error SDL_CreateRenderer : %s", SDL_GetError());
      _error = true;
   }
   texture_ = SDL_CreateTexture(renderer_, SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_TARGET, w, h);
   SDL_SetTextureBlendMode(texture_, SDL_BLENDMODE_NONE);
   SDL_SetRenderTarget(renderer_, texture_);

   if (_error)
   {
      if (renderer_ != NULL)
         SDL_DestroyRenderer(renderer_);
      if (NULL != window_)
         SDL_DestroyWindow(window_);
      SDL_Quit();
   }
   return;
}

void Adafruit_GFX_dummy_display::setColor(uint16_t color)
{
   uint8_t r = ((((color >> 11) & 0x1F) * 527) + 23) >> 6;
   uint8_t g = ((((color >> 5) & 0x3F) * 259) + 33) >> 6;
   uint8_t b = (((color & 0x1F) * 527) + 23) >> 6;

   SDL_SetRenderDrawColor(renderer_, r, g, b, 255);
}
