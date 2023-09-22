#ifndef _basegame_
#define _basegame_

#include "Arduino.h"
#include <PxMatrix.h>
#include <Utility.h>

class BaseGame
{
public:
    BaseGame(Utility &utility, uint8_t numPlayers) : utility(&utility),
                                                     display(utility.display),
                                                     MATRIX_WIDTH(utility.MATRIX_WIDTH),
                                                     MATRIX_HEIGHT(utility.MATRIX_HEIGHT),
                                                     numPlayers(numPlayers)
    {
    }
    virtual bool loopGame();

protected:
    Utility *utility;
    PxMATRIX display;
    const uint8_t MATRIX_WIDTH;
    const uint8_t MATRIX_HEIGHT;
    uint8_t numPlayers;

    uint16_t RESET_DELAY;

    uint8_t updateDelay;
    unsigned long msCurrent;
    unsigned long msPrevious;

    bool paused;
};

#endif