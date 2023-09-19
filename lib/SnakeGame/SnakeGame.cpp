#include "SnakeGame.h"

// void Snake::setPlayer(uint8_t playerNum)
// {
//     player = playerNum;
//     Point p = getInitialPosition();     // bleh
//     segments.add(p);                    // bleh
// }

Point Snake::getInitialPosition()
{
    Serial.print("creating starting position for player ");
    Serial.println(player);
    return Point(player * 20, 50);
}

// Snake::Snake(uint8_t MATRIX_WIDTH, uint8_t MATRIX_HEIGHT) : MATRIX_WIDTH(MATRIX_WIDTH),
//                                                             MATRIX_HEIGHT(MATRIX_HEIGHT)
Snake::Snake(uint8_t player, uint8_t MATRIX_WIDTH, uint8_t MATRIX_HEIGHT) : player(player),
                                                                            MATRIX_WIDTH(MATRIX_WIDTH),
                                                                            MATRIX_HEIGHT(MATRIX_HEIGHT)

{
    score = 0;
    currentDirection = UP;
    segments = LinkedList<Point>();
    Point p = getInitialPosition(); //
    segments.add(p);                //
    Serial.println("done making snake");
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

// calculate the next position based on the current head position and the current direction
Point Snake::getNextPosition()
{
    Point head = segments.get(0);
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
        // for (int i = 0; i < numPlayers; i++)
        // {
        Snake *s = &snakeP1;
        // if (i == 1)
        // {
        //     // s = snakeP2;
        // }
        insideSnake = insideSnake || s->occupiesPoint(x, y);
        if (numPlayers == 2)
        {
            s = &snakeP2;
            insideSnake = insideSnake || s->occupiesPoint(x, y);
        }
        // }
        Serial.println("end of getApplePosition do for loop");
    } while (insideSnake);

    return Point(x, y);
}

SnakeGame::SnakeGame(Utility &utility, u_int8_t numPlayers) : MATRIX_WIDTH(utility.MATRIX_WIDTH),
                                                              MATRIX_HEIGHT(utility.MATRIX_HEIGHT),
                                                              utility(&utility),
                                                              numPlayers(numPlayers),
                                                              display(utility.display)
{
    MIN_DELAY = 10;
    MAX_DELAY = 200; // max value for uint8_t
    SPEED_LOSS = 10;
    RESET_DELAY = 10000;

    gameDelay = MAX_DELAY;

    msCurrent = 0;
    msPrevious = 0;

    paused = false;
    justStarted = true;

    // for (int i = 0; i < numPlayers; i++)
    // {
    //     Serial.print("Setting up Snake ");
    //     Serial.println(i);
    //     // snakes[i] = new Snake(MATRIX_WIDTH, MATRIX_HEIGHT, i + 1);
    //     snakes = new Snake(MATRIX_WIDTH, MATRIX_HEIGHT, i + 1);
    // }
    // snakeP1 = Snake(MATRIX_WIDTH, MATRIX_HEIGHT, 1);     // messes up

    // snakeP1.setPlayer(1);
    // snakeP2.setPlayer(2);

    Serial.println("getting apple position");

    applePosition = getApplePosition();

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
    utility->inputs.update();

    // for (int i = 0; i < numPlayers; i++)
    // {
    //     Serial.print("number of players: ");
    //     Serial.println(numPlayers);

    Snake *s = &snakeP1;
    // if (i == 1)
    // {
    //     // s = snakeP2;
    // }
    bool _up;
    bool _down;
    bool _left;
    bool _right;

    _up = utility->inputs.UP_P1_active;
    _down = utility->inputs.DOWN_P1_active;
    _left = utility->inputs.LEFT_P1_active;
    _right = utility->inputs.RIGHT_P1_active;

    // if (s.player == 1)
    // {
    //     _up = utility->inputs.UP_P1_active;
    //     _down = utility->inputs.DOWN_P1_active;
    //     _left = utility->inputs.LEFT_P1_active;
    //     _right = utility->inputs.RIGHT_P1_active;
    // }
    // else if (s.player == 2)
    // {
    //     _up = utility->inputs.UP_P2_active;
    //     _down = utility->inputs.DOWN_P2_active;
    //     _left = utility->inputs.LEFT_P2_active;
    //     _right = utility->inputs.RIGHT_P2_active;
    // }

    // prevents moving back against yourself, also favors switching directions if 2 directions are held simultaneously
    if (_up && s->currentDirection != UP && s->currentDirection != DOWN)
    {
        Serial.println("up");
        s->currentDirection = UP;
    }
    else if (_down && s->currentDirection != DOWN && s->currentDirection != UP)
    {
        Serial.println("down");
        s->currentDirection = DOWN;
    }
    else if (_right && s->currentDirection != RIGHT && s->currentDirection != LEFT)
    {
        Serial.println("right");
        s->currentDirection = RIGHT;
    }
    else if (_left && s->currentDirection != LEFT && s->currentDirection != RIGHT)
    {
        Serial.println("left");
        s->currentDirection = LEFT;
    }
    Serial.println("checked inputs for P1");
    // if no input detected or already moving in desired direction or attempting to move back on self, just leave currentDirection as-is
    // }
    if (numPlayers == 2)
    {
        s = &snakeP2;
        _up = utility->inputs.UP_P2_active;
        _down = utility->inputs.DOWN_P2_active;
        _left = utility->inputs.LEFT_P2_active;
        _right = utility->inputs.RIGHT_P2_active;

        // prevents moving back against yourself, also favors switching directions if 2 directions are held simultaneously
        if (_up && s->currentDirection != UP && s->currentDirection != DOWN)
        {
            Serial.println("up");
            s->currentDirection = UP;
        }
        else if (_down && s->currentDirection != DOWN && s->currentDirection != UP)
        {
            Serial.println("down");
            s->currentDirection = DOWN;
        }
        else if (_right && s->currentDirection != RIGHT && s->currentDirection != LEFT)
        {
            Serial.println("right");
            s->currentDirection = RIGHT;
        }
        else if (_left && s->currentDirection != LEFT && s->currentDirection != RIGHT)
        {
            Serial.println("left");
            s->currentDirection = LEFT;
        }
        Serial.println("checked inputs for P2");
    }
    Serial.println("done updateSnakeDirections");
}

void SnakeGame::drawApple()
{
    display.drawPixel(applePosition.x, applePosition.y, paused ? c_cyan : c_red);
}

void SnakeGame::drawSnakes()
{
    // for (int i = 0; i < numPlayers; i++)
    // {
    Snake *s = &snakeP1;
    // if (i == 1)
    // {
    //     // s = snakeP2;
    // }
    for (int i = 0; i < s->segments.size(); i++)
    {
        Point p = s->segments.get(i);
        display.drawPixel(p.x, p.y, paused ? c_blue : c_green);
    }
    if (numPlayers == 2)
    {
        s = &snakeP2;
        for (int i = 0; i < s->segments.size(); i++)
        {
            Point p = s->segments.get(i);
            display.drawPixel(p.x, p.y, paused ? c_blue : c_green);
        }
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

void SnakeGame::checkForPause()
{
    utility->inputs.update();

    if (utility->inputs.START)
    {
        paused = !paused;
        Serial.print("Paused: ");
        Serial.println(paused);
        display.clearDisplay();
        drawSnakes();
        drawApple();
        msPrevious = millis();
        Serial.println("done pause if block");
    }
}

// delete snakes and create new ones
void SnakeGame::resetSnakes()
{
    // for (int i = 0; i < numPlayers; i++)
    // {
    Snake *s = &snakeP1;
    // if (i == 1)
    // {
    //     // s = snakeP2;
    // }
    while (s->segments.size() > 0)
    {
        s->segments.pop();
    }
    s->segments.add(s->getInitialPosition());
    // }
    if (numPlayers == 2)
    {
        s = &snakeP2;
        while (s->segments.size() > 0)
        {
            s->segments.pop();
        }
        s->segments.add(s->getInitialPosition());
    }
}

void SnakeGame::resetApple()
{
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
    display.fillScreen(c_red);
    delay(250);
    display.clearDisplay();
    delay(250);
    display.fillScreen(c_yellow);
    delay(250);
    display.clearDisplay();
    delay(250);
    display.fillScreen(c_red);
    delay(250);
    display.clearDisplay();

    display.setTextColor(c_cyan);
    display.setFont();
    display.setCursor(1, 1);
    // for (int i = 0; i < numPlayers; i++)
    // {
    Snake *s = &snakeP1;
    // if (i == 1)
    // {
    //     // s = snakeP2;
    // }
    display.setCursor(1, (s->player - 1) * 8 + 1);
    display.print("P");
    display.print(s->player);
    display.print(": ");
    display.print(s->score);
    // }
    if (numPlayers == 2)
    {
        s = &snakeP2;
        display.setCursor(1, (s->player - 1) * 8 + 1);
        display.print("P");
        display.print(s->player);
        display.print(": ");
        display.print(s->score);
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
    // for (int i = 0; i < numPlayers; i++)    // REMOVING ALL FOR LOOPS AND CHECKING
    // {
    s = &snakeP1;
    // if (i == 1)
    // {
    //     // s = snakeP2;
    // }
    s->currentDirection = UP;
    s->score = 0;
    if (numPlayers == 2)
    {
        s = &snakeP2;
        s->currentDirection = UP;
        s->score = 0;
    }
    // }

    delay(RESET_DELAY);
    // ESP.restart();
}

bool SnakeGame::loopGame()
{
    if (justStarted)
    {
        display.clearDisplay();
        drawSnakes();
        drawApple();
        delay(1000);
        justStarted = false;    // probably won't reset if played 2 times in a row
    }
    checkForPause();

    Serial.print("done check for pause, paused=");
    Serial.println(paused);

    if (!paused)
    {
        msCurrent = millis();
        if ((msCurrent - msPrevious) > gameDelay)
        {
            updateSnakeDirections();
            Serial.println("return to loopGame() after updateSnakeDirections");
            bool snakeCollision = false;
            Serial.println("before loopGame() for loop");

            // for (int i = 0; i < numPlayers; i++)
            // {
            Serial.println();
            Serial.println("beginning of loopGame player for loop");
            Snake *s = &snakeP1;
            // if (i == 1)
            // {
            //     // s = snakeP2;
            // }
            Serial.println("set Snake");
            Point nextPoint = s->getNextPosition();
            if (s->isNextPointValid(nextPoint))
            {
                Serial.println("next point is valid");
                s->segments.add(0, nextPoint);

                // check if snake got the apple
                if (applePosition.isEqual(nextPoint.x, nextPoint.y))
                {
                    Serial.println("snake got apple");
                    s->score++;
                    resetApple();
                    increaseSpeed();
                }
                else
                {
                    Serial.println("snake didn't get apple");
                    // delete (s.segments.pop());
                    s->segments.pop();
                }
            }
            else
            {
                Serial.println("next point not valid");
                if (numPlayers == 2 && s->score - 3 >= 0)
                {
                    s->score -= 3;
                }
                else
                {
                    s->score = 0;
                }

                snakeCollision = true;
            }
            // }
            if (numPlayers == 2)
            {
                s = &snakeP2;
                Serial.println("set Snake");
                Point nextPoint = s->getNextPosition();
                if (s->isNextPointValid(nextPoint))
                {
                    Serial.println("next point is valid");
                    s->segments.add(0, nextPoint);

                    // check if snake got the apple
                    if (applePosition.isEqual(nextPoint.x, nextPoint.y))
                    {
                        Serial.println("snake got apple");
                        s->score++;
                        resetApple();
                        increaseSpeed();
                    }
                    else
                    {
                        Serial.println("snake didn't get apple");
                        // delete (s.segments.pop());
                        s->segments.pop();
                    }
                }
                else
                {
                    Serial.println("next point not valid");
                    if (numPlayers == 2 && s->score - 3 >= 0)
                    {
                        s->score -= 3;
                    }
                    else
                    {
                        s->score = 0;
                    }

                    snakeCollision = true;
                }
            }

            Serial.println("after loopGame for player loop");
            if (snakeCollision)
            {
                Serial.println("game over");
                gameOver();
                return false;
            }
            else
            {
                Serial.println("no collision, drawing");
                display.clearDisplay();
                drawSnakes();
                drawApple();
                Serial.println("after drawn");
            }

            // displayFPS();
            msPrevious = msCurrent;
        }
    }
    return true;

    // loops++;
    // if (millis() - loop_time > 1000)
    // {
    //     fps = loops;
    //     loop_time = millis();
    //     loops = 0;
    // }
}

// instead of clearing and redrawing, maybe only draw if something changes
//    when popping a snake segment clear that pixel
//    when adding a segment draw it
//    when new apple is made draw it