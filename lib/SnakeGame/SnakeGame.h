#ifndef _snake_
#define _snake_

#include "Arduino.h"
#include <LinkedList.h>
#include <PxMatrix.h>
#include <Utility.h>

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
    Snake() : Snake(64, 64, 1){};
    Snake(uint8_t MATRIX_WIDTH, uint8_t MATRIX_HEIGHT, uint8_t player);
    uint8_t player;
    uint8_t score;
    LinkedList<Point> segments;
    direction currentDirection;
    Point getInitialPosition();
    bool occupiesPoint(const int &x, const int &y);
    bool isNextPointValid(const Point &p);
    Point getNextPosition();
};

class SnakeGame
{
public:
    SnakeGame(const Utility &utility, uint8_t numPlayers);
    // void setPlayers(uint8_t numPlayers);
    void loopGame();

private:
    uint8_t MATRIX_WIDTH;
    uint8_t MATRIX_HEIGHT;
    PxMATRIX display;
    Utility utility;    // this is probably constructing an empty Utility
                // could declare Inputs inputs then in constructor inputs(utility.inputs)
    uint8_t numPlayers;
    // constexpr may save SRAM? but it wants them to be static and initialized here
    uint8_t MIN_DELAY;
    uint8_t MAX_DELAY;
    uint8_t SPEED_LOSS;
    uint16_t RESET_DELAY;

    uint8_t gameDelay;

    unsigned long msCurrent;
    unsigned long msPrevious;

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
    Snake snakeP1;
    // Snake snakeP2;

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