#include "SnakeGame.h"

Point SnakeGame::Snake::getInitialPosition()
{
    return Point(player * 20, 50);
}

SnakeGame::Snake::Snake(uint8_t player, uint8_t FRAME_X_MIN, uint8_t FRAME_X_MAX, uint8_t FRAME_Y_MIN, uint8_t FRAME_Y_MAX) : player(player),
                                                                                                                              FRAME_X_MIN(FRAME_X_MIN),
                                                                                                                              FRAME_X_MAX(FRAME_X_MAX),
                                                                                                                              FRAME_Y_MIN(FRAME_Y_MIN),
                                                                                                                              FRAME_Y_MAX(FRAME_Y_MAX)

{
    score = 0;
    lastDirection = UP;
    currentDirection = UP;
    segments = LinkedList<Point>();
    Point p = getInitialPosition();
    segments.add(p);
}

bool SnakeGame::Snake::occupiesPoint(const int &x, const int &y)
{
    for (int i = 0; i < segments.size() - 1; i++) // ignores tail segment because that will be in a different place
    {                                             // if adding snake collision this should be changed because when getting apple the
        if (segments.get(i).isEqual(x, y))        // tail isn't deleted
        {
            return true;
        }
    }
    return false;
}

// make sure the next point for the head of the snake is in a valid position
bool SnakeGame::Snake::isNextPointValid(const Point &p) // TODO: for other game modes, check if hits another player's snake
{
    // check if within boundary or in the snake
    if (p.x < FRAME_X_MIN || p.x > FRAME_X_MAX || p.y < FRAME_Y_MIN || p.y > FRAME_Y_MAX || occupiesPoint(p.x, p.y))
    {
        return false;
    }

    return true;
}

// calculate the next position based on the current head position and the current direction
Point SnakeGame::Snake::getNextPosition()
{
    Point head = segments.get(0);
    switch (currentDirection)
    {
    case UP:
        return Point(head.x, head.y - 1);
    case DOWN:
        return Point(head.x, head.y + 1);
    case LEFT:
        return Point(head.x - 1, head.y);
    case RIGHT:
        return Point(head.x + 1, head.y);
    default: // NOT VALID current_direction
        return Point();
    }
}

void SnakeGame::Snake::setColors(uint16_t color, uint16_t colorPaused)
{
    this->color = color;
    this->colorPaused = colorPaused;
}

Point SnakeGame::getNewApplePosition()
{
    int x, y;
    bool insideSnake = false;

    do
    {
        uint8_t width = FRAME_X_MAX - FRAME_X_MIN;
        uint8_t height = FRAME_Y_MAX - FRAME_Y_MIN;
        x = random(width) + FRAME_X_MIN;
        y = random(height) + FRAME_Y_MIN;
        for (int i = 0; i < numPlayers; i++)
        {
            Snake *s = snakes[i];

            insideSnake = insideSnake || s->occupiesPoint(x, y);
        }
    } while (insideSnake);

    return Point(x, y);
}

SnakeGame::SnakeGame(Utility &utility, u_int8_t numPlayers) : BaseGame{utility, numPlayers},
                                                              FRAME_X_MIN(1),                        // 1
                                                              FRAME_X_MAX(utility.MATRIX_WIDTH - 2), // 62
                                                              FRAME_Y_MIN(3),                        // 3
                                                              FRAME_Y_MAX(utility.MATRIX_HEIGHT - 4) // 60
{
    MIN_DELAY = 10;
    MAX_DELAY = 200; // max value for uint8_t is 255
    SPEED_LOSS = 10;
    GAME_OVER_DELAY = 10000;

    updateDelay = MAX_DELAY;

    msCurrent = 0;
    msPrevious = 0;

    paused = false;
    justStarted = true;

    for (int i = 0; i < numPlayers; i++)
    {
        snakes[i] = new Snake(i + 1, FRAME_X_MIN, FRAME_X_MAX, FRAME_Y_MIN, FRAME_Y_MAX);
        if (i == 0)
        {
            snakes[i]->setColors(utility.colors.orange, utility.colors.red); // why utility. and not utility-> ??
        }
        else if (i == 1)
        {
            snakes[i]->setColors(utility.colors.purple, utility.colors.pink);
        }
    }

    applePosition = getNewApplePosition();
}

void SnakeGame::setPlayers(uint8_t players)
{
    numPlayers = players;
    for (int i = 0; i < numPlayers; i++)
    {
        if (snakes[i] == nullptr)
        {
            snakes[i] = new Snake(i + 1, FRAME_X_MIN, FRAME_X_MAX, FRAME_Y_MIN, FRAME_Y_MAX);
            if (i == 0)
            {
                snakes[i]->setColors(utility->colors.orange, utility->colors.red);
            }
            else if (i == 1)
            {
                snakes[i]->setColors(utility->colors.purple, utility->colors.pink);
            }
        }
    }
}

void SnakeGame::updateSnakeDirections()
{
    for (int i = 0; i < numPlayers; i++)
    {
        Snake *s = snakes[i];

        bool _up;
        bool _down;
        bool _left;
        bool _right;

        if (s->player == 1)
        {
            utility->inputs.update2(utility->inputs.pins.p1Directions);
            _up = utility->inputs.UP_P1_pressed;
            _down = utility->inputs.DOWN_P1_pressed;
            _left = utility->inputs.LEFT_P1_pressed;
            _right = utility->inputs.RIGHT_P1_pressed;
        }
        else if (s->player == 2)
        {
            utility->inputs.update2(utility->inputs.pins.p2Directions);
            _up = utility->inputs.UP_P2_pressed;
            _down = utility->inputs.DOWN_P2_pressed;
            _left = utility->inputs.LEFT_P2_pressed;
            _right = utility->inputs.RIGHT_P2_pressed;
        }

        // prevents moving back against yourself, also favors switching directions if 2 directions are held simultaneously
        // turning around at fast speeds is janky because of holding down 2 directions for a split second
        if (_up && s->lastDirection != UP && s->lastDirection != DOWN)
        {
            s->currentDirection = UP;
        }
        else if (_down && s->lastDirection != DOWN && s->lastDirection != UP)
        {
            s->currentDirection = DOWN;
        }
        else if (_right && s->lastDirection != RIGHT && s->lastDirection != LEFT)
        {
            s->currentDirection = RIGHT;
        }
        else if (_left && s->lastDirection != LEFT && s->lastDirection != RIGHT)
        {
            s->currentDirection = LEFT;
        }
        // if no input detected or already moving in desired direction or attempting to move back on self, just leave currentDirection as-is
    }
}

void SnakeGame::drawFrame()
{
    uint8_t width = FRAME_X_MAX - FRAME_X_MIN + 3;
    uint8_t height = FRAME_Y_MAX - FRAME_Y_MIN + 3;
    display.drawRect(FRAME_X_MIN - 1, FRAME_Y_MIN - 1, width, height, paused ? utility->colors.yellow : utility->colors.red);
}

void SnakeGame::drawScore() // TODO: make this more readable by converting groups of 5 to 1 pixel of a different color?
{
    for (int i = 0; i < numPlayers; i++)
    {
        Snake *s = snakes[i];
        if (s->player == 1)
        {
            for (int i = 0; i < s->score; i++)
            {
                display.drawPixel(i, MATRIX_WIDTH - 1, utility->colors.magenta);
            }
        }
        else if (s->player == 2)
        {
            for (int i = 0; i < s->score; i++)
            {
                display.drawPixel(MATRIX_WIDTH - 1 - i, 0, utility->colors.magenta);
            }
        }
    }
}

void SnakeGame::drawApple()
{
    display.drawPixel(applePosition.x, applePosition.y, paused ? utility->colors.cyan : utility->colors.red);
}

void SnakeGame::drawSnakes()
{
    for (int i = 0; i < numPlayers; i++)
    {
        Snake *s = snakes[i];

        for (int i = 0; i < s->segments.size(); i++)
        {
            Point p = s->segments.get(i);
            display.drawPixel(p.x, p.y, paused ? s->colorPaused : s->color);
        }
    }
}

void SnakeGame::increaseSpeed()
{
    if (updateDelay > MIN_DELAY)
    {
        updateDelay -= SPEED_LOSS;
    }
}

void SnakeGame::checkForPause()
{
    utility->inputs.update2(utility->inputs.pins.startButton);

    if (utility->inputs.START)
    {
        paused = !paused;
        display.clearDisplay();
        drawFrame();
        drawScore();
        drawSnakes();
        drawApple();
        msPrevious = millis();
    }
}

void SnakeGame::resetSnakes()
{
    for (int i = 0; i < numPlayers; i++)
    {
        Snake *s = snakes[i];

        while (s->segments.size() > 0)
        {
            s->segments.pop();
        }
        s->segments.add(s->getInitialPosition());

        s->currentDirection = UP;
        s->score = 0;
    }
}

void SnakeGame::resetApple()
{
    applePosition = getNewApplePosition();
}

void SnakeGame::gameOver()
{
    display.fillScreen(utility->colors.red);
    delay(250);
    display.clearDisplay();
    delay(250);
    display.fillScreen(utility->colors.yellow);
    delay(250);
    display.clearDisplay();
    delay(250);
    display.fillScreen(utility->colors.red);
    delay(250);
    display.clearDisplay();

    display.setTextColor(utility->colors.cyan);
    display.setFont();
    display.setCursor(1, 1);

    if (numPlayers == 2)
    {
        display.setFont(utility->fonts.my5x5boxy);
        uint8_t scores[2];

        for (int i = 0; i < numPlayers; i++)
        {
            Snake *s = snakes[i];
            scores[i] = s->score;
        }
        if (scores[0] > scores[1])
        {
            display.print("Player 1 wins!");
        }
        else if (scores[0] > scores[1])
        {
            display.print("Player 2 wins!");
        }
        else
        {
            display.print("It's a draw!");
        }
    }

    display.setFont();

    for (int i = 0; i < numPlayers; i++)
    {
        Snake *s = snakes[i];

        display.setCursor(1, (s->player) * 8 + 1);
        display.print("P");
        display.print(s->player);
        display.print(": ");
        display.print(s->score);
    }

    resetSnakes();
    resetApple();
    updateDelay = MAX_DELAY;

    delay(GAME_OVER_DELAY);
}

bool SnakeGame::loopGame()
{
    if (justStarted)
    {
        display.clearDisplay();
        drawFrame();
        drawSnakes();
        drawApple();
        delay(1000);
        justStarted = false;
    }

    checkForPause();

    if (!paused)
    {
        updateSnakeDirections();
        msCurrent = millis();
        if ((msCurrent - msPrevious) > updateDelay)
        {
            // updateSnakeDirections();
            bool snakeGotApple = false;  // would it better if these were declared outside of the loop? so it isn't
            bool snakeCollision = false; // allocating new memory every time the loop runs

            for (int i = 0; i < numPlayers; i++)
            {
                Snake *s = snakes[i];

                Point nextPoint = s->getNextPosition();
                if (s->isNextPointValid(nextPoint))
                {
                    s->segments.add(0, nextPoint);
                    s->lastDirection = s->currentDirection;

                    if (applePosition.isEqual(nextPoint.x, nextPoint.y)) // check if snake got the apple
                    {
                        s->score++;
                        snakeGotApple = true;
                    }
                    else
                    {
                        s->segments.pop();
                    }
                }
                else // snake has hit something
                {
                    if (numPlayers == 2)
                    {
                        if (s->score - 3 >= 0)
                        {
                            s->score -= 3;
                        }
                        else
                        {
                            s->score = 0;
                        }
                    }
                    snakeCollision = true;
                }
            }

            if (snakeGotApple)
            {
                resetApple();
                increaseSpeed();
            }

            if (snakeCollision)
            {
                gameOver();
                return false;
            }
            else    // instead of clearing and redrawing, maybe only draw if something changes
            {       //    when popping a snake segment clear that pixel
                    //    when adding a segment draw it
                    //    when new apple is made draw it
                display.clearDisplay();
                drawFrame();
                drawScore();
                drawSnakes();
                drawApple();
            }

            msPrevious = msCurrent;
        }
    }
    return true;
}
