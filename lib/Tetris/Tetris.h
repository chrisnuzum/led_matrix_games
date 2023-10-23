#ifndef _tetris_
#define _tetris_

#include "Arduino.h"
#include <PxMatrix.h>
#include <Utility.h>
#include <BaseGame.h>
/*
    int lengthOfLine = sizeof(iPiece) / sizeof(iPiece[0]);
    // or
    int lengthOfLine2 = sizeof(iPiece[0]) / sizeof(iPiece[0][0]);
    // or
    int lengthOfLine3 = std::end(iPiece[0]) - std::begin(iPiece[0]) ??
    // or store length in the piece or somewhere else

    std::shuffle is preferred over std::random_shuffle (removed in c++17) but requires an extra include (#include <random>)
        auto rd = std::random_device{};
        auto rng = std::default_random_engine{rd()};
        std::shuffle(std::begin(bag), std::end(bag), rng);
    getRandom() enables std::random_shuffle to be truly random with different results each reboot. std::shuffle does not need this^
*/

/*
    Game board:
        10 wide by 20 tall, so "pixels" can be 2x2 and still have room to fit 2 players side-by-side
        with a 1 pixel border around the games and a 2 pixel border between them = 46 horizontally x 42 vertically
        this leaves 18 horizontally x 22 vertically for score & next piece info
        -for single player they could be 3x3, so 30 wide by 60 tall
    Scoring:
        1 point for each piece that lands on board
        1 point for each block in a cleared line, unless multiple lines are cleared at the same time
        Multiplier: lines x mult x blocks = score
            1 line:     1 x 1   x 10 = 10
            2 lines:    2 x 1.5 x 10 = 30
            3 lines:    3 x 2   x 10 = 60
            4 lines:    4 x 2.5 x 10 = 100
        These multi-line clears would be how you get powerups in multiplayer to hurt the other player
        Combos?
    keep array of arrays to track the game board
        array of size 10 for each line
        array items correspond to the color of that pixel (or none)
*/

/*
Multiplayer:
    Multi-line clears interrupt opponent in different ways depending on the number of lines cleared.
    Should each player have their own game speed depending on number of lines cleared? Or could just get faster over time
*/

// controls:
//   joystick:
//     left/right: moves piece left or right
//     down: soft drop, moves piece down faster
//     [maybe] up: hard drop, moves to bottom and locks immediately
//   A: hard drop, moves to bottom and locks immediately OR [maybe] rotate piece counter-clockwise
//   B: rotate piece clockwise

// pieces will be stored in variables ahead of time
// hard-coded or calculated rotation?
//    here are the official rotations with the spawn orientation on the left: https://static.wikia.nocookie.net/tetrisconcept/images/3/3d/SRS-pieces.png/revision/latest?cb=20060626173148
//    I and O spawn in center, others favor left of center
// piece colors:
//    I: light blue/teal        35967
//    J: dark blue              22
//    L: orange                 64512
//    O: yellow                 65504
//    S: green (not dark)       2016
//    T: pink                   63504
//    Z: red                    63488
// "bag" of all seven pieces is randomly ordered from the start
// this ensures that a streak of bad pieces does not occur

// as a piece falls (no input):
//   check to see that each spot under the piece is not occupied
//   if good > drop piece after the current pause
//   if not > lock in current position after the current pause
//   could show where hard drop would land in a lighter color

// when a rotate is requested:
//   check rotation table for desired position of piece
//   if good > display the rotation immediately
//   if rotation won't fit to left(or right) > try rotated position shifted 1 to the right(or left)
//   to kick off of the sides most pieces would only need to shift 1, but I piece could need to shift 2
//   if not good, rotation fails (could try shifting piece up or down like the kick tables)

/*
Copyrighted parts:
    -apparently the tetromino pieces and the 10x20 field size are copyrighted: https://publicknowledge.org/tetris-copyright-decision-shows-how-complicated-copyright-for-games-can-be/

Ideas to avoid issues:
    -piece spawns in center and player can move it in any direction, essentially having 4 options to make lines
*/

/*
TODO:
combine tryMovePiece() and tryLowerPiece() into 1 function because they are basically the same
    utilize direction enum as input to combined function
figure out why TetrisPiece default constructor is getting called (twice)
    without providing default constructor there is an error

*/

class Tetris : public BaseGame
{
public:
    Tetris(Utility &utility, uint8_t numPlayers);
    void setPlayers(uint8_t players);
    bool loopGame();
    bool justStarted;

private:
    const static uint8_t NUM_PIECES = 7;
    const static uint8_t NUM_ROTATIONS = 4;
    const static uint8_t NUM_PIECE_SEGMENTS = 4; // tetronimoes

    const static uint8_t BOARD_X_POSITION = 10;
    const static uint8_t BOARD_Y_POSITION = 23;
    const static uint8_t NEXTPIECE_X_POSITION = 22;
    const static uint8_t NEXTPIECE_Y_POSITION = 13;
    const static uint8_t BOARD_HEIGHT = 20;
    const static uint8_t BOARD_WIDTH = 10;
    const static uint8_t BOARD_PIXEL_SIZE = 2; // 2x2 pixels, should have multiple constants for 1P, 2P, etc.

    class TetrisPiece
    {
    public:
        TetrisPiece(); // without this it has an error for some reason
        TetrisPiece(uint16_t color, uint8_t length, const bool *orientations, bool rotatable, int8_t spawnXOffset, int8_t spawnYOffset);
        uint16_t color;
        uint8_t length;
        const bool *orientations;
        bool rotatable;
        int8_t spawnXOffset;
        int8_t spawnYOffset;
        // everything above is not modified at runtime
    };

    const bool iPieceRaw[4][4][4] =
        {{{0, 0, 0, 0},
          {1, 1, 1, 1},
          {0, 0, 0, 0},
          {0, 0, 0, 0}},
         {{0, 0, 1, 0},
          {0, 0, 1, 0},
          {0, 0, 1, 0},
          {0, 0, 1, 0}},
         {{0, 0, 0, 0},
          {0, 0, 0, 0},
          {1, 1, 1, 1},
          {0, 0, 0, 0}},
         {{0, 1, 0, 0},
          {0, 1, 0, 0},
          {0, 1, 0, 0},
          {0, 1, 0, 0}}};
    const bool jPieceRaw[4][3][3] =
        {{{1, 0, 0},
          {1, 1, 1},
          {0, 0, 0}},
         {{0, 1, 1},
          {0, 1, 0},
          {0, 1, 0}},
         {{0, 0, 0},
          {1, 1, 1},
          {0, 0, 1}},
         {{0, 1, 0},
          {0, 1, 0},
          {1, 1, 0}}};
    const bool lPieceRaw[4][3][3] =
        {{{0, 0, 1},
          {1, 1, 1},
          {0, 0, 0}},
         {{0, 1, 0},
          {0, 1, 0},
          {0, 1, 1}},
         {{0, 0, 0},
          {1, 1, 1},
          {1, 0, 0}},
         {{1, 1, 0},
          {0, 1, 0},
          {0, 1, 0}}};
    const bool oPieceRaw[2][2] =
        {{1, 1},
         {1, 1}};
    const bool sPieceRaw[4][3][3] =
        {{{0, 1, 1},
          {1, 1, 0},
          {0, 0, 0}},
         {{0, 1, 0},
          {0, 1, 1},
          {0, 0, 1}},
         {{0, 0, 0},
          {0, 1, 1},
          {1, 1, 0}},
         {{1, 0, 0},
          {1, 1, 0},
          {0, 1, 0}}};
    const bool tPieceRaw[4][3][3] =
        {{{0, 1, 0},
          {1, 1, 1},
          {0, 0, 0}},
         {{0, 1, 0},
          {0, 1, 1},
          {0, 1, 0}},
         {{0, 0, 0},
          {1, 1, 1},
          {0, 1, 0}},
         {{0, 1, 0},
          {1, 1, 0},
          {0, 1, 0}}};
    const bool zPieceRaw[4][3][3] =
        {{{1, 1, 0},
          {0, 1, 1},
          {0, 0, 0}},
         {{0, 0, 1},
          {0, 1, 1},
          {0, 1, 0}},
         {{0, 0, 0},
          {1, 1, 0},
          {0, 1, 1}},
         {{0, 1, 0},
          {1, 1, 0},
          {1, 0, 0}}};

    const TetrisPiece iPiece; // spawn position is offset 1 up
    const TetrisPiece jPiece;
    const TetrisPiece lPiece;
    const TetrisPiece oPiece; // spawn position is offset 1 right
    const TetrisPiece sPiece;
    const TetrisPiece tPiece;
    const TetrisPiece zPiece;

    class TetrisBoard
    {
        TetrisPiece bag[NUM_PIECES];
        uint8_t nextPieceBagPosition;

        TetrisPiece currentPiece;
        uint8_t currentOrientation;                             // could be direction enum?
        uint8_t currentPieceCoordinates[NUM_PIECE_SEGMENTS][2]; // could use Point struct from SnakeGame for better readability
        int8_t currentPieceTopLeft[2];                          // can be negative

        const static uint8_t SPAWN_X_OFFSET = 3;

        bool addCurrentPieceToBoard();
        void checkForLineClear();
        void addLineClearScore(u_int8_t linesCleared);
        void shuffleBag();
        static int getRandom(int i);

    public:
        TetrisBoard(uint8_t player, const TetrisPiece &iPiece, const TetrisPiece &jPiece, const TetrisPiece &lPiece,
                    const TetrisPiece &oPiece, const TetrisPiece &sPiece, const TetrisPiece &tPiece, const TetrisPiece &zPiece);

        uint8_t player;
        uint16_t score;
        TetrisPiece nextPiece;
        uint16_t board[BOARD_HEIGHT][BOARD_WIDTH] = {0};
        unsigned long lastMove;
        bool gameOver;
        bool softDrop;
        bool updateFlipper;
        bool hardDrop;

        bool tryGetNewPiece();
        bool tryRotatePiece(bool clockwise);
        bool tryMovePiece(bool left);
        bool tryLowerPiece();
        void resetBoard();
    };

    TetrisBoard *boards[2] = {}; // initializes to nullptrs, probably need a destructor to clear this out when switching games
    uint16_t tempBoards[2][BOARD_HEIGHT][BOARD_WIDTH] = {0};

    uint8_t MIN_DELAY;
    uint8_t MAX_DELAY;
    uint8_t SPEED_LOSS;
    uint8_t MOVE_DELAY;

    void drawFrame();
    void drawScore(uint8_t player);
    void clearNextPiece();
    // void clearBoard();
    void drawNextPiece(uint8_t player);
    void drawBoard(uint8_t player);
    void checkForInput();
    void checkForPause();

    void gameOver();
};

#endif