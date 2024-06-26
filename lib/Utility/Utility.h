#ifndef _inputsfonts_
#define _inputsfonts_

#include <Arduino.h>
// #include <vector>

#include <PxMatrix.h>

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
    class Inputs // ensure the same order of pins/buttons is used everywhere in this file
    {
    private:
        static constexpr uint8_t START_PIN = 36; // need a button!
        static constexpr uint8_t A_P1_PIN = 36;
        static constexpr uint8_t B_P1_PIN = 27; // NOT WORKING??!!
        static constexpr uint8_t UP_P1_PIN = 32;
        static constexpr uint8_t DOWN_P1_PIN = 33;
        static constexpr uint8_t LEFT_P1_PIN = 25;
        static constexpr uint8_t RIGHT_P1_PIN = 26;
        static constexpr uint8_t A_P2_PIN = 3;
        static constexpr uint8_t B_P2_PIN = 21;
        static constexpr uint8_t UP_P2_PIN = 4;
        static constexpr uint8_t DOWN_P2_PIN = 17;
        static constexpr uint8_t LEFT_P2_PIN = 2;
        static constexpr uint8_t RIGHT_P2_PIN = 0;

        struct PINS
        {
            // enum : uint8_t // probably not necessary
            // {
            //     START_PIN = 36, // need a button!
            //     A_P1_PIN = 36,
            //     B_P1_PIN = 27,
            //     UP_P1_PIN = 32,
            //     DOWN_P1_PIN = 33,
            //     LEFT_P1_PIN = 25,
            //     RIGHT_P1_PIN = 26,
            //     A_P2_PIN = 3,
            //     B_P2_PIN = 21,
            //     UP_P2_PIN = 4,
            //     DOWN_P2_PIN = 17,
            //     LEFT_P2_PIN = 2,
            //     RIGHT_P2_PIN = 0
            // };

            // no way to check the size of a passed array, so I'm adding it as first element
            uint8_t p1Buttons[3] = {2, A_P1_PIN, B_P1_PIN};
            uint8_t p1Directions[5] = {4, UP_P1_PIN, DOWN_P1_PIN, LEFT_P1_PIN, RIGHT_P1_PIN};
            uint8_t p2Buttons[3] = {2, A_P2_PIN, B_P2_PIN};
            uint8_t p2Directions[5] = {4, UP_P2_PIN, DOWN_P2_PIN, LEFT_P2_PIN, RIGHT_P2_PIN};
            uint8_t startButton[2] = {1, START_PIN};
        };

        static constexpr uint8_t numInputs = 13;
        static constexpr uint8_t DEBOUNCE_PERIOD = 50; // uint8_t is max of 255

        uint8_t ALL_PINS[numInputs] = {START_PIN, A_P1_PIN, B_P1_PIN, UP_P1_PIN, DOWN_P1_PIN, LEFT_P1_PIN, RIGHT_P1_PIN,
                                       A_P2_PIN, B_P2_PIN, UP_P2_PIN, DOWN_P2_PIN, LEFT_P2_PIN, RIGHT_P2_PIN};

        // unsigned long lastPress = 0;
        unsigned long lastPress[numInputs] = {0};

        bool *inputsNewPress[numInputs] = {&START, &A_P1, &B_P1, &UP_P1, &DOWN_P1, &LEFT_P1, &RIGHT_P1,
                                           &A_P2, &B_P2, &UP_P2, &DOWN_P2, &LEFT_P2, &RIGHT_P2};

        // bool inputsValsPrev[numInputs] = {false, false, false, false, false, false, false, false, false, false, false, false, false};

        bool *pressed[numInputs] = {&START_pressed, &A_P1_pressed, &B_P1_pressed, &UP_P1_pressed, &DOWN_P1_pressed, &LEFT_P1_pressed, &RIGHT_P1_pressed,
                                    &A_P2_pressed, &B_P2_pressed, &UP_P2_pressed, &DOWN_P2_pressed, &LEFT_P2_pressed, &RIGHT_P2_pressed};

        bool lastPressed = false;
        bool inputCurrent = false;

    public:
        Inputs()
        {
            for (uint8_t PIN_NUMBER : ALL_PINS)
            {
                // if (PIN_NUMBER == 34 || PIN_NUMBER == 35 || PIN_NUMBER == 36 || PIN_NUMBER == 39 || PIN_NUMBER == 12) // these require external pull-ups, 12 can't be used at all until efuses are burned
                // {
                //     continue;
                // }
                // even with external pullup it still needs this command to be run for some reason
                pinMode(PIN_NUMBER, INPUT_PULLUP);
            }
        }

        PINS pins; // probably not necessary

        // bools are ints, 8 bits in size. To pack them smaller to save space: https://forum.arduino.cc/t/bool-vs-boolean-again/136074/33

        bool START = false, START_pressed = false;
        bool A_P1 = false, A_P1_pressed = false;
        bool B_P1 = false, B_P1_pressed = false;
        bool UP_P1 = false, UP_P1_pressed = false;
        bool DOWN_P1 = false, DOWN_P1_pressed = false;
        bool LEFT_P1 = false, LEFT_P1_pressed = false;
        bool RIGHT_P1 = false, RIGHT_P1_pressed = false;
        bool A_P2 = false, A_P2_pressed = false;
        bool B_P2 = false, B_P2_pressed = false;
        bool UP_P2 = false, UP_P2_pressed = false;
        bool DOWN_P2 = false, DOWN_P2_pressed = false;
        bool LEFT_P2 = false, LEFT_P2_pressed = false;
        bool RIGHT_P2 = false, RIGHT_P2_pressed = false;

        void update() // should be able to update only certain inputs, so the "_active" variables are rarely required
        {             // for instance, checking for pause (START) shouldn't find a new direction press
                      // maybe pass in variable arguments of some (public) class type that refers to the pin numbers
                      // this is called variadic arguments https://stackoverflow.com/questions/1657883/variable-number-of-arguments-in-c
                      //
            String input_current_string = "";
            String input_new_string = "";

            for (int i = 0; i < numInputs; i++)
            {
                inputCurrent = !digitalRead(ALL_PINS[i]); // pressed = LOW = 0 = false, so flipping it to mean pressed = true
                input_current_string += inputCurrent;

                *inputsNewPress[i] = false; // this should only ever be true for 1 update()

                if (millis() - lastPress[i] > DEBOUNCE_PERIOD) // if debounce period (per input) has not passed don't bother looking for new press
                {
                    lastPressed = *pressed[i];
                    *pressed[i] = inputCurrent ? true : false;

                    if (!lastPressed && *pressed[i])
                    {
                        *inputsNewPress[i] = true;
                        lastPress[i] = millis();
                    }
                    else if (lastPressed && !*pressed[i])
                    {
                        lastPress[i] = millis();
                    }
                }
                input_new_string += *inputsNewPress[i] ? "1" : "0";
            }

            if (false)
            {
                if (!input_current_string.equals("0000000000000") || !input_new_string.equals("0000000000000"))
                {
                    Serial.println("key   SAB^D<>AB^D<>");
                    input_current_string = "curr [" + input_current_string + "]";
                    Serial.println(input_current_string);
                    input_new_string = "new  [" + input_new_string + "]";
                    Serial.println(input_new_string);
                }
            }
        };

        // allows to only check certain inputs
        void update2(uint8_t pinsToCheck[]) // could make this a special class/struct, could also pass by ref and use template to get size
        {
            String input_current_string = "";
            String input_new_string = "";

            //  could say if (pinsToCheck == null) pinsToCheck = ALL_PINS   along with pinsToCheck defaulting to null if not passed
            uint8_t sizeOfArray = pinsToCheck[0];

            uint8_t count = 0;

            for (int i = 0; i < numInputs; i++)
            {
                if (count < sizeOfArray)
                {
                    for (int j = 1; j <= sizeOfArray; j++)
                    {
                        if (ALL_PINS[i] == pinsToCheck[j])
                        {
                            inputCurrent = !digitalRead(ALL_PINS[i]); // pressed = LOW = 0 = false, so flipping it to mean pressed = true
                            input_current_string += inputCurrent;

                            *inputsNewPress[i] = false; // this should only ever be true for 1 update()

                            if (millis() - lastPress[i] > DEBOUNCE_PERIOD) // if debounce period (per input) has not passed don't bother looking for new press
                            {
                                lastPressed = *pressed[i];
                                *pressed[i] = inputCurrent ? true : false;

                                if (!lastPressed && *pressed[i])
                                {
                                    *inputsNewPress[i] = true;
                                    lastPress[i] = millis();
                                }
                                else if (lastPressed && !*pressed[i])
                                {
                                    lastPress[i] = millis();
                                }
                            }
                            input_new_string += *inputsNewPress[i] ? "1" : "0";
                            count++;
                            break;
                        }
                    }
                }
                else
                {
                    break;
                }
            }
            if (false)
            {
                if (!(input_current_string.equals("0") || input_current_string.equals("00") || input_current_string.equals("0000")) ||
                    !(input_new_string.equals("0") || input_new_string.equals("00") || input_new_string.equals("0000")))
                {
                    Serial.println("key   SAB^D<>AB^D<>");
                    input_current_string = "curr [" + input_current_string + "]";
                    Serial.println(input_current_string);
                    input_new_string = "new  [" + input_new_string + "]";
                    Serial.println(input_new_string);
                }
            }
        };
    };

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
    Utility(uint8_t MATRIX_WIDTH, uint8_t MATRIX_HEIGHT, PxMATRIX &display) : MATRIX_WIDTH(MATRIX_WIDTH),
                                                                              MATRIX_HEIGHT(MATRIX_HEIGHT),
                                                                              display(display)
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
    const uint8_t MATRIX_WIDTH;
    const uint8_t MATRIX_HEIGHT;
    PxMATRIX display;

    Inputs inputs;
    Fonts fonts;
    Colors colors;

    void setDisplay(PxMATRIX d)
    {
        this->display = d;
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
                //display.fillScreen(display.color565(red, green, blue));

                display.fillRect(24, 0, 13, 64, colors.black);  // to clear the number section

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