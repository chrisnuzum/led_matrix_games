#ifndef _basegame_
#define _basegame_

#include <Arduino.h>
// #include <PxMatrix.h>
#include <Utility.h>

const uint8_t MAX_NUM_PLAYER_OPTIONS = 3;
const uint8_t NUM_MAX_OPTIONS = 10;

// struct MenuInfo
// {
//     MenuInfo(const char *gameName) : gameDisplayName(gameName)
//     {
//     }
//     const char *playerOptions[MAX_NUM_PLAYER_OPTIONS];  // the BaseGame object will receive the selected mode's index as number of players
//     const char *gameDisplayName;
//     // const BaseGame *gamePointer;

//     void setMenuInfo(bool autoplayMode, bool onePlayerMode, bool twoPlayerMode)
//     {
//         playerOptions[0] = autoplayMode ? "Auto" : "";
//         playerOptions[1] = onePlayerMode ? "1P" : "";
//         playerOptions[2] = twoPlayerMode ? "2P" : "";
//     }
// };

class BaseGame
{
public: // https://www.learncpp.com/cpp-tutorial/constructors-and-initialization-of-derived-classes/
    BaseGame(Utility &utility) : utility(&utility),
                                 //  display(utility.display),
                                 MATRIX_WIDTH(utility.MATRIX_WIDTH),
                                 MATRIX_HEIGHT(utility.MATRIX_HEIGHT)
    {
    }
    virtual ~BaseGame(){}; // this allows child objects stored in BaseGame* to have their own destructors called
    virtual void setPlayers(uint8_t) = 0;
    virtual bool loopGame() = 0; // https://www.geeksforgeeks.org/pure-virtual-functions-and-abstract-classes/#
    bool justStarted = false;

protected:
    Utility *utility; // this has to be a pointer or inputs are unreliable, even when it is passed here as reference. not sure why
    // PxMATRIX display;
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

// With an array the number of elements needs to be defined ahead of time and they need to be inserted by index.
// This is not necessary for map or vector.

// std::map<GameOption, void (*)()> options;

/*
Example of a way that derived classes can specify size of base class's array:
class Baseclass {
public:
uint16_t *arr;
size_t arraySize;

Baseclass(uint16_t *arr, size_t arraySize) : arr(arr), arraySize(arraySize)
{}
};

class InheritedClass : public Baseclass
{
InheritedClass() : Baseclass(arr_memory, 5)
{}
private:
uint16_t arr_memory[5];
};
*/