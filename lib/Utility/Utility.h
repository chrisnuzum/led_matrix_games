#ifndef _inputsfonts_
#define _inputsfonts_

#include <Arduino.h>
#include <vector>

#include <PxMatrix.h>

#include <Fonts/Tiny3x3a2pt7b.h> // 2-3 tall x 2-3 wide (most lowercase 2 wide, some 1px below baseline) (difficult to read)
#include <Fonts/My3x4_4pt7b.h>
#include <Fonts/My5x5round_4pt7b.h>
#include <Fonts/My5x5boxy_4pt7b.h>
#include <Fonts/Picopixel.h>    // 3-5 tall (lowercase 3-4) x 1-5 wide (most 3 wide, some 1px below baseline)
#include <Fonts/TomThumb.h>     // 4-5 tall x 3 wide fixed (except lowercase i, some 1px below baseline) (pretty good for small space)
#include <Fonts/Font4x5Fixed.h> // 4-5 tall x 1-4 wide (most 3 wide)
#include <Fonts/Org_01.h>       // 4-5 tall x 1-5 wide (most 4 wide, some 1px below baseline) (boxy looking)

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


*/
class Utility
{
private:
    class Inputs // ensure the same order of pins/buttons is used everywhere in this file
    {
    private:
        static constexpr uint8_t START_PIN = 99; // need a button!
        static constexpr uint8_t A_P1_PIN = 36;
        static constexpr uint8_t B_P1_PIN = 27;
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
            enum : uint8_t // probably not necessary
            {
                START_PIN = 99, // need a button!
                A_P1_PIN = 36,
                B_P1_PIN = 27,
                UP_P1_PIN = 32,
                DOWN_P1_PIN = 33,
                LEFT_P1_PIN = 25,
                RIGHT_P1_PIN = 26,
                A_P2_PIN = 3,
                B_P2_PIN = 21,
                UP_P2_PIN = 4,
                DOWN_P2_PIN = 17,
                LEFT_P2_PIN = 2,
                RIGHT_P2_PIN = 0
            };

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
                // even with external pullup it needs this run to work for some reason
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

        void update2(uint8_t pinsToCheck[]) // allows to only check certain inputs
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
            if (true)
            {
                if (!(input_current_string.equals("0") || input_current_string.equals("00") || input_current_string.equals("0000")) ||
                    !(input_new_string.equals("0") || input_new_string.equals("00") || input_new_string.equals("0000")) )
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
};

#endif