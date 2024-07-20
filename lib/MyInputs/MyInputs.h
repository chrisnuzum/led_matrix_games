#ifndef _myinputs_
#define _myinputs_

// #define PC_BUILD

/*
SDL inputs:

https://wiki.libsdl.org/SDL2/SDL_GetKeyboardState
https://stackoverflow.com/questions/27414548/sdl-timers-and-waitevent
https://stackoverflow.com/questions/48226816/sdl-window-freezes
https://stackoverflow.com/questions/12050234/closing-an-sdl-window-without-quitting-sdl
https://stackoverflow.com/questions/47046276/c-and-sdl-quit-from-function


*/

#include <Arduino.h>

#ifndef PC_BUILD
class MyInputs // ensure the same order of pins/buttons is used everywhere in this file
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
    MyInputs()
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
#else
#include <SDL.h>

/*
This is how it's called:
utility->inputs.update2(utility->inputs.pins.p1Buttons)
utility->inputs.update2(utility->inputs.pins.p1Directions)
utility->inputs.A_P1
utility->inputs.UP_P1_pressed

Seems like I never use update() anymore?
*/
class MyInputs
{
private:
    struct PINS
    {
        // no way to check the size of a passed array, so I'm adding it as first element
        // int32_t p1Buttons[3] = {2, SDLK_n, SDLK_m};
        // int32_t p1Directions[5] = {4, SDLK_UP, SDLK_DOWN, SDLK_LEFT, SDLK_RIGHT};
        // int32_t p2Buttons[3] = {2, SDLK_y, SDLK_u};
        // int32_t p2Directions[5] = {4, SDLK_w, SDLK_s, SDLK_a, SDLK_d};
        // int32_t startButton[2] = {1, SDLK_RETURN};
        uint32_t p1Buttons[3] = {2, SDL_SCANCODE_N, SDL_SCANCODE_M};
        uint32_t p1Directions[5] = {4, SDL_SCANCODE_UP, SDL_SCANCODE_DOWN, SDL_SCANCODE_LEFT, SDL_SCANCODE_RIGHT};
        uint32_t p2Buttons[3] = {2, SDL_SCANCODE_Y, SDL_SCANCODE_U};
        uint32_t p2Directions[5] = {4, SDL_SCANCODE_W, SDL_SCANCODE_S, SDL_SCANCODE_A, SDL_SCANCODE_D};
        uint32_t startButton[2] = {1, SDL_SCANCODE_RETURN};
    };

    static constexpr uint8_t numInputs = 13;

    // int32_t ALL_PINS[numInputs] = {SDLK_RETURN, SDLK_n, SDLK_m, SDLK_UP, SDLK_DOWN, SDLK_LEFT, SDLK_RIGHT,
    //    SDLK_y, SDLK_u, SDLK_w, SDLK_s, SDLK_a, SDLK_d};

    uint32_t ALL_PINS[numInputs] = {SDL_SCANCODE_RETURN, SDL_SCANCODE_N, SDL_SCANCODE_M, SDL_SCANCODE_UP, SDL_SCANCODE_DOWN,
                                    SDL_SCANCODE_LEFT, SDL_SCANCODE_RIGHT, SDL_SCANCODE_Y, SDL_SCANCODE_U, SDL_SCANCODE_W,
                                    SDL_SCANCODE_S, SDL_SCANCODE_A, SDL_SCANCODE_D};

    bool *inputsNewPress[numInputs] = {&START, &A_P1, &B_P1, &UP_P1, &DOWN_P1, &LEFT_P1, &RIGHT_P1,
                                       &A_P2, &B_P2, &UP_P2, &DOWN_P2, &LEFT_P2, &RIGHT_P2};

    bool *pressed[numInputs] = {&START_pressed, &A_P1_pressed, &B_P1_pressed, &UP_P1_pressed, &DOWN_P1_pressed, &LEFT_P1_pressed, &RIGHT_P1_pressed,
                                &A_P2_pressed, &B_P2_pressed, &UP_P2_pressed, &DOWN_P2_pressed, &LEFT_P2_pressed, &RIGHT_P2_pressed};

public:
    MyInputs()
    {
    }
    PINS pins;

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

    void update()
    {
        SDL_PumpEvents();
        const Uint8 *keyboardState = SDL_GetKeyboardState(NULL);

        for (int i = 0; i < numInputs; i++)
        {
            *inputsNewPress[i] = false;
            if (keyboardState[ALL_PINS[i]] == 1)
            {
                if (!*pressed[i]) // previous state
                {
                    *inputsNewPress[i] = true;
                }
                *pressed[i] = true;
            }
            else
            {
                *pressed[i] = false;
            }
        }
    }
    void update2(uint32_t pinsToCheck[])
    {
        SDL_PumpEvents();
        const Uint8 *keyboardState = SDL_GetKeyboardState(NULL);
        uint32_t sizeOfArray = pinsToCheck[0];
        uint8_t count = 0;

        for (int i = 0; i < numInputs; i++)
        {
            if (count < sizeOfArray)
            {
                for (int j = 1; j <= sizeOfArray; j++)
                {
                    if (ALL_PINS[i] == pinsToCheck[j])
                    {
                        *inputsNewPress[i] = false;
                        if (keyboardState[ALL_PINS[i]] == 1)
                        {
                            if (!*pressed[i]) // previous state
                            {
                                *inputsNewPress[i] = true;
                            }
                            *pressed[i] = true;
                        }
                        else
                        {
                            *pressed[i] = false;
                        }
                        count++;
                        break;
                    }
                }
            }
        }
    }
    // void update2(int32_t pinsToCheck[])
    // {
    //     // int32_t sizeOfArray = pinsToCheck[0];

    //     SDL_Event event;
    //     while (SDL_PollEvent(&event))
    //     {
    //         if (event.type == SDL_KEYDOWN)
    //         {
    //             for (int i = 0; i < numInputs; i++)
    //             {
    //                 if (event.key.keysym.sym == ALL_PINS[i])
    //                 {

    //                 }
    //             }
    //         }
    //         for (int i = 0; i < numInputs; i++)
    //         {
    //             switch (event.type)
    //             {
    //             case SDL_KEYDOWN:
    //                 switch (event.key.keysym.sym)
    //                 {
    //                 case SDLK_UP:
    //                 case SDLK_PLUS:
    //                 case SDLK_KP_PLUS:
    //                     break;
    //                 case SDLK_DOWN:
    //                 case SDLK_MINUS:
    //                 case SDLK_KP_MINUS:
    //                     break;
    //                 case SDLK_q:
    //                     SDL_Quit();
    //                     // SDL_DestroyWindow();
    //                     exit(0);
    //                 }
    //                 break;
    //             case SDL_QUIT:
    //                 SDL_Quit();
    //                 // SDL_DestroyWindow();
    //                 exit(0);
    //             }
    //         }
    //     }
    // }
};
#endif
// SDL_Event event;
// while (SDL_PollEvent(&event))
// {
//     switch (event.type)
//     {
//     case SDL_KEYDOWN:
//         switch (event.key.keysym.sym)
//         {
//         case SDLK_UP:
//         case SDLK_PLUS:
//         case SDLK_KP_PLUS:
//             break;
//         case SDLK_DOWN:
//         case SDLK_MINUS:
//         case SDLK_KP_MINUS:
//             break;
//         case SDLK_q:
//             // running = 0;
//             SDL_Quit();
//             exit(0);
//         }
//         break;
//     case SDL_QUIT:
//         // running = 0;
//         SDL_Quit();
//         exit(0);
//     }
// }
#endif