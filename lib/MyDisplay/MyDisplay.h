#ifndef _mydisplay_
#define _mydisplay_

// #define PC_BUILD

/*
https://forum.arduino.cc/t/interrupt-in-class-esp32/1039326/21?page=2
https://stackoverflow.com/questions/78126998/c-callback-to-member-function-pointers-on-esp32-arduino
https://arduino.stackexchange.com/questions/89173/attach-the-arduino-isr-function-to-the-class-member

This could maybe work to turn this into a class (use member functions in interrupts).

PxMatrix explanation
-Useful bitwise operation visualizer: https://braedonwooding.github.io/BitwiseCmpViz/#/
-The way PxMatrix does color seems odd.
  -Color depth by default is set to 4.
  -Colors passed to fillMatrixBuffer are uint_8t r, g, b values (so max of 255)
  -But, each value is bitwise right-shifted by (8 - colorDepth)
        -This means <15 becomes 0, 60 becomes 3, 120 becomes 7, 255 becomes 15
  -figure out what the interlacing does (1001 - 1016)
    -starting with rightmost bit of each color value (least significant bit) just checks if that bit is 1 and if so, adds it somewhere into the buffer
    -but, the interpolation doesn't account for the place of the digit.
        -with a color depth of 4: 16 and 143 will be turn out to be the same (143 > 10001111 and 16 > 00010000; after >> 4, both have 1 1)
        -with a color depth of 8: 1 and 128 will be the same (00000001 and 10000000)
        -remember that green has 1 more bit, so
  -Line 1353: not sure how it combines the interpolated bits to get a "brightness" for each color of each pixel
  -check lines 1022-1024
    -
-Figure out what drawPixelRGB565 does to get individual RGB values from a uint16_t color (line 1022)
    -this still would have the same shift done
-Line 367 color565(150,150,150) = 38066
-Line 1022 with 38066 = r:148, g:150, b:148
-16-bit color uses 5 bits for red and blue, 6 bits for green, hence color565
    -this means max usuable value of 31 for red and blue and 63 for green
        -BUT interlacing reduces the actual usuable even more

^I'm gonna try and organize this info a bit better and explain in order what happens in PxMatrix.
First you need a color:
1. You can simply pass your 8-bit RGB values to drawPixel888(x, y, r, g, b)
2. Or you can pack your 3 8-bit values into one 16-bit color value by calling color565(r, g, b), and then use drawPixel(x, y, color)
which calls drawPixelRGB565(x, y, color) 2a. When packing the 3 8 - bit values into a 16 - bit value, RED and BLUE become 5 bits and GREEN becomes 6 bits, so there is some loss color565() code : ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3)->so RED and BLUE just drop their rightmost 3 bits and GREEN 2 bits this means RED and BLUE lose 0 - 7 and GREEN loses 0 - 3 2b. drawPixelRGB565() unpacks the 16 - bit color into r, g, and b values uint8_t r = ((((color >> 11) & 0x1F) * 527) + 23) >> 6;
why not : ((color & 0xf800) >> 8) + 3 - adding back the middle of the range of what could have been lost uint8_t g = ((((color >> 5) & 0x3F) * 259) + 33) >> 6;
why not : ((color & 0x7e0) >> 3) + 1 - adding back the middle of the range of what could have been lost uint8_t b = (((color & 0x1F) * 527) + 23) >> 6;
why not : ((color & 0x1f) << 3) + 3 - adding back the middle of the range of what could have been lost ^ my way consistently gets closer to original values over a large number of runs : https : // cpp.sh/?source=%23include+%3Ciostream%3E%0A%23include+%3Cstring%3E%0A%23include+%3Ccmath%3E%0A%0Ausing+namespace+std%3B%0A%0Auint8_t+COLOR_DEPTH+%3D+8%3B%0A%0Auint16_t+toColor+(uint8_t+r%2C+uint8_t+g%2C+uint8_t+b)%0A%7B%0A++++return+((r+%26+0xF8)+%3C%3C+8)+%7C+((g+%26+0xFC)+%3C%3C+3)+%7C+(b+%3E%3E+3)%3B%0A%7D%0A%0Avoid+printValues+(uint8_t+r%2C+uint8_t+g%2C+uint8_t+b%2C+uint8_t+colorDepth)%0A%7B%0A++++uint16_t+fullColor+%3D+toColor(r%2C+g%2C+b)%3B%0A++++cout+%3C%3C+%22++++++++++++++R%2C+G%2C+B%3A+%22+%3C%3C+static_cast%3Cint%3E(r)+%3C%3C+%22+%22+%3C%3C+static_cast%3Cint%3E(g)+%3C%3C+%22+%22+%3C%3C+static_cast%3Cint%3E(b)+%3C%3C+%22++-%3E+to+16-bit%3A+%22+%3C%3C+static_cast%3Cint%3E(fullColor)+%3C%3C+endl%3B%0A+++%0A++++%2F%2F+cout+%3C%3C+%22Converted+to+16-bit%3A+++%22+%3C%3C+fullColor+%3C%3C+endl%3B%0A++++uint8_t+new_r+%3D+((((fullColor+%3E%3E+11)+%26+0x1F)+*+527)+%2B+23)+%3E%3E+6%3B+++++%2F%2F+5+bits%0A++++uint8_t+new_g+%3D+((((fullColor+%3E%3E+5)+%26+0x3F)+*+259)+%2B+33)+%3E%3E+6%3B++++++%2F%2F+6+bits%0A++++uint8_t+new_b+%3D+(((fullColor+%26+0x1F)+*+527)+%2B+23)+%3E%3E+6%3B+++++++++++++%2F%2F+5+bits%0A++++cout+%3C%3C+%22Converted+back+to+RGB%3A+%22+%3C%3C+static_cast%3Cint%3E(new_r)+%3C%3C+%22+%22+%3C%3C+static_cast%3Cint%3E(new_g)+%3C%3C+%22+%22+%3C%3C+static_cast%3Cint%3E(new_b)+%3C%3C+endl%3B%0A++++uint8_t+shifted_r+%3D+new_r+%3E%3E+(8+-+colorDepth)%3B%0A++++uint8_t+shifted_g+%3D+new_g+%3E%3E+(8+-+colorDepth)%3B%0A++++uint8_t+shifted_b+%3D+new_b+%3E%3E+(8+-+colorDepth)%3B%0A++++uint8_t+half_shifted_r+%3D+new_r+%3E%3E+(8+-+colorDepth+%2F+2)%3B%0A++++uint8_t+half_shifted_g+%3D+new_g+%3E%3E+(8+-+colorDepth+%2F+2)%3B%0A++++uint8_t+half_shifted_b+%3D+new_b+%3E%3E+(8+-+colorDepth+%2F+2)%3B%0A++++cout+%3C%3C+%22+++Shifted+by+depth+%22+%3C%3C+static_cast%3Cint%3E(colorDepth)+%3C%3C+%22%3A+%22+%3C%3C+static_cast%3Cint%3E(shifted_r)+%3C%3C+%22+%22+%3C%3C+static_cast%3Cint%3E(shifted_g)+%3C%3C+%22+%22+%3C%3C+static_cast%3Cint%3E(shifted_b)%3B%0A++++cout+%3C%3C+%22++by+depth+%22+%3C%3C+static_cast%3Cint%3E(colorDepth+%2F+2)+%3C%3C+%22%3A+%22+%3C%3C+static_cast%3Cint%3E(half_shifted_r)+%3C%3C+%22+%22+%3C%3C+static_cast%3Cint%3E(half_shifted_g)+%3C%3C+%22+%22+%3C%3C+static_cast%3Cint%3E(half_shifted_b)+%3C%3C+endl%3B%0A++++cout+%3C%3C+endl%3B%0A%7D%0A%0Avoid+printValuesFromFull+(uint16_t+fullColor%2C+uint8_t+colorDepth)%0A%7B+++%0A++++uint8_t+new_r+%3D+((((fullColor+%3E%3E+11)+%26+0x1F)+*+527)+%2B+23)+%3E%3E+6%3B+++++%2F%2F+5+bits%0A++++uint8_t+new_g+%3D+((((fullColor+%3E%3E+5)+%26+0x3F)+*+259)+%2B+33)+%3E%3E+6%3B++++++%2F%2F+6+bits%0A++++uint8_t+new_b+%3D+(((fullColor+%26+0x1F)+*+527)+%2B+23)+%3E%3E+6%3B+++++++++++++%2F%2F+5+bits%0A++++cout+%3C%3C+%22Converted+back+to+RGB%3A+%22+%3C%3C+static_cast%3Cint%3E(new_r)+%3C%3C+%22+%22+%3C%3C+static_cast%3Cint%3E(new_g)+%3C%3C+%22+%22+%3C%3C+static_cast%3Cint%3E(new_b)+%3C%3C+endl%3B%0A++++uint8_t+shifted_r+%3D+new_r+%3E%3E+(8+-+colorDepth)%3B%0A++++uint8_t+shifted_g+%3D+new_g+%3E%3E+(8+-+colorDepth)%3B%0A++++uint8_t+shifted_b+%3D+new_b+%3E%3E+(8+-+colorDepth)%3B%0A++++uint8_t+half_shifted_r+%3D+new_r+%3E%3E+(8+-+colorDepth+%2F+2)%3B%0A++++uint8_t+half_shifted_g+%3D+new_g+%3E%3E+(8+-+colorDepth+%2F+2)%3B%0A++++uint8_t+half_shifted_b+%3D+new_b+%3E%3E+(8+-+colorDepth+%2F+2)%3B%0A++++cout+%3C%3C+%22+++Shifted+by+depth+%22+%3C%3C+static_cast%3Cint%3E(colorDepth)+%3C%3C+%22%3A+%22+%3C%3C+static_cast%3Cint%3E(shifted_r)+%3C%3C+%22+%22+%3C%3C+static_cast%3Cint%3E(shifted_g)+%3C%3C+%22+%22+%3C%3C+static_cast%3Cint%3E(shifted_b)%3B%0A++++cout+%3C%3C+%22++by+depth+%22+%3C%3C+static_cast%3Cint%3E(colorDepth+%2F+2)+%3C%3C+%22%3A+%22+%3C%3C+static_cast%3Cint%3E(half_shifted_r)+%3C%3C+%22+%22+%3C%3C+static_cast%3Cint%3E(half_shifted_g)+%3C%3C+%22+%22+%3C%3C+static_cast%3Cint%3E(half_shifted_b)+%3C%3C+endl%3B%0A++++cout+%3C%3C+endl%3B%0A%7D%0A%0Avoid+unpacker(uint8_t+r%2C+uint8_t+g%2C+uint8_t+b%2C+uint16_t+%26theirDiff%2C+uint16_t+%26myDiff)%0A%7B%0A++++uint16_t+fullColor+%3D+toColor(r%2C+g%2C+b)%3B%0A++++cout+%3C%3C+%22++++++++++++++R%2C+G%2C+B%3A+%22+%3C%3C+static_cast%3Cint%3E(r)+%3C%3C+%22+%22+%3C%3C+static_cast%3Cint%3E(g)+%3C%3C+%22+%22+%3C%3C+static_cast%3Cint%3E(b)+%3C%3C+%22++-%3E+to+16-bit%3A+%22+%3C%3C+static_cast%3Cint%3E(fullColor)+%3C%3C+endl%3B%0A++++cout+%3C%3C+%22+++++++++++++++++++++++%22+%3C%3C+(r+%26+0xF8)+%3C%3C+%22+%22+%3C%3C+(g+%26+0xFC)+%3C%3C+%22+%22+%3C%3C+(b+%26+0xF8)+%3C%3C+endl%3B%0A++++uint8_t+new_r+%3D+((((fullColor+%3E%3E+11)+%26+0x1F)+*+527)+%2B+23)+%3E%3E+6%3B+++++%2F%2F+5+bits%0A++++uint8_t+new_g+%3D+((((fullColor+%3E%3E+5)+%26+0x3F)+*+259)+%2B+33)+%3E%3E+6%3B++++++%2F%2F+6+bits%0A++++uint8_t+new_b+%3D+(((fullColor+%26+0x1F)+*+527)+%2B+23)+%3E%3E+6%3B+++++++++++++%2F%2F+5+bits%0A++++int+diff+%3D+abs(new_r+-+r)+%2B+abs(new_g+-+g)+%2B+abs(new_b+-+b)%3B%0A++++cout+%3C%3C+%22++++++++RGB+their+way%3A+%22+%3C%3C+static_cast%3Cint%3E(new_r)+%3C%3C+%22+%22+%3C%3C+static_cast%3Cint%3E(new_g)+%3C%3C+%22+%22+%3C%3C+static_cast%3Cint%3E(new_b)+%3C%3C+%22+++(%22+%3C%3C+new_r+-+r+%3C%3C+%22+%22+%3C%3C+new_g+-+g+%3C%3C+%22+%22+%3C%3C+new_b+-+b+%3C%3C+%22)%3A+%22+%3C%3C+diff+%3C%3C+endl%3B%0A++++uint8_t+new_r2+%3D+((fullColor+%26+0xf800)+%3E%3E+8)+%2B+3%3B+++++%2F%2F+5+bits%0A++++uint8_t+new_g2+%3D+((fullColor+%26+0x7e0)+%3E%3E+3)+%2B+1%3B++++++%2F%2F+6+bits%0A++++uint8_t+new_b2+%3D+((fullColor+%26+0x1f)+%3C%3C+3)+%2B+3%3B+++++++++++++%2F%2F+5+bits%0A++++int+diff2+%3D+abs(new_r2+-+r)+%2B+abs(new_g2+-+g)+%2B+abs(new_b2+-+b)%3B%0A++++cout+%3C%3C+%22+++++++++++RGB+my+way%3A+%22+%3C%3C+static_cast%3Cint%3E(new_r2)+%3C%3C+%22+%22+%3C%3C+static_cast%3Cint%3E(new_g2)+%3C%3C+%22+%22+%3C%3C+static_cast%3Cint%3E(new_b2)+%3C%3C+%22+++(%22+%3C%3C+new_r2+-+r+%3C%3C+%22+%22+%3C%3C+new_g2+-+g+%3C%3C+%22+%22+%3C%3C+new_b2+-+b+%3C%3C+%22)%3A+%22+%3C%3C+diff2+%3C%3C+endl%3B%0A++++theirDiff+%2B%3D+diff%3B%0A++++myDiff+%2B%3D+diff2%3B%0A++++if+(diff+%3D%3D+diff2)%0A++++%7B%0A++++++++cout+%3C%3C+%22TIE%22+%3C%3C+endl+%3C%3C+endl%3B%0A++++%7D%0A++++else+if+(diff+%3C+diff2)%0A++++%7B%0A++++++++cout+%3C%3C+%22THEIR+WAY+WINS%22+%3C%3C+endl+%3C%3C+endl%3B%0A++++%7D%0A++++else%0A++++%7B%0A++++++++cout+%3C%3C+%22MY+WAY+WINS%22+%3C%3C+endl+%3C%3C+endl%3B%0A++++%7D%0A%7D%0A%0Aint+main()%0A%7B%0A++++%2F%2F+uint8_t+red+%3D+0%3B%0A++++%2F%2F+uint8_t+green+%3D+0%3B%0A++++%2F%2F+uint8_t+blue+%3D+0%3B%0A++++%2F%2F+uint8_t+depth+%3D+0%3B%0A++++%2F%2F+string+sRed%3B%0A++++%2F%2F+string+sGreen%3B%0A++++%2F%2F+string+sBlue%3B%0A++++%2F%2F+string+sDepth%3B%0A++++%0A++++uint16_t+theirTotalDiff+%3D+0%3B%0A++++uint16_t+myTotalDiff+%3D+0%3B%0A++++srand+(time(NULL))%3B%0A++++%0A++++for+(int+i+%3D+0%3B+i+%3C+80%3B+i%2B%2B)%0A++++%7B%0A++++++++unpacker(rand()+%25+255%2C+rand()+%25+255%2C+rand()+%25+255%2C+theirTotalDiff%2C+myTotalDiff)%3B%0A++++%7D%0A++++cout+%3C%3C+%22+++++++++++THEIR+TOTAL%3A+%22+%3C%3C+theirTotalDiff+%3C%3C+%22+++MY+TOTAL%3A+%22+%3C%3C+myTotalDiff+%3C%3C+endl%3B%0A%7D
                                                                                                                                                                                                  ^I haven't implemented my way yet. !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! 3. drawPixel() functions then call fillMatrixBuffer(x, y, r, g, b) 3a. first each color's offset value is subtracted from each color value (if color < offset, color is set to 0) by default all offsets are 0,
    can be changed with setColorOffset(r, g, b) 3b. some offsets are calculated to be able to place the data at the correct spot in the buffer
        3c. Each color is right
        - shifted by(8 - PxMATRIX_COLOR_DEPTH)
              r = r >> (8 - PxMATRIX_COLOR_DEPTH);
        By default, color depth is 4. This means each color loses its rightmost 4 bits, and ends up with a value between 0 and 15
            -> 0-15 becomes 0, 112-127 becomes 7, 240-255 becomes 15
    3d. Lastly, colors are interlaced into the matrix buffer
        Colors are stepped through by bit position: for (int this_color_bit = 0; this_color_bit < PxMATRIX_COLOR_DEPTH; this_color_bit++)
        If a bit is present in a color value, it is added to the buffer: (r >> this_color_bit) & 0x01       ->in for loop this checks each bit starting with rightmost bit
            See About the matrix buffer #2
        This results in the strength of a color being determined by how many 1 bits are present, regardless of their place value.
        The number of different strengths/brightnesses/intensities per color is equal to the color depth.
            -with a color depth of 4: 16 and 143 will be turn out to be the same (143 = 10001111 and 16 = 00010000; after >> 4, both have one 1)
            -with a color depth of 8: 1 and 128 will be the same (00000001 and 10000000)
            This is unintuitive! When increasing from 0-255 with color depth 4:
                0-15 = 0;
        16 - 47, 64 - 79, 128 - 143 = 1;
        48 - 63, 80 - 111, 144 - 175, 192 - 207 = 2;
        112 - 127, 176 - 191, 208 - 239 = 3;
        240 - 255 = 4 Brightness / intensity of a color jumps up and down as the value is incremented, so trying to get a certain color or create a rainbow pattern doesn't work About the matrix buffer : 1. It is defined as : PxMATRIX_buffer = new uint8_t[PxMATRIX_COLOR_DEPTH * _buffer_size];
        PxMATRIX_COLOR_DEPTH by default = 4 _buffer_size = (_width * _height * 3 / 8) so 64 * 64 * 3 / 8 = 1536 so PxMATRIX_COLOR_DEPTH *_buffer_size = 4 * 1536 = 6144 Each color of each pixel gets 4 bits._buffer_size is divided by 8 because it determines the number of bytes(uint8_t) in the buffer
2. Up to PxMATRIX_COLOR_DEPTH of bits are stored in the buffer for each color of each pixel
    if color bit is present: PxMATRIX_bufferp[this_color_bit * _buffer_size + total_offset_r] |= _BV(bit_select);
        else : PxMATRIX_bufferp[this_color_bit * _buffer_size + total_offset_r] &= ~_BV(bit_select);
        These select a byte which corresponds to 8 horizontal pixels in order. _BV(bit_select) ensures only the bit for the current pixel is updated.
        this_color_bit is incremented 0 to PxMATRIX_COLOR_DEPTH - 1
        There is one _buffer_size per depth of color
        total_offset_r is calculated based on x and y and a lot of precomputed variables based on the display
            total_offset_g = total_offset_r - _pattern_color_bytes;
        total_offset_b = total_offset_g - _pattern_color_bytes;
        bit_select is x % 8                         = 4 if x is 20
        _BV(bit_select) is (1UL << bit_select))     = 16 if bit_select is 4; equivalent to 2^bit_select     00000001   << 4 =   00010000
        So, this means buffer looks like this (for my setup of 64x64 with color depth of 4):
            4 sequential sections of 1536 uint8_ts (1 section for each depth of color)
                Each section contains bits for all pixels in matrix in BBB...BBBGGG...GGGRRR...RRR order
                If 2 out of 4 bits are present in a color at a pixel, when the buffer is pushed to the matrix that pixel will be lit for 2 out of 4 cycles
                    This gives the appearance of it being at 50% intensity
To fix:
I want to get expected color levels for the 8-bit values I pass.
There are PxMATRIX_COLOR_DEPTH number of brightness levels for each color.
    This means 256/(colorDepth + 1) is the size of each brightness range (rangeSize).
    colorValue / rangeSize = the brightness level/the number of cycles the pixel should be lit for
    0-63 = 0;
        64 - 127 = 1;
        128 - 191 = 2;
        192 - 255 = 3;
        0 - 50 = 0;
        51 - 101 = 1;
        102 - 152 = 2;
        153 - 203 = 3;
        204 - 254 = 4;

        uint8_t rTemp = 0;
        for (int i = 1; i <= PxMATRIX_COLOR_DEPTH; i++)
        {
            if (r > i * (256 / (PxMATRIX_COLOR_DEPTH + 1)))
            {
                rTemp |= _BV(i - 1);
            }
        }
*/

#include <Arduino.h>

extern uint8_t MATRIX_WIDTH_;
extern uint8_t MATRIX_HEIGHT_;

#ifndef PC_BUILD
const uint8_t DISPLAY_UPDATE_TIME = 1;

#define PxMATRIX_double_buffer true // this doesn't do anything, it is all CAPSed in PxMatrix.h
#include <PxMatrix.h>               // if issues down the road, PlatformIO Library manager installs latest release,
                                    //    but ArduinoIDE installs the lastest version of the master branch
                                    //    only the latest version works, maybe consider manually adding library to project
                                    // Definitely need to add my own because I made edits on line 766+ and 13 and 1042?
                                    // Should probably fork PxMatrix, push new commit, then use that
                                    // !!! DO THIS SOON
                                    // Check pull requests on GitHub, some good improvements
                                    // https://github.com/2dom/PxMatrix/compare/master...gcurtis79:PxMatrix:master
                                    // Pins for LED MATRIX
#define P_LAT 22
#define P_A 19
#define P_B 23
#define P_C 18
#define P_D 5
#define P_E 15
#define P_OE 16

// extern DRAM_ATTR PxMATRIX disp;
extern PxMATRIX disp;
// extern hw_timer_t *timer;
// extern portMUX_TYPE timerMux;
// extern uint8_t display_draw_time;

// extern void IRAM_ATTR display_updater();
// extern void display_update_enable(bool is_enable);

#else
const uint8_t DISPLAY_UPDATE_TIME = 2;
#define PxMATRIX_COLOR_DEPTH 4 // why am I defining this here?
#include <Adafruit_GFX_dummy_display.h>
#define HEIGHT 64
#define WIDTH 64
/* If the display is small, this will increase it by ZOOM on the display */
#define ZOOM 10 // 10
// extern SDL_TimerID timerID;
extern Adafruit_GFX_dummy_display disp;

// extern Uint32 SDL_Updater(Uint32 interval, void *param);
#endif

extern void initDisplay();

#endif