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
    virtual void setPlayers(uint8_t players)
    {
        numPlayers = players;
    };
    virtual bool loopGame() = 0;

protected:
    Utility *utility; // this has to be a pointer or inputs are unreliable, even when it is passed here as reference. not sure why
    PxMATRIX display;
    const uint8_t MATRIX_WIDTH;
    const uint8_t MATRIX_HEIGHT;
    uint8_t numPlayers;

    uint8_t updateDelay;
    unsigned long msCurrent;
    unsigned long msPrevious;
    uint16_t GAME_OVER_DELAY;
    bool paused;
};

#endif