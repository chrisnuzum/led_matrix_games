#include "MyDisplay.h"

uint8_t MATRIX_WIDTH_ = 64;
uint8_t MATRIX_HEIGHT_ = 64;

#ifndef PC_BUILD
hw_timer_t *timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;
uint8_t display_draw_time = 1; // 30-60 is usually fine              brightness but only used if fastupdate isn't used
PxMATRIX DRAM_ATTR disp = PxMATRIX(MATRIX_WIDTH_, MATRIX_HEIGHT_, P_LAT, P_OE, P_A, P_B, P_C, P_D, P_E);

void IRAM_ATTR display_updater()
{
    // Increment the counter and set the time of ISR
    portENTER_CRITICAL_ISR(&timerMux);
    disp.display(display_draw_time);
    portEXIT_CRITICAL_ISR(&timerMux);
}

void display_update_enable(bool is_enable)
{
    if (is_enable)
    {
        timer = timerBegin(0, 80, true); // timer clock is 80MHz so divider of 80 = 1MHz or 1/us
        timerAttachInterrupt(timer, &display_updater, true);
        timerAlarmWrite(timer, 2000, true); // 2000 us = 2 ms
        timerAlarmEnable(timer);
    }
    else
    {
        timerDetachInterrupt(timer);
        timerAlarmDisable(timer);
    }
}

#else
Adafruit_GFX_dummy_display disp = Adafruit_GFX_dummy_display(WIDTH, HEIGHT, ZOOM);

Uint32 SDL_Updater(Uint32 interval, void *param)
{
    disp.display();
    return interval;
}
SDL_TimerID timerID = SDL_AddTimer(30, &SDL_Updater, nullptr); // 2 ms
#endif

void initDisplay()
{
#ifndef PC_BUILD
    // Define your display layout here, e.g. 1/8 step, and optional SPI inputs begin(row_pattern, CLK, MOSI, MISO, SS) //utility.display.begin(8, 14, 13, 12, 4);
    disp.begin(32);
    disp.setFastUpdate(true);
    // disp.setRotation(0);
    disp.clearDisplay();
    display_update_enable(true);
#else
#endif
}