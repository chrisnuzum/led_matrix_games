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
-Only draw what is necessary, don't clear screen > DONE but needs added to auto snake
-Make winner message indicate the position of the player with an arrow.

AI:
-for auto snake, maybe keep track of min/max X & Y occupied by snake and aim for more open part of screen if snake spans all the way across?

Ideas:
-getNewApplePosition() could lock up if there are a lot of spaces occupied or unlucky RNG

-after 1 player crashes other snake can keep going?

Power-ups
    -shoot out a temporary wall that the other player's head can't run into
    -speed up/slow down
-Obstacles?
-Maybe moving apples in straight line (could be diagonal)? (slower than the snake speed?)
    Would have to check if only the head of the snake got the apple (currently the case)
-Should snakes collide? If this is added, change occupiesPoint() [see note on that function]
-Use SPIFFS to store a high score file
-Alternate game mode with snakes colliding with one another, or a Tron-style securing territory
-Each player can choose their color
*/
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
    void setPlayers(uint8_t players);
    bool loopGame();
    bool justStarted;

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
        LinkedList<Point> segments;
        Point getInitialPosition();
        bool occupiesPoint(const int &x, const int &y);
        bool isNextPointValid(const Point &p);
        Point getNextPosition();
        void setColors(uint16_t color, uint16_t colorPaused);
    };

    static const uint8_t FRAME_THICKNESS = 0; // 2 MATRIX_WIDTH - (2 * FRAME_THICKNESS) and
    static const uint8_t FRAME_Y_OFFSET = 0;  // 1 MATRIX_HEIGHT - (2 * FRAME_THICKNESS + 2 * FRAME_Y_OFFSET)
    static const uint8_t PIXEL_SIZE = 16;      // 2 must be a multiple of PIXEL_SIZE < only matters if using PIXEL_SIZE of 3+
    const uint8_t FIELD_WIDTH;
    const uint8_t FIELD_HEIGHT;

    static const uint8_t NUM_APPLES = 5;

    uint8_t MIN_DELAY;
    uint8_t MAX_DELAY;
    uint8_t SPEED_LOSS;

    Snake *snakes[2] = {}; // initializes to nullptrs, probably need a destructor to clear this out when switching games

    Point applePositions[NUM_APPLES]; // TODO: once snake gets long this should be reduced

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
};

#endif