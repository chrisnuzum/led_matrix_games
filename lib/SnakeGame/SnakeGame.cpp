#include "SnakeGame.h"

// check if the x y coordinates are covered by a part of the snake
bool SnakeGame::Snake::containsPosition(int x, int y)
{
    for (int i = 0; i < snakePositions.size(); i++)
    {
        if (snakePositions.get(i)->isEqual(x, y))
        {
            return true;
        }
    }
    return false;
}

// generate the position of the apple so it is not out of bounds or within the snake
Point *SnakeGame::getApplePosition()
{
    int x = random(MATRIX_WIDTH - 1);
    int y = random(MATRIX_HEIGHT - 1);

    // make sure to generate an apple that is not in the same position as the snake
    while (snakeContainsPosition(x, y))
    {
        x = random(MATRIX_WIDTH - 1);
        y = random(MATRIX_HEIGHT - 1);
    }
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
    RESET_DELAY = 5000;

    game_delay = MAX_DELAY;

    ms_current = 0;
    ms_previous = 0;
    last_debounce_time = 0;

    paused = false;
    last_temp_state = HIGH;
    do_pause_toggle = false;

    currect_direction = UP;

    score = 0;

    applePosition = getApplePosition();

    snakePositions = LinkedList<Point *>();
    snakePositions.add(new Point(50, 50));

    c_red = display.color565(255, 0, 0);
    c_green = display.color565(0, 255, 0);
    c_blue = display.color565(0, 0, 255);
    c_white = display.color565(255, 255, 255);
    c_yellow = display.color565(255, 255, 0);
    c_cyan = display.color565(0, 255, 255);
    c_magenta = display.color565(255, 0, 255);
    c_black = display.color565(0, 0, 0);
}

void SnakeGame::setPlayers(uint8_t num_players)
{
    this->num_players = num_players;
}

void SnakeGame::updateCurrentDirection()
{
    inputs.update();
    bool _up = inputs.UP_P1 == LOW;
    bool _down = inputs.DOWN_P1 == LOW;
    bool _left = inputs.LEFT_P1 == LOW;
    bool _right = inputs.RIGHT_P1 == LOW;

    // prevents from moving back against yourself, also favors switching directions if 2 directions are held simultaneously
    if (_up && currect_direction != UP && currect_direction != DOWN)
    {
        currect_direction = UP;
        return;
    }
    if (_down && currect_direction != DOWN && currect_direction != UP)
    {
        currect_direction = DOWN;
        return;
    }
    if (_right && currect_direction != RIGHT && currect_direction != LEFT)
    {
        currect_direction = RIGHT;
        return;
    }
    if (_left && currect_direction != LEFT && currect_direction != RIGHT)
    {
        currect_direction = LEFT;
        return;
    }

    // if no input detected just leave current_direction as-is
}

Point *SnakeGame::getHead()
{
    return snakePositions.get(0);
}

Point *SnakeGame::getTail()
{
    return snakePositions.get(snakePositions.size() - 1);
}

void SnakeGame::addToBeginning(Point *p)
{
    snakePositions.add(0, p);
}

void SnakeGame::removeTail()
{
    delete (snakePositions.pop());
}

// calculate the next position based on the current head position and the current direction
Point *SnakeGame::getNextPosition()
{
    Point *head = getHead();
    switch (currect_direction)
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

// make sure the next point for the head of the snake is in a valid position
bool SnakeGame::isNextPointValid(Point *p)
{
    int x = p->x;
    int y = p->y;

    // check if within boundary or in the snake
    if (x < 0 || x >= MATRIX_WIDTH || y < 0 || y >= MATRIX_HEIGHT || snakeContainsPosition(x, y))
    {
        return false;
    }

    return true;
}

// draw the apple
void SnakeGame::renderApple()
{
    display.drawPixel(applePosition->x, applePosition->y, paused ? c_cyan : c_red);
}

// draw the snake
void SnakeGame::renderSnake()       // IT BREAKS HERE, FURTHER TESTING NEEDED. ADD PRINTLNS HERE
{
    Point *p;
    for (int i = 0; i < snakePositions.size(); i++)
    {
        p = snakePositions.get(i);
        display.drawPixel(p->x, p->y, paused ? c_blue : c_green);
    }
}

void SnakeGame::increaseSpeed()
{
    if (game_delay > MIN_DELAY)
    {
        game_delay -= SPEED_LOSS;
    }
}

void SnakeGame::checkForPause()
{
    inputs.update();
    bool temp_state = inputs.START;
    if (temp_state)
    {
        if (!last_temp_state)
        {
            last_debounce_time = millis();
            do_pause_toggle = true;
        }
        if (millis() - last_debounce_time > 50 && do_pause_toggle)
        {
            paused = !paused;
            Serial.print("Paused: ");
            Serial.println(paused);
            display.clearDisplay();
            renderSnake();
            renderApple();
            ms_previous = millis();
            do_pause_toggle = false;
        }
    }
    last_temp_state = temp_state;
}

// delete the snake and create a new one
void SnakeGame::resetSnake()
{
    while (snakePositions.size() > 0)
    {
        delete (snakePositions.pop());
    }
    snakePositions.add(getStartingPosition());
}

// delete the current position and draw a new apple
void SnakeGame::resetApple()
{
    delete (applePosition);
    applePosition = getApplePosition();
}

void SnakeGame::checkForApple(Point *nextPoint)
{
    // if we land on an apple grow the snake
    if (applePosition->isEqual(nextPoint->x, nextPoint->y))
    {
        score++;
        addToBeginning(nextPoint);
        resetApple();
        increaseSpeed();
    }
    else
    {
        addToBeginning(nextPoint);
        removeTail();
    }
}

// show an end screen and reset the game state
void SnakeGame::resetGame()
{
    resetSnake();
    resetApple();
    game_delay = MAX_DELAY;
    currect_direction = UP;

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
    display.print("SCORE:");
    display.print(score);
    display.setFont(&Tiny3x3a2pt7b);
    display.setCursor(1, 12);
    display.print("SCORE:");
    display.print(score);
    display.setFont(&TomThumb); // 2 shorter
    display.setCursor(1, 20);
    display.print("SCORE:");
    display.print(score);
    display.setFont(&Org_01); // 1
    display.setCursor(1, 28);
    display.print("SCORE:");
    display.print(score);
    display.setFont(&Picopixel);
    display.setCursor(1, 35);
    display.print("SCORE:");
    display.print(score);
    score = 0;

    // use SPIFFS to store a high score file

    delay(RESET_DELAY + 20000);
}

void SnakeGame::loopGame()
{
    checkForPause();

    if (!paused)
    {
        ms_current = millis();
        if ((ms_current - ms_previous) > game_delay)
        {
            updateCurrentDirection();
            Point *nextPoint = getNextPosition();
            if (isNextPointValid(nextPoint))
            {
                display.clearDisplay();
                checkForApple(nextPoint);
                renderSnake();
                renderApple();
            }
            else
            {
                resetGame();
            }
            // displayFPS();
            ms_previous = ms_current;
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
