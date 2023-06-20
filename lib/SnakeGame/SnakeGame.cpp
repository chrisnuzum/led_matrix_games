#include "SnakeGame.h"

Point Snake::getInitialPosition()
{
    Serial.print("creating starting position for player ");
    Serial.println(player);
    return Point(player * 20, 50);
}

Snake::Snake(uint8_t MATRIX_WIDTH, uint8_t MATRIX_HEIGHT, uint8_t player) : MATRIX_WIDTH(MATRIX_WIDTH),
                                                                            MATRIX_HEIGHT(MATRIX_HEIGHT),
                                                                            player(player)
{
    score = 0;
    currentDirection = UP;
    segments = LinkedList<Point>();
    segments.add(getInitialPosition());
}

// check if the point is occupied by the snake
bool Snake::occupiesPoint(const int &x, const int &y)
{
    for (int i = 0; i < segments.size(); i++)
    {
        Serial.print("checking if ");
        Serial.print(x);
        Serial.print(", ");
        Serial.print(y);
        Serial.print(" is equal to ");
        Serial.print(segments.get(i).x);
        Serial.print(", ");
        Serial.println(segments.get(i).y);
        if (segments.get(i).isEqual(x, y))
        {
            Serial.println("yes");
            return true;
        }
        Serial.println("no");
    }
    return false;
}

// make sure the next point for the head of the snake is in a valid position
bool Snake::isNextPointValid(const Point &p) // TODO: for other game modes, check if hits another player's snake
{
    // check if within boundary or in the snake
    if (p.x < 0 || p.x >= MATRIX_WIDTH || p.y < 0 || p.y >= MATRIX_HEIGHT || occupiesPoint(p.x, p.y))
    {
        return false;
    }

    return true;
}

// choose an apple position that is not within a snake
Point SnakeGame::getApplePosition()
{
    int x, y;
    bool insideSnake = false;

    do
    {
        Serial.println("start of getApplePosition do for loop");
        x = random(MATRIX_WIDTH - 1);
        y = random(MATRIX_HEIGHT - 1);
        // Snake s = *snakes;
        // insideSnake = insideSnake || s.occupiesPoint(x, y);
        insideSnake = insideSnake || snakeP1.occupiesPoint(x, y);
        Serial.println("end of getApplePosition do for loop");
    } while (insideSnake);

    // Serial.print("SnakeGame getApplePosition() snakes->segments.get(0): ");
    // Serial.print(snakes->segments.get(0).x);
    // Serial.print(", ");
    // Serial.println(snakes->segments.get(0).y);
    // Serial.print("   address of snakes->segments.get(0): ");
    // Serial.println((long)&snakes->segments.get(0), HEX);
    // Serial.print("   address of snakes->segments.get(0)->x: ");
    // Serial.println((long)&snakes->segments.get(0).x, HEX);
    // Serial.print("   address of snakes->segments.get(0)->y: ");
    // Serial.println((long)&snakes->segments.get(0).y, HEX);
    // Serial.print("   address of snakes: ");
    // Serial.println((long)snakes, HEX);

    return Point(x, y);
}

SnakeGame::SnakeGame(Utility utility, uint8_t numPlayers) : utility(utility),
                                                            numPlayers(numPlayers),
                                                            display(utility.display)
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

    // for (int i = 0; i < numPlayers; i++)
    // {
    //     Serial.print("Setting up Snake ");
    //     Serial.println(i);
    //     // snakes[i] = new Snake(MATRIX_WIDTH, MATRIX_HEIGHT, i + 1);
    //     snakes = new Snake(MATRIX_WIDTH, MATRIX_HEIGHT, i + 1);
    // }
    snakeP1 = Snake(MATRIX_WIDTH, MATRIX_HEIGHT, 1);

    Serial.println("getting apple position");

    applePosition = getApplePosition();

    // Serial.print("   address of applePosition: ");
    // Serial.println((long)&applePosition, HEX);
    // Serial.print("   address of applePosition.x: ");
    // Serial.println((long)&applePosition.x, HEX);
    // Serial.print("   address of applePosition.y: ");
    // Serial.println((long)&applePosition.y, HEX);

    // Serial.print("SnakeGame after getApplePosition() snakes->segments.get(0): ");
    // Serial.print(snakes->segments.get(0).x);
    // Serial.print(", ");
    // Serial.println(snakes->segments.get(0).y);
    // Point p = snakes->segments.get(0);
    // Serial.println((long)&p);
    // // Serial.print("   address of snakes->segments.get(0): ");
    // // Serial.println((long)&snakes->segments.get(0), HEX);
    // // Serial.print("   address of snakes->segments.get(0)->x: ");
    // // Serial.println((long)&snakes->segments.get(0).x, HEX);
    // // Serial.print("   address of snakes->segments.get(0)->y: ");
    // // Serial.println((long)&snakes->segments.get(0).y, HEX);
    // Serial.print("   address of snakes: ");
    // Serial.println((long)snakes, HEX);

    Serial.println("got apple position");

    c_red = display.color565(255, 0, 0);
    c_green = display.color565(0, 255, 0);
    c_blue = display.color565(0, 0, 255);
    c_white = display.color565(255, 255, 255);
    c_yellow = display.color565(255, 255, 0);
    c_cyan = display.color565(0, 255, 255);
    c_magenta = display.color565(255, 0, 255);
    c_black = display.color565(0, 0, 0);

    Serial.println("done constructor");
}

void SnakeGame::updateSnakeDirections()
{
    utility.inputs.update();

    // for (int i = 0; i < numPlayers; i++)
    // {
    // Snake s = *snakes[i];
    Serial.println("updating snake direction for loop");
    bool _up;
    bool _down;
    bool _left;
    bool _right;

    _up = utility.inputs.UP_P1_active;
    _down = utility.inputs.DOWN_P1_active;
    _left = utility.inputs.LEFT_P1_active;
    _right = utility.inputs.RIGHT_P1_active;

    // // if (s.player == 1)
    // if (snakes->player == 1)
    // {
    //     Serial.println("current is player 1");
    //     _up = utility.inputs.UP_P1_active;
    //     _down = utility.inputs.DOWN_P1_active;
    //     _left = utility.inputs.LEFT_P1_active;
    //     _right = utility.inputs.RIGHT_P1_active;
    //     Serial.println("done setting temp direction vars");
    // }
    // // else if (s.player == 2)
    // else if (snakes->player == 2)
    // {
    //     _up = utility.inputs.UP_P2_active;
    //     _down = utility.inputs.DOWN_P2_active;
    //     _left = utility.inputs.LEFT_P2_active;
    //     _right = utility.inputs.RIGHT_P2_active;
    // }

    // prevents moving back against yourself, also favors switching directions if 2 directions are held simultaneously
    if (_up && snakeP1.currentDirection != UP && snakeP1.currentDirection != DOWN)
    {
        Serial.println("up");
        snakeP1.currentDirection = UP;
    }
    else if (_down && snakeP1.currentDirection != DOWN && snakeP1.currentDirection != UP)
    {
        Serial.println("down");
        snakeP1.currentDirection = DOWN;
    }
    else if (_right && snakeP1.currentDirection != RIGHT && snakeP1.currentDirection != LEFT)
    {
        Serial.println("right");
        snakeP1.currentDirection = RIGHT;
    }
    else if (_left && snakeP1.currentDirection != LEFT && snakeP1.currentDirection != RIGHT)
    {
        Serial.println("left");
        snakeP1.currentDirection = LEFT;
    }
    Serial.println("checked all");
    // if no input detected or already moving in desired direction or attempting to move back on self, just leave currentDirection as-is
    // }
    Serial.println("done for loop in updateSnakeDirections");
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
Point Snake::getNextPosition()
{
    Serial.println("start of getNextPosition()");
    Point head = segments.get(0);
    Serial.print("head - x: ");
    Serial.print(head.x);
    Serial.print(", y: ");
    Serial.println(head.y);
    switch (currentDirection)
    {
    case UP:
        Serial.println("Up");
        return Point(head.x, head.y - 1);
    case DOWN:
        Serial.println("Down");
        return Point(head.x, head.y + 1);
    case LEFT:
        Serial.println("Left");
        return Point(head.x - 1, head.y);
    case RIGHT:
        Serial.println("Right");
        return Point(head.x + 1, head.y);
    default:
        Serial.println("NOT VALID current_direction");
        return Point();
    }
}

void SnakeGame::drawApple()
{
    display.drawPixel(applePosition.x, applePosition.y, paused ? c_cyan : c_red);
}

void SnakeGame::drawSnakes()
{
    // for (int i = 0; i < numPlayers; i++)
    // {
    //     // Snake s = *snakes[i];
    //     Snake s = *snakes;
    // for (int i = 0; i < s.segments.size(); i++)
    for (int i = 0; i < snakeP1.segments.size(); i++)
    {
        Point p = snakeP1.segments.get(i);
        display.drawPixel(p.x, p.y, paused ? c_blue : c_green);
    }
    // }
}

void SnakeGame::increaseSpeed()
{
    if (gameDelay > MIN_DELAY)
    {
        gameDelay -= SPEED_LOSS;
    }
}

void SnakeGame::checkForPause() // most of this is obsolete and already incorporated into Input library
{
    Serial.print("checkForPause() begin head - ");
    Serial.print(snakeP1.segments.get(0).x);
    Serial.print(", ");
    Serial.println(snakeP1.segments.get(0).y);

    utility.inputs.update();
    bool tempState = utility.inputs.START;
    Serial.print("tempState: ");
    Serial.println(tempState);
    if (tempState)
    {
        Serial.println("in check for pause if block");
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

    Serial.print("checkForPause() end head - ");
    Serial.print(snakeP1.segments.get(0).x);
    Serial.print(", ");
    Serial.println(snakeP1.segments.get(0).y);
}

// delete snakes and create new ones
void SnakeGame::resetSnakes()
{
    Serial.println("beginning of resetSnakes()");
    // for (int i = 0; i < numPlayers; i++)
    // {
    //     // Snake s = *snakes[i];
    //     Snake s = *snakes;
    // while (s.segments.size() > 0)
    while (snakeP1.segments.size() > 0)
    {
        // delete (s.segments.pop());
        Serial.println("segment pop");
        snakeP1.segments.pop();
    }
    Serial.println("adding initial segment");
    snakeP1.segments.add(snakeP1.getInitialPosition());
    // }
    Serial.println("end of resetSnakes()");
}

void SnakeGame::resetApple()
{
    Serial.println("beginning of resetApple()");
    // delete (applePosition);
    applePosition = getApplePosition();
    Serial.println("end of resetApple()");
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
    // for (int i = 0; i < numPlayers; i++)
    // {
    //     // Snake s = *snakes[i];
    //     // Snake s = *snakes;
    //     Snake s = snakeP1;
    display.setCursor(1, (snakeP1.player - 1) * 8 + 1);
    display.print("P");
    display.print(snakeP1.player);
    display.print(": ");
    display.print(snakeP1.score);
    // }
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
    // for (int i = 0; i < numPlayers; i++)
    // {
    //     // Snake s = *snakes[i];
    //     // Snake s = *snakes;
    //     Snake s = snakeP1;
    snakeP1.currentDirection = UP;
    snakeP1.score = 0;
    // } // probably in future go back to main menu

    delay(RESET_DELAY);
    Serial.println("end of gameOver()");
}

void SnakeGame::loopGame()
{
    Serial.println();
    Serial.println();
    Serial.println();
    // Serial.print("loopGame() snakes->segments.get(0): ");
    // Serial.print(snakes->segments.get(0).x);
    // Serial.print(", ");
    // Serial.println(snakes->segments.get(0).y);
    Serial.print("loopGame() begin head - ");
    Serial.print(snakeP1.segments.get(0).x);
    Serial.print(", ");
    Serial.println(snakeP1.segments.get(0).y);

    checkForPause();

    Serial.print("loopGame() after pause check head - ");
    Serial.print(snakeP1.segments.get(0).x);
    Serial.print(", ");
    Serial.println(snakeP1.segments.get(0).y);

    Serial.println("done check for pause");
    if (!paused)
    {
        msCurrent = millis();
        if ((msCurrent - msPrevious) > gameDelay)
        {
            Serial.print("loopGame() before dirs head - ");
            Serial.print(snakeP1.segments.get(0).x);
            Serial.print(", ");
            Serial.println(snakeP1.segments.get(0).y);

            updateSnakeDirections();

            Serial.print("loopGame() after dirs head - ");
            Serial.print(snakeP1.segments.get(0).x);
            Serial.print(", ");
            Serial.println(snakeP1.segments.get(0).y);

            Serial.println("done check for directions");
            bool snakeCollision = false;
            for (int i = 0; i < numPlayers; i++)
            {
                Serial.println("start of Snake for loopGame loop");
                // Snake s = *snakes[i];
                // Snake s = *snakes;
                Serial.print("loopGame() head - ");
                Serial.print(snakeP1.segments.get(0).x);
                Serial.print(", ");
                Serial.println(snakeP1.segments.get(0).y);
                Point nextPoint = snakeP1.getNextPosition();
                if (snakeP1.isNextPointValid(nextPoint))
                {
                    Serial.println("next point is valid");
                    snakeP1.segments.add(0, nextPoint);

                    // check if snake got the apple
                    if (applePosition.isEqual(nextPoint.x, nextPoint.y))
                    {
                        Serial.println("snake got apple");
                        snakeP1.score++;
                        resetApple();
                        increaseSpeed();
                    }
                    else
                    {
                        Serial.println("snake didn't get apple");
                        // delete (s.segments.pop());
                        snakeP1.segments.pop();
                    }
                }
                else
                {
                    Serial.println("next point not valid");
                    snakeCollision = true;
                }
                Serial.println("end of Snake for loopGame loop");
            }
            if (snakeCollision)
            {
                Serial.println("game over");
                gameOver();
            }
            else
            {
                Serial.println("draw scene");
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
