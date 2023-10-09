#include "Arduino.h"
#include <LinkedList.h>
#include <PxMatrix.h>
#include <Utility.h>
#include <BaseGame.h>

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

class Tetris : public BaseGame
{
public:
    Tetris(Utility &utility, uint8_t numPlayers);
    void setPlayers(uint8_t players);
    bool loopGame();
    bool justStarted;

private:
    uint8_t MIN_DELAY;
    uint8_t MAX_DELAY;
    uint8_t SPEED_LOSS;

    void checkForPause();

};