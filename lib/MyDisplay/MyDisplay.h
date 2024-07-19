#ifndef _mydisplay_
#define _mydisplay_

// #define PC_BUILD

/*
https://forum.arduino.cc/t/interrupt-in-class-esp32/1039326/21?page=2
https://stackoverflow.com/questions/78126998/c-callback-to-member-function-pointers-on-esp32-arduino
https://arduino.stackexchange.com/questions/89173/attach-the-arduino-isr-function-to-the-class-member

This could maybe work to turn this into a class (use member functions in interrupts).

*/

#include <Arduino.h>

extern uint8_t MATRIX_WIDTH_;
extern uint8_t MATRIX_HEIGHT_;

#ifndef PC_BUILD
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

extern DRAM_ATTR PxMATRIX disp;
extern hw_timer_t *timer;
extern portMUX_TYPE timerMux;
extern uint8_t display_draw_time;

extern void IRAM_ATTR display_updater();
extern void display_update_enable(bool is_enable);

#else
#define PxMATRIX_COLOR_DEPTH 4
#include <Adafruit_GFX_dummy_display.h>
#define HEIGHT 64
#define WIDTH 64
/* If the display is small, this will increase it by ZOOM on the display */
#define ZOOM 10
extern SDL_TimerID timerID;
extern Adafruit_GFX_dummy_display disp;

extern Uint32 SDL_Updater(Uint32 interval, void *param);
#endif

extern void initDisplay();

#endif