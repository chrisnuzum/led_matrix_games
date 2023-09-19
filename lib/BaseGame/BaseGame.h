#ifndef _basegame_
#define _basegame_

#include "Arduino.h"
#include <PxMatrix.h>
#include <Utility.h>

class BaseGame
{
public:
    BaseGame(Utility &utility, uint8_t numPlayers);
    void loopGame();

protected:
    const uint8_t MATRIX_WIDTH;
    const uint8_t MATRIX_HEIGHT;
    PxMATRIX display;
    Utility *utility;
    uint8_t numPlayers;
    uint8_t MIN_DELAY;
    uint8_t MAX_DELAY;
    uint8_t SPEED_LOSS;
    uint16_t RESET_DELAY;

    uint8_t gameDelay;

    unsigned long msCurrent;
    unsigned long msPrevious;

    bool paused;

    uint16_t c_red;
    uint16_t c_green;
    uint16_t c_blue;
    uint16_t c_white;
    uint16_t c_yellow;
    uint16_t c_cyan;
    uint16_t c_magenta;
    uint16_t c_black;
};

#endif