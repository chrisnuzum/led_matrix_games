#ifndef _snake_
#define _snake_

#include "Arduino.h"
#include <LinkedList.h>
#include <PxMatrix.h>
#include <Utility.h>
#include <BaseGame.h>
/*
Changes:
Right now if both snakes reach apple at the same time, only P1 gets it.
"Buffer" direction changes. This would prevent the situation of quickly
    clicking and releasing a direction not doing anything on slow speeds.
    Not sure if this would need to be implemented in Utility code or here...
In 2-player snakes should be different color.
Announce the winner at the end.

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
    enum direction
    {
        UP,
        DOWN,
        LEFT,
        RIGHT
    };

    class Snake
    {
        const uint8_t FRAME_X_MIN;
        const uint8_t FRAME_X_MAX;
        const uint8_t FRAME_Y_MIN;
        const uint8_t FRAME_Y_MAX;

    public:
        Snake(uint8_t player, uint8_t FRAME_X_MIN, uint8_t FRAME_X_MAX, uint8_t FRAME_Y_MIN, uint8_t FRAME_Y_MAX);
        uint8_t player;
        uint8_t score;
        LinkedList<Point> segments;
        direction currentDirection;
        Point getInitialPosition();
        bool occupiesPoint(const int &x, const int &y);
        bool isNextPointValid(const Point &p);
        Point getNextPosition();
    };

    // const uint8_t MATRIX_WIDTH;
    // const uint8_t MATRIX_HEIGHT;
    const uint8_t FRAME_X_MIN;
    const uint8_t FRAME_X_MAX;
    const uint8_t FRAME_Y_MIN;
    const uint8_t FRAME_Y_MAX;

    // PxMATRIX display;
    // Utility *utility; 
    // uint8_t numPlayers;
    uint8_t MIN_DELAY;
    uint8_t MAX_DELAY;
    uint8_t SPEED_LOSS;
    // uint16_t GAME_OVER_DELAY;

    // uint8_t updateDelay;

    // unsigned long msCurrent;
    // unsigned long msPrevious;

    // bool paused;

    Snake *snakes[2] = {nullptr};   // probably need a destructor to clear this out when switching games

    Point applePosition;

    Point getNewApplePosition();
    void updateSnakeDirections();

    void drawFrame();
    void drawScore();
    void drawApple();
    void drawSnakes();
    void increaseSpeed();
    void checkForPause();
    void resetSnakes();
    void resetApple();

    void gameOver();
};

#endif