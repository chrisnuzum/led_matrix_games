#ifndef _inputsfonts_
#define _inputsfonts_

#include <Arduino.h>

#include <Fonts/Picopixel.h>
#include <Fonts/Tiny3x3a2pt7b.h>
#include <Fonts/TomThumb.h>
#include <Fonts/Org_01.h>

class Inputs
{
private:
    static constexpr uint8_t START_PIN = 25;
    static constexpr uint8_t A_P1_PIN = 27;
    static constexpr uint8_t B_P1_PIN = 27;
    static constexpr uint8_t UP_P1_PIN = 32;
    static constexpr uint8_t DOWN_P1_PIN = 33;
    static constexpr uint8_t LEFT_P1_PIN = 27;  //25
    static constexpr uint8_t RIGHT_P1_PIN = 26;
    static constexpr uint8_t A_P2_PIN = 27;
    static constexpr uint8_t B_P2_PIN = 27;
    static constexpr uint8_t UP_P2_PIN = 27;
    static constexpr uint8_t DOWN_P2_PIN = 27;
    static constexpr uint8_t LEFT_P2_PIN = 27;
    static constexpr uint8_t RIGHT_P2_PIN = 27;

    static constexpr uint8_t num_inputs = 13;

    uint8_t ALL_PINS[num_inputs] = {START_PIN, A_P1_PIN, B_P1_PIN, UP_P1_PIN, DOWN_P1_PIN, LEFT_P1_PIN, RIGHT_P1_PIN,
                                    A_P2_PIN, B_P2_PIN, UP_P2_PIN, DOWN_P2_PIN, LEFT_P2_PIN, RIGHT_P2_PIN};

public:
    Inputs()
    {
        pinMode(START_PIN, INPUT_PULLUP);
        pinMode(A_P1_PIN, INPUT_PULLUP);
        pinMode(B_P1_PIN, INPUT_PULLUP);
        pinMode(UP_P1_PIN, INPUT_PULLUP);
        pinMode(DOWN_P1_PIN, INPUT_PULLUP);
        pinMode(LEFT_P1_PIN, INPUT_PULLUP);
        pinMode(RIGHT_P1_PIN, INPUT_PULLUP);
        pinMode(A_P2_PIN, INPUT_PULLUP);
        pinMode(B_P2_PIN, INPUT_PULLUP);
        pinMode(UP_P2_PIN, INPUT_PULLUP);
        pinMode(DOWN_P2_PIN, INPUT_PULLUP);
        pinMode(LEFT_P2_PIN, INPUT_PULLUP);
        pinMode(RIGHT_P2_PIN, INPUT_PULLUP);
    }

    // bools are ints, 8 bits in size. To pack them smaller to save space: https://forum.arduino.cc/t/bool-vs-boolean-again/136074/33

    bool START = false, START_active = false;
    bool A_P1 = false, A_P1_active = false;
    bool B_P1 = false, B_P1_active = false;
    bool UP_P1 = false, UP_P1_active = false;
    bool DOWN_P1 = false, DOWN_P1_active = false;
    bool LEFT_P1 = false, LEFT_P1_active = false;
    bool RIGHT_P1 = false, RIGHT_P1_active = false;
    bool A_P2 = false, A_P2_active = false;
    bool B_P2 = false, B_P2_active = false;
    bool UP_P2 = false, UP_P2_active = false;
    bool DOWN_P2 = false, DOWN_P2_active = false;
    bool LEFT_P2 = false, LEFT_P2_active = false;
    bool RIGHT_P2 = false, RIGHT_P2_active = false;

    bool *inputs_new_press[num_inputs] = {&START, &A_P1, &B_P1, &UP_P1, &DOWN_P1, &LEFT_P1, &RIGHT_P1,
                                          &A_P2, &B_P2, &UP_P2, &DOWN_P2, &LEFT_P2, &RIGHT_P2};

    bool *inputs_current[num_inputs] = {&START_active, &A_P1_active, &B_P1_active, &UP_P1_active, &DOWN_P1_active, &LEFT_P1_active, &RIGHT_P1_active,
                                        &A_P2_active, &B_P2_active, &UP_P2_active, &DOWN_P2_active, &LEFT_P2_active, &RIGHT_P2_active};

    bool inputs_vals_prev[num_inputs] = {false, false, false, false, false, false, false, false, false, false, false, false, false};

    void update()   // need debounce
    {
        String input_prev_string = "";
        String input_current_string = "";
        String input_new_string = "";
        for (int i = 0; i < num_inputs; i++)
        {
            inputs_vals_prev[i] = *inputs_current[i];
            *inputs_current[i] = !digitalRead(ALL_PINS[i]); // pressed = LOW = 0 = false, so flipping it to mean pressed = true

            if (*inputs_current[i] == true && inputs_vals_prev[i] == false)
            {
                *inputs_new_press[i] = true;
                Serial.println("button pressed");
            }
            else
            {
                *inputs_new_press[i] = false;
            }
            input_prev_string += inputs_vals_prev[i];
            input_current_string += *inputs_current[i];
            input_new_string += *inputs_new_press[i];
        }
        
        if (!input_prev_string.equals("0000000000000") || !input_current_string.equals("0000000000000") || !input_new_string.equals("0000000000000"))
        {
            Serial.println("key   SAB^D<>AB^D<>");
            input_prev_string = "prev [" + input_prev_string + "]";
            Serial.println(input_prev_string);
            input_current_string = "curr [" + input_current_string + "]";
            Serial.println(input_current_string);
            input_new_string = "new  [" + input_new_string + "]";
            Serial.println(input_new_string);
        }
    };
};

#endif