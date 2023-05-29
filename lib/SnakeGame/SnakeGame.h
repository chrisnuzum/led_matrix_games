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
    void setPlayers(uint8_t num_players);
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

    uint8_t game_delay;

    unsigned long ms_current;
    unsigned long ms_previous;
    unsigned long last_debounce_time;

    bool paused;
    bool last_temp_state;
    bool do_pause_toggle;

    uint8_t score;

    class Snake
    {
        LinkedList<Point *> snakePositions;
        enum direction
        {
            UP,
            DOWN,
            LEFT,
            RIGHT
        } currect_direction;
        bool containsPosition(int x, int y);
    };

    Snake snakes[2];

    Point *applePosition;
    uint8_t num_players;

    uint16_t c_red;
    uint16_t c_green;
    uint16_t c_blue;
    uint16_t c_white;
    uint16_t c_yellow;
    uint16_t c_cyan;
    uint16_t c_magenta;
    uint16_t c_black;

    Point *getApplePosition();
    void updateCurrentDirection();
    Point *getHead();
    Point *getTail();
    void addToBeginning(Point *p);
    void removeTail();
    Point *getNextPosition();
    bool isNextPointValid(Point *p);
    void renderApple();
    void renderSnake();
    void increaseSpeed();
    void checkForPause();
    void resetSnake();
    void resetApple();
    void checkForApple(Point *nextPoint);
    void resetGame();
};

#endif