#ifndef _inputsfonts_
#define _inputsfonts_

// #define PC_BUILD

#include <Arduino.h>

#include <MyDisplay.h>
#include <MyInputs.h>

#include <Fonts/Tiny3x3a2pt7b.h> // 2-3 tall x 2-3 wide (most lowercase 2 wide, some 1px below baseline) (difficult to read)
#include <Fonts/My3x4_4pt7b.h>
#include <Fonts/My5x5round_4pt7b.h>
#include <Fonts/My5x5boxy_4pt7b.h>
#include <Fonts/Picopixel.h>    // 3-5 tall (lowercase 3-4) x 1-5 wide (most 3 wide, some 1px below baseline)
#include <Fonts/TomThumb.h>     // 4-5 tall x 3 wide fixed (except lowercase i, some 1px below baseline) (pretty good for small space)
#include <Fonts/Font4x5Fixed.h> // 4-5 tall x 1-4 wide (most 3 wide)
#include <Fonts/Org_01.h>       // 4-5 tall x 1-5 wide (most 4 wide, some 1px below baseline) (boxy looking)

// Desktop/Arduino/Arduino Fonts
// font viewer: https://github.com/fel88/GFXFontTool
// color picker: https://barth-dev.de/online/rgb565-color-picker/
// bitmap to PROGMEM (only monochrome): https://javl.github.io/image2cpp/

// probably want mono nums for scoring

/* potentially better fonts: https://github.com/robjen/GFX_fonts
 first: https://www.pentacom.jp/pentacom/bitfontmaker2/
 second: https://yal.cc/r/20/pixelfont/frombmf/       import bitfontmaker2 json, copy image into website,
                                                        tile width: widest character
                                                        tile height: includes descender
                                                        offsets/separations: 1
                                                        baseline x: 0
                                                        baseline: height without descender
                                                      figure out ascender/descender proportions,
                                                      fill out/clear meta info (add _ to end of name)
 third: convert any truetype font to AdafruitGFX: https://rop.nl/truetype2gfx/        change size to 4pt
*/
/*
Notes:

!!!B_P1 not working!!!
    -only in games, works in menus....
Add all of my fonts to the project directory


*/
class Utility
{
private:
    class Fonts
    {
    public:
        const GFXfont *pico = &Picopixel;
        const GFXfont *tiny = &Tiny3x3a2pt7b;
        const GFXfont *tom = &TomThumb;
        const GFXfont *org = &Org_01;
        const GFXfont *f4x5 = &Font4x5Fixed;
        const GFXfont *my3x4 = &My3x4_4pt7b;
        const GFXfont *my5x5round = &My5x5round_4pt7b;
        const GFXfont *my5x5boxy = &My5x5boxy_4pt7b;
    };

    class Colors
    {
    public:
        uint16_t redLight;
        uint16_t red;
        uint16_t redDark;
        uint16_t orange;
        uint16_t yellow;
        uint16_t greenYellow;
        uint16_t greenLight;
        uint16_t green;
        uint16_t greenDark;
        uint16_t greenCyan;
        uint16_t cyan;
        uint16_t cyanBlue;
        uint16_t blueLight;
        uint16_t blue;
        uint16_t blueDark;
        uint16_t purple;
        uint16_t magenta;
        uint16_t pink;
        uint16_t white;
        uint16_t gray;
        uint16_t black;
    };

public:
    Utility() : display(disp), MATRIX_WIDTH(MATRIX_WIDTH_), MATRIX_HEIGHT(MATRIX_HEIGHT_)
    {
        colors.redLight = display.color565(255, 140, 140);
        colors.red = display.color565(255, 0, 0);
        colors.redDark = display.color565(180, 0, 0);
        colors.orange = display.color565(255, 128, 0);
        colors.yellow = display.color565(255, 255, 0);
        colors.greenYellow = display.color565(128, 255, 0);
        colors.greenLight = display.color565(140, 255, 140);
        colors.green = display.color565(0, 255, 0);
        colors.greenDark = display.color565(0, 180, 0);
        colors.greenCyan = display.color565(0, 255, 128);
        colors.cyan = display.color565(0, 255, 255);
        colors.cyanBlue = display.color565(0, 128, 255);
        colors.blueLight = display.color565(140, 140, 255);
        colors.blue = display.color565(0, 0, 255);
        colors.blueDark = display.color565(0, 0, 180);
        colors.purple = display.color565(128, 0, 255);
        colors.magenta = display.color565(255, 0, 255);
        colors.pink = display.color565(255, 0, 128);
        colors.white = display.color565(255, 255, 255);
        colors.gray = display.color565(128, 128, 128);
        colors.black = display.color565(0, 0, 0);
    }
#ifndef PC_BUILD
    PxMATRIX display;
#else
    Adafruit_GFX_dummy_display display;
#endif
    uint8_t MATRIX_WIDTH;
    uint8_t MATRIX_HEIGHT;
    MyInputs inputs;
    Fonts fonts;
    Colors colors;

    void delayWhileDisplaying(uint32_t msDelay)
    {
#ifdef PC_BUILD
        delay(msDelay);
#else
        uint32_t begin = millis();
        display.display();
        uint32_t _lastDisplayUpdate = millis();
        while (millis() < begin + msDelay)
        {
            if (millis() - _lastDisplayUpdate > DISPLAY_UPDATE_TIME)
            {
                display.display();
                _lastDisplayUpdate = millis();
            }
        }
#endif
    }

    void setDisplay()
    {
        display = disp;
    }

    void inputTest() // P1's A+B+DOWN ends the test
    {
        display.clearDisplay();
        display.setFont(fonts.my5x5round);
        display.setTextColor(colors.white);
        display.setCursor(7, 11);
        display.print("P1");
        display.setCursor(36, 11);
        display.print("P2");

        // bool lastState[13] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
        bool LAST_START = false;
        bool LAST_A_P1 = false;
        bool LAST_B_P1 = false;
        bool LAST_UP_P1 = false;
        bool LAST_DOWN_P1 = false;
        bool LAST_LEFT_P1 = false;
        bool LAST_RIGHT_P1 = false;
        bool LAST_A_P2 = false;
        bool LAST_B_P2 = false;
        bool LAST_UP_P2 = false;
        bool LAST_DOWN_P2 = false;
        bool LAST_LEFT_P2 = false;
        bool LAST_RIGHT_P2 = false;
        while (true)
        {
            inputs.update();
            // bool refresh = false;
            // for (int i = 0; i < 13; i++)
            // {
            //     if ()
            // }
            if (inputs.START_pressed != LAST_START ||
                inputs.A_P1_pressed != LAST_A_P1 ||
                inputs.B_P1_pressed != LAST_B_P1 ||
                inputs.UP_P1_pressed != LAST_UP_P1 ||
                inputs.DOWN_P1_pressed != LAST_DOWN_P1 ||
                inputs.LEFT_P1_pressed != LAST_LEFT_P1 ||
                inputs.RIGHT_P1_pressed != LAST_RIGHT_P1 ||
                inputs.A_P2_pressed != LAST_A_P2 ||
                inputs.B_P2_pressed != LAST_B_P2 ||
                inputs.UP_P2_pressed != LAST_UP_P2 ||
                inputs.DOWN_P2_pressed != LAST_DOWN_P2 ||
                inputs.LEFT_P2_pressed != LAST_LEFT_P2 ||
                inputs.RIGHT_P2_pressed != LAST_RIGHT_P2)
            {
                LAST_START = inputs.START_pressed;
                LAST_A_P1 = inputs.A_P1_pressed;
                LAST_B_P1 = inputs.B_P1_pressed;
                LAST_UP_P1 = inputs.UP_P1_pressed;
                LAST_DOWN_P1 = inputs.DOWN_P1_pressed;
                LAST_LEFT_P1 = inputs.LEFT_P1_pressed;
                LAST_RIGHT_P1 = inputs.RIGHT_P1_pressed;
                LAST_A_P2 = inputs.A_P2_pressed;
                LAST_B_P2 = inputs.B_P2_pressed;
                LAST_UP_P2 = inputs.UP_P2_pressed;
                LAST_DOWN_P2 = inputs.DOWN_P2_pressed;
                LAST_LEFT_P2 = inputs.LEFT_P2_pressed;
                LAST_RIGHT_P2 = inputs.RIGHT_P2_pressed;
                display.clearDisplay();
                display.setCursor(20, 5);
                display.print(inputs.START_pressed ? "START" : "");
                display.setCursor(7, 11);
                display.print("P1");
                display.setCursor(1, 17);
                display.print(inputs.A_P1_pressed ? "A" : "");
                display.setCursor(1, 23);
                display.print(inputs.B_P1_pressed ? "B" : "");
                display.setCursor(1, 29);
                display.print(inputs.UP_P1_pressed ? "UP" : "");
                display.setCursor(1, 35);
                display.print(inputs.DOWN_P1_pressed ? "DOWN" : "");
                display.setCursor(1, 41);
                display.print(inputs.LEFT_P1_pressed ? "LEFT" : "");
                display.setCursor(1, 47);
                display.print(inputs.RIGHT_P1_pressed ? "RIGHT" : "");

                display.setCursor(36, 11);
                display.print("P2");
                display.setCursor(30, 17);
                display.print(inputs.A_P2_pressed ? "A" : "");
                display.setCursor(30, 23);
                display.print(inputs.B_P2_pressed ? "B" : "");
                display.setCursor(30, 29);
                display.print(inputs.UP_P2_pressed ? "UP" : "");
                display.setCursor(30, 35);
                display.print(inputs.DOWN_P2_pressed ? "DOWN" : "");
                display.setCursor(30, 41);
                display.print(inputs.LEFT_P2_pressed ? "LEFT" : "");
                display.setCursor(30, 47);
                display.print(inputs.RIGHT_P2_pressed ? "RIGHT" : "");
            }

            if (inputs.A_P1_pressed && inputs.B_P1_pressed && inputs.DOWN_P1_pressed)
            {
                display.clearDisplay();
                display.setCursor(7, 11);
                display.print("ENDING TEST");
                delay(3000);
                break;
            }
        }
    }

    /*
    New plan:
    Each section's background is only that individual color, so Red ignores Green and Blue.
    Right side sections show the full RGB color.
    ~ sections are equivalent to 255-###

    R: ###      RGB         R: is x=0-31, y=0-10        RGB is x=32-63, y=0-31
    G: ###      RGB         G: is x=0-31, y=11-21
    B: ###      RGB         B: is x=0-31, y=22-31
    ~R: ###     ~RGB        ~R: is x=0-31, y=32-42      ~RGB is x=32-63, y=32-63
    ~G: ###     ~RGB        ~G: is x=0-31, y=43-53
    ~B: ###     ~RGB        ~B: is x=0-31, y=54-63

    */
    void colorTest()
    {
        uint8_t red = 0;
        uint8_t lastRed = 1;
        uint8_t green = 0;
        uint8_t lastGreen = 1;
        uint8_t blue = 0;
        uint8_t lastBlue = 1;
        display.fillScreen(display.color565(red, green, blue));
        unsigned long msPrevious = 0;

        while (true)
        {
            inputs.update();
            if (inputs.B_P1)
            {
                break;
            }
            if (millis() - msPrevious > 100)
            {
                if (inputs.UP_P1_pressed)
                {
                    red += inputs.A_P1_pressed ? -1 : 1;
                }
                if (inputs.LEFT_P1_pressed)
                {
                    green += inputs.A_P1_pressed ? -1 : 1;
                }
                if (inputs.RIGHT_P1_pressed)
                {
                    blue += inputs.A_P1_pressed ? -1 : 1;
                }
            }
            if (red != lastRed || green != lastGreen || blue != lastBlue)
            {
                display.setFont(fonts.my5x5round);
                lastRed = red;
                lastGreen = green;
                lastBlue = blue;
                msPrevious = millis();
                // display.fillScreen(display.color565(red, green, blue));

                display.fillRect(24, 0, 13, 64, colors.black); // to clear the number section

                display.fillRect(0, 0, 23, 11, display.color565(red, 0, 0));
                display.drawRect(23, 0, 15, 11, display.color565(red, 0, 0));
                display.fillRect(0, 11, 23, 11, display.color565(0, green, 0));
                display.drawRect(23, 11, 15, 11, display.color565(0, green, 0));
                display.fillRect(0, 22, 23, 10, display.color565(0, 0, blue));
                display.drawRect(23, 22, 15, 10, display.color565(0, 0, blue));
                display.fillRect(38, 0, 26, 32, display.color565(red, green, blue));

                // display.setTextColor(display.color565(255 - red, 255 - green, 255 - blue));

                display.fillRect(0, 32, 23, 11, display.color565(255 - red, 0, 0));
                display.drawRect(23, 32, 15, 11, display.color565(255 - red, 0, 0));
                display.fillRect(0, 43, 23, 11, display.color565(0, 255 - green, 0));
                display.drawRect(23, 43, 15, 11, display.color565(0, 255 - green, 0));
                display.fillRect(0, 54, 23, 10, display.color565(0, 0, 255 - blue));
                display.drawRect(23, 54, 15, 10, display.color565(0, 0, 255 - blue));
                display.fillRect(38, 32, 26, 32, display.color565(255 - red, 255 - green, 255 - blue));

                display.setTextColor(colors.white);
                display.setCursor(1, 7);
                display.print("red:     ");
                display.print(red);
                display.setCursor(1, 18);
                display.print("green: ");
                display.print(green);
                display.setCursor(1, 28);
                display.print("blue:    ");
                display.print(blue);

                display.setCursor(1, 39);
                display.print("red:     ");
                display.print(255 - red);
                display.setCursor(1, 50);
                display.print("green: ");
                display.print(255 - green);
                display.setCursor(1, 60);
                display.print("blue:    ");
                display.print(255 - blue);
            }
        }
    }
};

#endif