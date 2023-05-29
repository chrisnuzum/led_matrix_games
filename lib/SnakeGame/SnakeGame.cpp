#include "SnakeGame.h"

// check if the point is occupied by the snake
bool SnakeGame::Snake::occupiesPoint(int x, int y)
{
    for (int i = 0; i < segmentPositions.size(); i++)
    {
        if (segmentPositions.get(i)->isEqual(x, y))
        {
            return true;
        }
    }
    return false;
}

// make sure the next point for the head of the snake is in a valid position
bool SnakeGame::Snake::isNextPointValid(Point *p)
{
    int x = p->x;
    int y = p->y;

    // check if within boundary or in the snake
    if (x < 0 || x >= MATRIX_WIDTH || y < 0 || y >= MATRIX_HEIGHT || occupiesPoint(x, y))
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
    RESET_DELAY = 5000;

    game_delay = MAX_DELAY;

    ms_current = 0;
    ms_previous = 0;
    last_debounce_time = 0;

    paused = false;
    last_temp_state = HIGH;
    do_pause_toggle = false;

    score = 0;

    for (int i = 0; i < num_players; i++)
    {
        snakes[i].player = i + 1;
        snakes[i].currectDirection = UP;
        snakes[i].segmentPositions = LinkedList<Point *>();
        snakes[i].segmentPositions.add(new Point((i + 1) * 20, 50));
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

void SnakeGame::setPlayers(uint8_t num_players)
{
    this->num_players = num_players;
}

void SnakeGame::updateCurrentDirection()
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

void SnakeGame::addToBeginning(Point *p)
{
    snakePositions.add(0, p);
}

void SnakeGame::removeTail()
{
    delete (snakePositions.pop());
}

// calculate the next position based on the current head position and the current direction
Point *SnakeGame::getNextPositions()        // THIS DOESN'T WORK, WILL RETURN FROM FIRST SNAKE. probably pass a snake instead or have this function in the Snake class
{
    for (Snake s : snakes)
    {
        Point *head = s.segmentPositions.get(0);
        switch (s.currectDirection)
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
}

// draw the apple
void SnakeGame::renderApple()
{
    display.drawPixel(applePosition->x, applePosition->y, paused ? c_cyan : c_red);
}

// draw the snake
void SnakeGame::renderSnake()
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

void SnakeGame::checkForApple(Point *nextPoint)     // needs to check per snake
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
