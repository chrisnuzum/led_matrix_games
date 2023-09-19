#ifndef _snake_
#define _snake_

#include "Arduino.h"
#include <LinkedList.h>
#include <PxMatrix.h>
#include <Utility.h>
    /*
    Changes:
    Right now if both snakes reach apple at the same time, only P1 gets it.
    "Buffer" direction changes. This would prevent the situation of quickly
        clicking and releasing a direction not doing anything on slow speeds.
        Not sure if this would need to be implemented in Utility code or here...
    In 2-player snakes should be different color.

    Ideas:
    Multiple apples at a time (maybe only in 2-player).
    Obstacles?
    Maybe moving apples in straight line (could be diagonal)? (slower than the snake speed?)
        Would have to check if only the head of the snake got the apple (currently the case)
    Should snakes collide?
    
    */
struct Point
{
    uint8_t x{0};
    uint8_t y{0};
    Point() = default;
    Point(uint8_t x, uint8_t y) : x(x), y(y)
    {
        Serial.print("creating new Point of x: ");
        Serial.print(x);
        Serial.print(", y: ");
        Serial.println(y);
    }
    bool isEqual(int x, int y)
    {
        return this->x == x && this->y == y;
    }
};

enum direction
{
    UP,
    DOWN,
    LEFT,
    RIGHT
};

class Snake
{
    uint8_t MATRIX_WIDTH;
    uint8_t MATRIX_HEIGHT;

public:
    // Snake() : Snake(1, 64, 64){};
    Snake(uint8_t player, uint8_t MATRIX_WIDTH, uint8_t MATRIX_HEIGHT);
    // Snake(uint8_t MATRIX_WIDTH, uint8_t MATRIX_HEIGHT);
    uint8_t player;
    uint8_t score;
    LinkedList<Point> segments;
    direction currentDirection;
    // void setPlayer(uint8_t playerNum);
    Point getInitialPosition();
    bool occupiesPoint(const int &x, const int &y);
    bool isNextPointValid(const Point &p);
    Point getNextPosition();
};

class SnakeGame
{
public:
    SnakeGame(Utility &utility, uint8_t numPlayers);
    // void setPlayers(uint8_t numPlayers);
    bool loopGame();

private:
    const uint8_t MATRIX_WIDTH;
    const uint8_t MATRIX_HEIGHT;
    PxMATRIX display;
    Utility *utility;
    uint8_t numPlayers;
    // constexpr may save SRAM? but it wants them to be static and initialized here
    uint8_t MIN_DELAY;
    uint8_t MAX_DELAY;
    uint8_t SPEED_LOSS;
    uint16_t RESET_DELAY;

    uint8_t gameDelay;

    unsigned long msCurrent;
    unsigned long msPrevious;

    bool justStarted;
    bool paused;

    // enum direction
    // {
    //     UP,
    //     DOWN,
    //     LEFT,
    //     RIGHT
    // };

    // class Snake
    // {
    //     uint8_t MATRIX_WIDTH;
    //     uint8_t MATRIX_HEIGHT;
    // public:
    //     // Snake();
    //     Snake(uint8_t MATRIX_WIDTH, uint8_t MATRIX_HEIGHT, uint8_t player);
    //     uint8_t player;
    //     uint8_t score;
    //     LinkedList<Point *> segments;
    //     direction currentDirection;
    //     Point *getInitialPosition();
    //     bool occupiesPoint(int x, int y);
    //     bool isNextPointValid(Point *p);
    //     Point *getNextPosition();
    // };

    // Snake *snakes;
    Snake snakeP1 = Snake(1, 64, 64);
    Snake snakeP2 = Snake(2, 64, 64);
    // Try going back to pointers to snakes or an array of pointers to snakes. Seems like the Snake objects need to be
    // created here in the header though.

    Point applePosition;

    uint16_t c_red;
    uint16_t c_green;
    uint16_t c_blue;
    uint16_t c_white;
    uint16_t c_yellow;
    uint16_t c_cyan;
    uint16_t c_magenta;
    uint16_t c_black;

    Point getApplePosition();
    void updateSnakeDirections();

    void drawApple();
    void drawSnakes();
    void increaseSpeed();
    void checkForPause();
    void resetSnakes();
    void resetApple();

    void gameOver();
};

#endif