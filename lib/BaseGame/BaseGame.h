#ifndef _basegame_
#define _basegame_

#include "Arduino.h"
#include <PxMatrix.h>
#include <Utility.h>

class BaseGame
{
public: // https://www.learncpp.com/cpp-tutorial/constructors-and-initialization-of-derived-classes/
    BaseGame(Utility &utility) : utility(&utility),
                                                     display(utility.display),
                                                     MATRIX_WIDTH(utility.MATRIX_WIDTH),
                                                     MATRIX_HEIGHT(utility.MATRIX_HEIGHT)
    {
    }
    virtual bool loopGame() = 0;    // https://www.geeksforgeeks.org/pure-virtual-functions-and-abstract-classes/#

protected:
    Utility *utility; // this has to be a pointer or inputs are unreliable, even when it is passed here as reference. not sure why
    PxMATRIX display;
    const uint8_t MATRIX_WIDTH;
    const uint8_t MATRIX_HEIGHT;
    uint8_t numPlayers;

    uint8_t updateDelay;
    unsigned long msPrevious;
    uint16_t GAME_OVER_DELAY;
    bool paused;

    enum direction
    {
        UP,
        DOWN,
        LEFT,
        RIGHT
    };
};

#endif