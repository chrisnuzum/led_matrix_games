#include "SnakeGame.h"

Point *SnakeGame::Snake::getInitialPosition()
{
    return new Point(player * 20, 50);
}

// check if the point is occupied by the snake
bool SnakeGame::Snake::occupiesPoint(int x, int y)
{
    for (int i = 0; i < segments.size(); i++)
    {
        if (segments.get(i)->isEqual(x, y))
        {
            return true;
        }
    }
    return false;
}

// make sure the next point for the head of the snake is in a valid position
bool SnakeGame::Snake::isNextPointValid(Point *p) // TODO: for other game modes, check if hits another player's snake
{
    int x = p->x;
    int y = p->y;

    // check if within boundary or in the snake
    if (x < 0 || x >= MATRIX_WIDTH || y < 0 || y >= MATRIX_HEIGHT || occupiesPoint(x, y)) // TODO: fix matrix size variables
    {
        return false;
    }

    return true;
}

// choose an apple position that is not within a snake
Point *SnakeGame::getApplePosition()
{
    int x, y;
    bool insideSnake = false;

    do
    {
        x = random(MATRIX_WIDTH - 1);
        y = random(MATRIX_HEIGHT - 1);
        for (Snake s : snakes)
        {
            insideSnake = insideSnake || s.occupiesPoint(x, y);
        }
    } while (!insideSnake);

    return new Point(x, y);
}

SnakeGame::SnakeGame(uint8_t MATRIX_WIDTH, uint8_t MATRIX_HEIGHT, PxMATRIX display, Inputs inputs) : MATRIX_WIDTH(MATRIX_WIDTH),
                                                                                                     MATRIX_HEIGHT(MATRIX_HEIGHT),
                                                                                                     display(display),
                                                                                                     inputs(inputs)
{
    MIN_DELAY = 10;
    MAX_DELAY = 255; // max value for uint8_t
    SPEED_LOSS = 5;
    RESET_DELAY = 10000;

    gameDelay = MAX_DELAY;

    msCurrent = 0;
    msPrevious = 0;
    lastDebounceTime = 0;

    paused = false;
    lastTempState = HIGH;
    doPauseToggle = false;

    for (int i = 0; i < numPlayers; i++)
    {
        snakes[i].MATRIX_WIDTH = MATRIX_WIDTH;
        snakes[i].MATRIX_HEIGHT = MATRIX_HEIGHT;
        snakes[i].player = i + 1;
        snakes[i].currectDirection = UP;
        snakes[i].segments = LinkedList<Point *>();
        snakes[i].segments.add(snakes[i].getInitialPosition());
        snakes[i].score = 0;
    }

    applePosition = getApplePosition();

    // currect_direction = UP;

    // snakePositions = LinkedList<Point *>();
    // snakePositions.add(new Point(50, 50));

    c_red = display.color565(255, 0, 0);
    c_green = display.color565(0, 255, 0);
    c_blue = display.color565(0, 0, 255);
    c_white = display.color565(255, 255, 255);
    c_yellow = display.color565(255, 255, 0);
    c_cyan = display.color565(0, 255, 255);
    c_magenta = display.color565(255, 0, 255);
    c_black = display.color565(0, 0, 0);
}

void SnakeGame::setPlayers(uint8_t numPlayers)
{
    this->numPlayers = numPlayers;
}

void SnakeGame::updateSnakeDirections()
{
    inputs.update();

    for (Snake s : snakes)
    {
        bool _up;
        bool _down;
        bool _left;
        bool _right;

        if (s.player == 1)
        {
            _up = inputs.UP_P1_active == LOW;
            _down = inputs.DOWN_P1_active == LOW;
            _left = inputs.LEFT_P1_active == LOW;
            _right = inputs.RIGHT_P1_active == LOW;
        }
        else if (s.player == 2)
        {
            _up = inputs.UP_P2_active == LOW;
            _down = inputs.DOWN_P2_active == LOW;
            _left = inputs.LEFT_P2_active == LOW;
            _right = inputs.RIGHT_P2_active == LOW;
        }

        // prevents moving back against yourself, also favors switching directions if 2 directions are held simultaneously
        if (_up && s.currectDirection != UP && s.currectDirection != DOWN)
        {
            s.currectDirection = UP;
            return;
        }
        if (_down && s.currectDirection != DOWN && s.currectDirection != UP)
        {
            s.currectDirection = DOWN;
            return;
        }
        if (_right && s.currectDirection != RIGHT && s.currectDirection != LEFT)
        {
            s.currectDirection = RIGHT;
            return;
        }
        if (_left && s.currectDirection != LEFT && s.currectDirection != RIGHT)
        {
            s.currectDirection = LEFT;
            return;
        }
        // if no input detected just leave current_direction as-is
    }
}

// Point *SnakeGame::getHead()
// {
//     return snakePositions.get(0);
// }

// Point *SnakeGame::getTail()
// {
//     return snakePositions.get(snakePositions.size() - 1);
// }

// void SnakeGame::addToBeginning(Point *p)
// {
//     snakePositions.add(0, p);
// }

// void SnakeGame::removeTail()
// {
//     delete (snakePositions.pop());
// }

// calculate the next position based on the current head position and the current direction
Point *SnakeGame::Snake::getNextPosition()
{
    Point *head = segments.get(0);
    switch (currectDirection)
    {
    case UP:
        Serial.println("Up");
        return new Point(head->x, head->y - 1);
    case DOWN:
        Serial.println("Down");
        return new Point(head->x, head->y + 1);
    case LEFT:
        Serial.println("Left");
        return new Point(head->x - 1, head->y);
    case RIGHT:
        Serial.println("Right");
        return new Point(head->x + 1, head->y);
    default:
        Serial.println("NOT VALID current_direction");
        return 0;
    }
}

void SnakeGame::drawApple()
{
    display.drawPixel(applePosition->x, applePosition->y, paused ? c_cyan : c_red);
}

void SnakeGame::drawSnakes()
{
    for (Snake s : snakes)
    {
        Point *p;
        for (int i = 0; i < s.segments.size(); i++)
        {
            p = s.segments.get(i);
            display.drawPixel(p->x, p->y, paused ? c_blue : c_green);
        }
    }
}

void SnakeGame::increaseSpeed()
{
    if (gameDelay > MIN_DELAY)
    {
        gameDelay -= SPEED_LOSS;
    }
}

void SnakeGame::checkForPause()
{
    inputs.update();
    bool tempState = inputs.START;
    if (tempState)
    {
        if (!lastTempState)
        {
            lastDebounceTime = millis();
            doPauseToggle = true;
        }
        if (millis() - lastDebounceTime > 50 && doPauseToggle)
        {
            paused = !paused;
            Serial.print("Paused: ");
            Serial.println(paused);
            display.clearDisplay();
            drawSnakes();
            drawApple();
            msPrevious = millis();
            doPauseToggle = false;
        }
    }
    lastTempState = tempState;
}

// delete snakes and create new ones
void SnakeGame::resetSnakes()
{
    for (Snake s : snakes)
    {
        while (s.segments.size() > 0)
        {
            delete (s.segments.pop());
        }
        s.segments.add(s.getInitialPosition());
    }
}

void SnakeGame::resetApple()
{
    delete (applePosition);
    applePosition = getApplePosition();
}

// void SnakeGame::checkForApple(Point *nextPoint) // needs to check per snake
// {
//     // if we land on an apple grow the snake
//     if (applePosition->isEqual(nextPoint->x, nextPoint->y))
//     {
//         score++;
//         // addToBeginning(nextPoint);
//         snakePositions.add(0, nextPoint);
//         resetApple();
//         increaseSpeed();
//     }
//     else
//     {
//         // addToBeginning(nextPoint);
//         snakePositions.add(0, nextPoint);
//         // removeTail();
//         delete (snakePositions.pop());
//     }
// }

// show an end screen and reset the game state
void SnakeGame::gameOver()
{
    display.fillScreen(c_yellow);
    delay(250);
    display.clearDisplay();
    delay(250);
    display.fillScreen(c_yellow);
    delay(250);
    display.clearDisplay();
    delay(250);
    display.fillScreen(c_yellow);
    delay(250);
    display.clearDisplay();

    display.setTextColor(c_cyan);
    display.setFont();
    display.setCursor(1, 1);
    for (Snake s : snakes)
    {
        display.setCursor(1, (s.player - 1) * 8 + 1);
        display.print("P");
        display.print(s.player);
        display.print(": ");
        display.print(s.score);
    }
    // display.print("SCORE:");
    // display.print(score);
    // display.setFont(&Tiny3x3a2pt7b);
    // display.setCursor(1, 12);
    // display.print("SCORE:");
    // display.print(score);
    // display.setFont(&TomThumb); // 2 shorter
    // display.setCursor(1, 20);
    // display.print("SCORE:");
    // display.print(score);
    // display.setFont(&Org_01); // 1
    // display.setCursor(1, 28);
    // display.print("SCORE:");
    // display.print(score);
    // display.setFont(&Picopixel);
    // display.setCursor(1, 35);
    // display.print("SCORE:");
    // display.print(score);

    // use SPIFFS to store a high score file

    resetSnakes();
    resetApple();
    gameDelay = MAX_DELAY;
    for (Snake s : snakes)
    {
        s.currectDirection = UP;
        s.score = 0;
    } // probably in future go back to main menu

    delay(RESET_DELAY);
}

void SnakeGame::loopGame()
{
    checkForPause();

    if (!paused)
    {
        msCurrent = millis();
        if ((msCurrent - msPrevious) > gameDelay)
        {
            updateSnakeDirections();
            bool snakeCollision = false;
            for (Snake s : snakes)
            {
                Point *nextPoint = s.getNextPosition();
                if (s.isNextPointValid(nextPoint))
                {
                    s.segments.add(0, nextPoint);

                    // check if snake got the apple
                    if (applePosition->isEqual(nextPoint->x, nextPoint->y))
                    {
                        s.score++;
                        resetApple();
                        increaseSpeed();
                    }
                    else
                    {
                        delete (s.segments.pop());
                    }
                }
                else
                {
                    snakeCollision = true;
                }
            }
            if (snakeCollision)
            {
                gameOver();
            }
            else
            {
                display.clearDisplay();
                drawSnakes();
                drawApple();
            }

            // displayFPS();
            msPrevious = msCurrent;
        }
    }

    // loops++;
    // if (millis() - loop_time > 1000)
    // {
    //     fps = loops;
    //     loop_time = millis();
    //     loops = 0;
    // }
}
