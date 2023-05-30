#ifndef _snake_
#define _snake_

#include "Arduino.h"
#include <LinkedList.h>
#include <PxMatrix.h>
#include <InputsFonts.h>

struct Point
{
    uint8_t x;
    uint8_t y;

    Point(uint8_t x, uint8_t y)
    {
        Serial.println("creating new Point...");
        this->x = x;
        this->y = y;
    }
    bool isEqual(int x, int y)
    {
        return this->x == x && this->y == y;
    }
};

class SnakeGame
{
public:
    SnakeGame(uint8_t MATRIX_WIDTH, uint8_t MATRIX_HEIGHT, PxMATRIX display, Inputs inputs);
    void setPlayers(uint8_t numPlayers);
    void loopGame();

private:
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

    unsigned long msCurrent;
    unsigned long msPrevious;
    unsigned long lastDebounceTime;

    bool paused;
    bool lastTempState;
    bool doPauseToggle;

    enum direction
        {
            UP,
            DOWN,
            LEFT,
            RIGHT
        };

    class Snake
    {
    public:
        uint8_t MATRIX_WIDTH;
        uint8_t MATRIX_HEIGHT;
        int player;
        int score;
        LinkedList<Point *> segments;
        direction currectDirection;
        Point *getInitialPosition();
        bool occupiesPoint(int x, int y);
        bool isNextPointValid(Point *p);
        Point *getNextPosition();
    };

    Snake snakes[2];    // for now just setting this to maximum players

    Point *applePosition;
    uint8_t numPlayers;

    uint16_t c_red;
    uint16_t c_green;
    uint16_t c_blue;
    uint16_t c_white;
    uint16_t c_yellow;
    uint16_t c_cyan;
    uint16_t c_magenta;
    uint16_t c_black;

    Point *getApplePosition();
    void updateSnakeDirections();
    // Point *getHead();
    // Point *getTail();
    // void addToBeginning(Point *p);
    // void removeTail();
    
    void drawApple();
    void drawSnakes();
    void increaseSpeed();
    void checkForPause();
    void resetSnakes();
    void resetApple();
    // void checkForApple(Point *nextPoint);
    void gameOver();
};

#endif