#ifndef _snake_
#define _snake_

#include "Arduino.h"
#include <LinkedList.h> // https://www.etlcpp.com/home.html     std::list? std::vector?
#include <PxMatrix.h>
#include <Utility.h>
#include <BaseGame.h>
/*
Changes:
-Make winner message indicate the position of the player with an arrow.

Ideas:
-Multiple apples at a time (maybe only in 2-player)
-Maybe allow either player to initiate a 1-player game, with it selecting the proper controls depending on the button pushed?
-Obstacles?
-Maybe moving apples in straight line (could be diagonal)? (slower than the snake speed?)
    Would have to check if only the head of the snake got the apple (currently the case)
-Should snakes collide? If this is added, change occupiesPoint() [see note on that function]
-Use SPIFFS to store a high score file

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
        const uint8_t FRAME_X_MIN;
        const uint8_t FRAME_X_MAX;
        const uint8_t FRAME_Y_MIN;
        const uint8_t FRAME_Y_MAX;

    public:
        Snake(uint8_t player, uint8_t FRAME_X_MIN, uint8_t FRAME_X_MAX, uint8_t FRAME_Y_MIN, uint8_t FRAME_Y_MAX);
        const uint8_t player;
        uint8_t score;
        uint16_t color;
        uint16_t colorPaused;
        direction lastDirection;
        direction currentDirection;
        LinkedList<Point> segments;
        Point getInitialPosition();
        bool occupiesPoint(const int &x, const int &y);
        bool isNextPointValid(const Point &p);
        Point getNextPosition();
        void setColors(uint16_t color, uint16_t colorPaused);
    };

    const uint8_t FRAME_X_MIN;
    const uint8_t FRAME_X_MAX;
    const uint8_t FRAME_Y_MIN;
    const uint8_t FRAME_Y_MAX;

    uint8_t MIN_DELAY;
    uint8_t MAX_DELAY;
    uint8_t SPEED_LOSS;

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