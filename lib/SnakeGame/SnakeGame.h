#ifndef _snake_
#define _snake_

#include "Arduino.h"
#include <LinkedList.h> // https://www.etlcpp.com/home.html     std::list? std::vector?
#include <PxMatrix.h>
#include <Utility.h>
#include <BaseGame.h>
/*
Changes:
-!!!!apples spawn inside autoSnake!!!!
    -I've seen an apple on top of the initial snake position when the game begins in regular mode (not auto)
        -when that happens, the apple disappears when the snake begins moving but nothing is added to the score and a new apple is not created
            -the space is probably cleared when the snake leaves it but the apple is still there (just invisible)
-Only draw what is necessary, don't clear screen > DONE but needs added to auto snake
-Make winner message indicate the position of the player with an arrow.
-#BUG_1 getNewApplePosition() could lock up if there are a lot of spaces occupied or unlucky RNG
    -I found that if there are no open spaces for a new apple the game just hangs

AI:
-for auto snake, maybe keep track of min/max X & Y occupied by snake and aim for more open part of screen if snake spans all the way across?

Ideas:
-add an option to just stay at same speed whole game
-each player can choose their color
-Use SPIFFS to store a high score file

-after 1 player crashes other snake can keep going?
-option for each player to have their own board? maybe initially start off with separate boards and then they combine?

Power-ups
    -shoot out a temporary wall that the other player's head can't run into
    -speed up/slow down
-Obstacles?
-Maybe moving apples in straight line (could be diagonal)? (slower than the snake speed?)
    Would have to check if only the head of the snake got the apple (currently the case)
-Should snakes collide? If this is added, change occupiesPoint() [see note on that function]

-Alternate game mode: Tron-style securing territory
*/

// MenuInfo inline getSnakeMenu()
// {
//     MenuInfo snakeMenu("SNAKE");
//     bool autoplayMode = true;
//     bool onePlayerMode = true;
//     bool twoPlayerMode = true;
//     snakeMenu.setMenuInfo(autoplayMode, onePlayerMode, twoPlayerMode);
//     // SnakeGame *snakeGamePointer = nullptr;
//     // snakeMenu.gamePointer = snakeGamePointer;

//     return snakeMenu;
// }

struct Point
{
    uint8_t x{0};
    uint8_t y{0};
    Point() = default;
    Point(uint8_t x, uint8_t y) : x(x), y(y)
    {
    }
    bool isEqual(int x, int y)
    {
        return this->x == x && this->y == y;
    }
};

class SnakeGame : public BaseGame
{
public:
    SnakeGame(Utility &utility, uint8_t numPlayers);
    ~SnakeGame();
    void setPlayers(uint8_t players);
    bool loopGame();
    // bool justStarted;
    // void setPixelSize(uint8_t newValue);
    void setNumApples(uint8_t newValue);
    void setStartSpeed(uint8_t newValue);
    void setMaxSpeed(uint8_t newValue);

private:
    class Snake
    {
        const uint8_t FIELD_WIDTH;
        const uint8_t FIELD_HEIGHT;

    public:
        Snake(uint8_t player, uint8_t FIELD_WIDTH, uint8_t FIELD_HEIGHT);
        const uint8_t player;
        uint8_t score;
        uint16_t color;
        uint16_t colorPaused;
        direction lastDirectionMoved;
        direction priorToLastDirectionMoved;
        direction aimedDirection;
        direction firstAimedDirectionAttempted;
        LinkedList<Point> segments; // this could probably be a pointer to or array of Point objects if Point had a "(struct?) Point *next;" member and the Snake stored the head (tail would point to NULL)
        Point getInitialPosition();
        bool occupiesPoint(const int &x, const int &y);
        bool isNextPointValid(const Point &p);
        Point getNextPosition();
        void setColors(uint16_t color, uint16_t colorPaused);
    };

    static const uint8_t FRAME_THICKNESS = 0; // 2   MATRIX_WIDTH - (2 * FRAME_THICKNESS) and ...
    static const uint8_t FRAME_Y_OFFSET = 0;  // 1   ... MATRIX_HEIGHT - (2 * FRAME_THICKNESS + 2 * FRAME_Y_OFFSET) ...
    static const uint8_t PIXEL_SIZE = 1;      // 2  ... must be a multiple of PIXEL_SIZE <- only matters if using PIXEL_SIZE of 3+
    // static uint8_t PIXEL_SIZE;
    const uint8_t FIELD_WIDTH;
    const uint8_t FIELD_HEIGHT;

    static const uint8_t MAX_NUM_APPLES = 5;
    uint8_t numApples = 3; // !!!!!!!!!!changed this applePositions now has MAX spots but only the ones set by option should be filled

    uint8_t MIN_DELAY;
    uint8_t MAX_DELAY;
    uint8_t SPEED_LOSS;

    Snake *snakes[2] = {}; // initializes to nullptrs, each snake is deleted in destructor

    Point applePositions[MAX_NUM_APPLES]; // TODO: once snake gets long this should be reduced #BUG_1

    // void (BaseGame::*myFuncPointer) () = static_cast<void (BaseGame::*)()>(&SnakeGame::setPixelSize);

    // GameOption = {"name", defaultValue, minValue, maxValue, &setterFunction}
    // GameOption pixelSizeGameOption = {"Pixel size", 4, 1, 8, static_cast<void (BaseGame::*)(uint8_t)>(&SnakeGame::setPixelSize)};
    // GameOption numberOfApplesGameOption = {"Num apples", 3, 1, 5, static_cast<void (BaseGame::*)(uint8_t)>(&SnakeGame::setNumApples)};
    // GameOption startSpeedGameOption = {"Start speed", 5, 1, 10, static_cast<void (BaseGame::*)(uint8_t)>(&SnakeGame::setStartSpeed)}; // need to convert a speed value to ms (so inverse): 265 - (speed * 15)
    // GameOption maxSpeedGameOption = {"Max speed", 7, 1, 10, static_cast<void (BaseGame::*)(uint8_t)>(&SnakeGame::setMaxSpeed)};       // 60 - (speed * 5)

    Point getNewApplePosition();
    void updateSnakeDirections();

    void drawFrame();
    void drawScore();

    void drawApple(uint8_t appleNum);
    void drawSnakes();
    void clearTail(Point p);
    void drawSnakeHeads();

    void increaseSpeed();
    void checkForPause();
    void resetSnakes();
    void resetApple(uint8_t appleNum);

    void gameOver();

    bool autoDirectionSet;
    bool autoCollision;
    uint8_t distanceToApple;
    void autoDrawSnake();
    void autoDrawScore();
    bool autoLoopGame();
    bool autoCheckForQuit();
};

#endif