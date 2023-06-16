#include "Arduino.h"
#include <LinkedList.h>
#include <PxMatrix.h>
#include <InputsFonts.h>

struct TetrisPiece
{
    bool iPiece[4][4] = 
    {{0, 0, 0, 0},
    {1, 1, 1, 1},
    {0, 0, 0, 0},
    {0, 0, 0, 0}};
    bool jPiece[3][3] =
    {{1, 0, 0},
    {1, 1, 1},
    {0, 0, 0}};
    bool lPiece[3][3] =
    {{0, 0, 1},
    {1, 1, 1},
    {0, 0, 0}};
    bool oPiece[2][2] =
    {{1, 1},
    {1, 1}};
    bool sPiece [3][3] =
    {{0, 1, 1},
    {1, 1, 0},
    {0, 0, 0}};
    bool tPiece [3][3] =
    {{0, 1, 0},
    {1, 1, 1},
    {0, 0, 0}};
    bool zPiece [3][3] =
    {{1, 1, 0},
    {0, 1, 1},
    {0, 0, 0}};



    int lengthOfLine = sizeof(iPiece)/sizeof(iPiece[0]);

};

class Tetris
{
    uint8_t MATRIX_WIDTH;
    uint8_t MATRIX_HEIGHT;
    PxMATRIX display;
    Inputs inputs;
    // constexpr may save SRAM? but it wants them to be static and initialized here
    uint8_t MIN_DELAY;
    uint8_t MAX_DELAY;
    uint8_t SPEED_LOSS;
    uint16_t RESET_DELAY;

    uint8_t gameDelay;

    uint8_t numPlayers;

    uint16_t c_red;
    uint16_t c_green;
    uint16_t c_blue;
    uint16_t c_white;
    uint16_t c_yellow;
    uint16_t c_cyan;
    uint16_t c_magenta;
    uint16_t c_black;

    bool paused;
    bool lastTempState;
    bool doPauseToggle;
    unsigned long lastDebounceTime;
    void checkForPause();

    unsigned long msCurrent;
    unsigned long msPrevious;
    void loopGame();

public:
    Tetris(uint8_t MATRIX_WIDTH, uint8_t MATRIX_HEIGHT, PxMATRIX display, Inputs inputs);
    void setPlayers(uint8_t numPlayers);
};