#ifndef _myemulator_
#define _myemulator_

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

/*
Display is wonky. With a timer set up in MyDisplay.cpp, there are many visual artifacts. It probably interrupts
updating the texture/renderer to draw.

Other option is to check ticks in the main loop and then call display.display() after the update delay has passed.
I can't do that here because this does not have access to the display object. Doing it in LED_Arcade.cpp means I
might as well just move this main() function there in an #ifdef directive. This option has a few issues:
    -Menu uses a while loop and doesn't return until the user has selected an option. This means that navigating
        the menu wouldn't trigger display refreshes. Could probably just add display.display() to the menu.displayBlah() functions.
    -Any delay will block display updates as well. This isn't necessarily a problem unless delays are used within a block
        with display changes in between, like the current Snake gameOver screen. display.display() can manually be added after
        the delays.
This method is probably ideal overall for both the matrix and SDL

Maybe?:
-Move main() from emulator.cpp to here and enclose it in an #ifdef PC_BUILD
    -Probably need to modify the Makefile after doing that....

*/
#include <Arduino.h>

extern void setup();
extern void loop();

int main(int argc, char *argv[])
{
    srand((unsigned int)millis());
    int running = 1;

    setup();

    while (running)
    {
        loop();
    }
    return 0;
}

// const static int16_t MAX_STR_LENGTH PROGMEM = 20;

// enum
// {
//     CURSOR_POS_DIG0 = 0,
//     CURSOR_POS_DIG1,
//     CURSOR_POS_DIG2,
//     CURSOR_POS_DIG3,
//     CURSOR_POS_DIG4,
//     CURSOR_POS_DIG5,
//     CURSOR_POS_DIG6,
//     CURSOR_POS_DIG7,
//     CURSOR_POS_ITEM,
//     CURSOR_POS_MAX
// };

// void setTitle(int _title_index, uint16_t _color)
// {
//     const static int16_t TITLE_POS_X PROGMEM = 15;
//     const static int16_t TITLE_POS_Y PROGMEM = 20;
//     const static int16_t INDEX_POS_X PROGMEM = 30;
//     const static int16_t INDEX_POS_Y PROGMEM = 5;
//     const static char title_cibi_default_freq[] PROGMEM = "CIBI DEFAULT FREQ.";
//     const static char title_cibi_min_freq[] PROGMEM = " CIBI FREQ. MIN.";
//     const static char title_cibi_max_freq[] PROGMEM = " CIBI FREQ. MAX.";
//     const static char title_fi_am_fm[] PROGMEM = "    FI AM/FM";
//     const static char title_fi_usb[] PROGMEM = "    FI USB";
//     const static char title_fi_lsb[] PROGMEM = "    FI LSB";
//     const static char *const titles[] PROGMEM = {
//         title_cibi_default_freq,
//         title_cibi_min_freq,
//         title_cibi_max_freq,
//         title_fi_am_fm,
//         title_fi_usb,
//         title_fi_lsb};
//     char str[MAX_STR_LENGTH] = {0};
//     int menu_index = _title_index + 1;
//     int menu_items = sizeof(titles) / sizeof(char *);
//     display.setTextSize(1);
//     display.setFont();
//     display.setTextColor(_color);
//     snprintf(str, MAX_STR_LENGTH + 1, "CONFIG %d/%d :", menu_index, menu_items);
//     display.setCursor(INDEX_POS_X, INDEX_POS_Y);
//     display.print(str);
//     // strcpy_P(str, (char*)pgm_read_word(&(titles[_title_index])));
//     display.setCursor(TITLE_POS_X, TITLE_POS_Y);
//     display.print((char *)titles[_title_index]);
//     display.drawFastHLine(0, 15, 128, Adafruit_GFX_dummy_display::WHITE);
//     display.drawFastHLine(0, 31, 128, Adafruit_GFX_dummy_display::WHITE);
// }

// void setValue(uint32_t _value, uint16_t _color)
// {
//     const static int16_t VALUE_POS_X PROGMEM = 15;
//     const static int16_t VALUE_POS_Y PROGMEM = 55;
//     char str[MAX_STR_LENGTH] = {0};
//     snprintf(str, MAX_STR_LENGTH + 1, "%08u", _value);
//     display.setTextSize(1);
//     display.setFont(&FreeSansBold9pt7b);
//     display.setCursor(VALUE_POS_X, VALUE_POS_Y);
//     display.setTextColor(_color);
//     display.print(str);
// }

// void setCursorPos(int _position, uint16_t _color)
// {
//     const static int16_t CURSOR_POS_X PROGMEM = 15;
//     const static int16_t CURSOR_POS_Y PROGMEM = 58;
//     const static int16_t CURSOR_BIG_WIDTH PROGMEM = 10;
//     const static int16_t FONT2_XADVANCE PROGMEM = 10;
//     int16_t x;
//     if (_position >= CURSOR_POS_DIG0 && _position <= CURSOR_POS_DIG7)
//     {
//         x = CURSOR_POS_X + (7 - _position) * FONT2_XADVANCE;
//         display.drawRect(x, CURSOR_POS_Y, CURSOR_BIG_WIDTH, 2, _color);
//     }
// }

// /* Your code under test should be called in those Arduino like functions: */

// int cursor_position = 0;
// uint32_t value = 00001000;
// void setup()
// {
//     display.setTextColor(WHITE);
//     display.print("Welcome!");
//     display.display();
//     sleep(1);
// }

// void loop()
// {
//     setTitle(value % 6, WHITE);
//     setValue(value, WHITE);
//     setCursorPos(cursor_position, WHITE);
//     display.display();
//     usleep(150000);
//     display.clearDisplay();
//     value = (value + 1) % 30000000;
//     cursor_position = (cursor_position + 1) % CURSOR_POS_MAX;
// }

#endif