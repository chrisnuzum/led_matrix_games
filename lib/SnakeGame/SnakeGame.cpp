#include "SnakeGame.h"

Point SnakeGame::Snake::getInitialPosition() // TODO: need to offset something by 1
{
    if (player == 1 || player == 0)
    {
        return Point(20 / PIXEL_SIZE, 50 / PIXEL_SIZE);
    }
    else if (player == 2)
    {
        return Point(FIELD_WIDTH - 1 - (20 / PIXEL_SIZE), FIELD_HEIGHT - 1 - (50 / PIXEL_SIZE));
    }
    else
    {
        return Point();
    }
}

SnakeGame::Snake::Snake(uint8_t player, uint8_t FIELD_WIDTH, uint8_t FIELD_HEIGHT) : player(player),
                                                                                     FIELD_WIDTH(FIELD_WIDTH),
                                                                                     FIELD_HEIGHT(FIELD_HEIGHT)

{
    score = 0;

    if (player == 1 || player == 0)
    {
        priorToLastDirectionMoved = RIGHT;
        lastDirectionMoved = UP;
        aimedDirection = UP;
    }
    else if (player == 2)
    {
        lastDirectionMoved = DOWN;
        aimedDirection = DOWN;
    }
    segments = LinkedList<Point>();
    Point p = getInitialPosition();
    segments.add(p);
}

bool SnakeGame::Snake::occupiesPoint(const int &x, const int &y)
{
    for (int i = 0; i < segments.size() - 1; i++) // ignores tail segment because that will be in a different place
    {                                             // if adding 2P snake-snake collision this should be changed because
        if (segments.get(i).isEqual(x, y))        // when getting apple the tail isn't deleted
        {
            return true;
        }
    }
    return false;
}

// make sure the next point for the head of the snake is in a valid position
bool SnakeGame::Snake::isNextPointValid(const Point &p) // TODO: for other game modes, check if hits another player's snake
{
    if (p.x > FIELD_WIDTH - 1 || p.y > FIELD_HEIGHT - 1 || occupiesPoint(p.x, p.y)) // p.x < 0 and p.y < 0 pointless because variables are unsigned
    {
        return false;
    }
    return true;
}

// calculate the next position based on the current head position and the current direction
Point SnakeGame::Snake::getNextPosition()
{
    Point head = segments.get(0);
    switch (aimedDirection)
    {
    case UP:
        return Point(head.x, head.y - 1);
    case DOWN:
        return Point(head.x, head.y + 1);
    case LEFT:
        return Point(head.x - 1, head.y);
    case RIGHT:
        return Point(head.x + 1, head.y);
    default: // NOT VALID aimedDirection
        return Point();
    }
}

void SnakeGame::Snake::setColors(uint16_t color, uint16_t colorPaused)
{
    this->color = color;
    this->colorPaused = colorPaused;
}

// void SnakeGame::setPixelSize(uint8_t newValue)
// {
//     PIXEL_SIZE = newValue;
// }

void SnakeGame::setNumApples(uint8_t newValue)
{
    Serial.print("Setting numApples to: ");
    Serial.println(newValue, 10);
    numApples = newValue;
}

void SnakeGame::setStartSpeed(uint8_t newValue)
{
    Serial.print("Setting MAX_DELAY to: ");
    Serial.print(newValue, 10);
    MAX_DELAY = 265 - (newValue * 15);
    updateDelay = MAX_DELAY;
    Serial.print(" > ");
    Serial.println(MAX_DELAY, 10);
}

void SnakeGame::setMaxSpeed(uint8_t newValue)
{
    Serial.print("Setting MIN_DELAY to: ");
    Serial.print(newValue, 10);
    MIN_DELAY = 60 - (newValue * 5);
    Serial.print(" > ");
    Serial.println(MIN_DELAY, 10);
}

Point SnakeGame::getNewApplePosition()
{
    int x, y;
    bool newPositionInvalid = false;
    // int _count = 0;
    do
    {
        newPositionInvalid = false;
        x = random(FIELD_WIDTH); // random(x) returns a number from 0 to x - 1
        y = random(FIELD_HEIGHT);

        for (int i = 0; i < numPlayers; i++)
        {
            Snake *s = snakes[i];

            newPositionInvalid = newPositionInvalid || s->occupiesPoint(x, y);
        }
        for (int i = 0; i < numApples; i++)
        {
            newPositionInvalid = newPositionInvalid || applePositions[i].isEqual(x, y);
        }
    } while (newPositionInvalid);

    return Point(x, y);
}

SnakeGame::SnakeGame(Utility &utility, uint8_t numPlayers) : BaseGame{utility},
                                                             FIELD_WIDTH((utility.MATRIX_WIDTH - 2 * FRAME_THICKNESS) / PIXEL_SIZE),
                                                             FIELD_HEIGHT((utility.MATRIX_HEIGHT - (2 * FRAME_THICKNESS + 2 * FRAME_Y_OFFSET)) / PIXEL_SIZE)
{
    MIN_DELAY = 25;  // 10
    MAX_DELAY = 180; // 200; max value for uint8_t is 255
    SPEED_LOSS = 5;
    GAME_OVER_DELAY = 10000;

    updateDelay = MAX_DELAY;

    msPrevious = 0;

    paused = false;
    justStarted = true;

    setPlayers(numPlayers);

    for (int i = 0; i < numApples; i++)
    {
        applePositions[i] = getNewApplePosition();
    }
}

SnakeGame::~SnakeGame()
{
    delete snakes[0];
    if (numPlayers > 1)
    {
        for (int i = 1; i < numPlayers; i++)
        {
            delete snakes[i];
        }
    }
}

void SnakeGame::setPlayers(uint8_t players)
{
    numPlayers = players;

    if (numPlayers == 0)
    {
        snakes[0] = new Snake(0, MATRIX_WIDTH / PIXEL_SIZE, MATRIX_HEIGHT / PIXEL_SIZE);
        snakes[0]->setColors(utility->colors.orange, utility->colors.red);
        autoDirectionSet = false;
        distanceToApple = 100;
        updateDelay = 1; // 10
    }
    else
    {
        for (int i = 0; i < numPlayers; i++)
        {
            if (snakes[i] == nullptr)
            {
                // Serial.println("snakes[i] == nullptr");
                snakes[i] = new Snake(i + 1, FIELD_WIDTH, FIELD_HEIGHT); // TODO: need to rerun this if FIELD changes size from user option
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
        // if (_up && s->lastDirectionMoved != UP && s->lastDirectionMoved != DOWN)
        if (_up && s->lastDirectionMoved != DOWN)
        {
            s->aimedDirection = UP;
        }
        // else if (_down && s->lastDirectionMoved != DOWN && s->lastDirectionMoved != UP)
        else if (_down && s->lastDirectionMoved != UP)
        {
            s->aimedDirection = DOWN;
        }
        // else if (_right && s->lastDirectionMoved != RIGHT && s->lastDirectionMoved != LEFT)
        else if (_right && s->lastDirectionMoved != LEFT)
        {
            s->aimedDirection = RIGHT;
        }
        // else if (_left && s->lastDirectionMoved != LEFT && s->lastDirectionMoved != RIGHT)
        else if (_left && s->lastDirectionMoved != RIGHT)
        {
            s->aimedDirection = LEFT;
        }
        // if no input detected or already moving in desired direction or attempting to move back on self, just leave aimedDirection as-is
    }
}

void SnakeGame::drawFrame()
{
    uint8_t width = MATRIX_WIDTH;
    uint8_t height = MATRIX_HEIGHT - 2 * FRAME_Y_OFFSET;

    for (int i = 0; i < FRAME_THICKNESS; i++)
    {
        utility->display.drawRect(i, i + FRAME_Y_OFFSET, width - 2 * i, height - 2 * i, paused ? utility->colors.purple : utility->colors.blueDark);
    }
}

void SnakeGame::drawScore() // Idea: make this more readable by converting groups of 5 to 1 pixel of a different color?
{                           // TODO: In its current rendition it should only add pixels not redraw all of them
    for (int i = 0; i < numPlayers; i++)
    {
        Snake *s = snakes[i];
        if (s->player == 1)
        {
            for (int i = 0; i < s->score; i++)
            {
                utility->display.drawPixel(i, MATRIX_HEIGHT - 1, utility->colors.magenta);
            }
        }
        else if (s->player == 2)
        {
            for (int i = 0; i < s->score; i++)
            {
                utility->display.drawPixel(MATRIX_WIDTH - 1 - i, 0, utility->colors.magenta);
            }
        }
    }
}

void SnakeGame::drawApple(uint8_t appleNum)
{
    for (int rowOffset = 0; rowOffset < PIXEL_SIZE; rowOffset++)
    {
        for (int colOffset = 0; colOffset < PIXEL_SIZE; colOffset++)
        {
            if (numPlayers == 0)
            {
                utility->display.drawPixel(PIXEL_SIZE * applePositions[appleNum].x + colOffset,
                                           PIXEL_SIZE * applePositions[appleNum].y + rowOffset,
                                           paused ? utility->colors.cyan : utility->colors.red);
            }
            else
            {
                utility->display.drawPixel(PIXEL_SIZE * applePositions[appleNum].x + FRAME_THICKNESS + colOffset,
                                           PIXEL_SIZE * applePositions[appleNum].y + FRAME_THICKNESS + FRAME_Y_OFFSET + rowOffset,
                                           paused ? utility->colors.cyan : utility->colors.red);
            }
        }
    }
}

void SnakeGame::drawSnakes() // use only when redrawing whole snakes are required (at start, when changing colors for pause)
{
    for (int i = 0; i < numPlayers; i++)
    {
        Snake *s = snakes[i];

        for (int i = 0; i < s->segments.size(); i++)
        {
            Point p = s->segments.get(i);

            for (int rowOffset = 0; rowOffset < PIXEL_SIZE; rowOffset++)
            {
                for (int colOffset = 0; colOffset < PIXEL_SIZE; colOffset++)
                {
                    utility->display.drawPixel(PIXEL_SIZE * p.x + FRAME_THICKNESS + colOffset,
                                               PIXEL_SIZE * p.y + FRAME_THICKNESS + FRAME_Y_OFFSET + rowOffset,
                                               paused ? s->colorPaused : s->color);
                }
            }
        }
    }
}

void SnakeGame::clearTail(Point p)
{
    for (int rowOffset = 0; rowOffset < PIXEL_SIZE; rowOffset++)
    {
        for (int colOffset = 0; colOffset < PIXEL_SIZE; colOffset++)
        {
            utility->display.drawPixel(PIXEL_SIZE * p.x + FRAME_THICKNESS + colOffset,
                                       PIXEL_SIZE * p.y + FRAME_THICKNESS + FRAME_Y_OFFSET + rowOffset,
                                       0);
        }
    }
}

void SnakeGame::drawSnakeHeads()
{
    for (int i = 0; i < numPlayers; i++)
    {
        Snake *s = snakes[i];

        Point p = s->segments.get(0);

        for (int rowOffset = 0; rowOffset < PIXEL_SIZE; rowOffset++)
        {
            for (int colOffset = 0; colOffset < PIXEL_SIZE; colOffset++)
            {
                utility->display.drawPixel(PIXEL_SIZE * p.x + FRAME_THICKNESS + colOffset,
                                           PIXEL_SIZE * p.y + FRAME_THICKNESS + FRAME_Y_OFFSET + rowOffset,
                                           paused ? s->colorPaused : s->color);
            }
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
    utility->inputs.update2(utility->inputs.pins.p1Buttons);
    utility->inputs.update2(utility->inputs.pins.p2Buttons);

    if (utility->inputs.A_P1 || utility->inputs.A_P2) // || utility->inputs.B_P1 || utility->inputs.B_P2)
    {
        paused = !paused;
        if (numPlayers == 0)
        {
            autoDrawSnake();
        }
        else
        {
            drawFrame();
            drawSnakes();
        }
        for (int i = 0; i < numApples; i++)
        {
            drawApple(i);
        }
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
        if (s->player == 1)
        {
            s->aimedDirection = UP;
        }
        else
        {
            s->aimedDirection = DOWN;
        }
        s->score = 0;
    }
}

void SnakeGame::resetApple(uint8_t appleNum)
{
    applePositions[appleNum] = getNewApplePosition();
}

void SnakeGame::gameOver()
{
    // utility->display.fillScreen(utility->colors.red);
    // delay(250);
    // utility->display.clearDisplay();
    // delay(250);
    // utility->display.fillScreen(utility->colors.yellow);
    // delay(250);
    // utility->display.clearDisplay();
    // delay(250);
    // utility->display.fillScreen(utility->colors.red);
    // delay(250);
    utility->display.clearDisplay();

    utility->display.setTextColor(utility->colors.cyan);
    utility->display.setFont();
    utility->display.setCursor(1, 1);

    if (numPlayers == 2)
    {
        utility->display.setFont(utility->fonts.my5x5boxy);
        uint8_t scores[2];

        for (int i = 0; i < numPlayers; i++)
        {
            Snake *s = snakes[i];
            scores[i] = s->score;
        }
        if (scores[0] > scores[1])
        {
            utility->display.print("Player 1 wins!");
        }
        else if (scores[1] > scores[0])
        {
            utility->display.print("Player 2 wins!");
        }
        else
        {
            utility->display.print("It's a draw!");
        }
    }

    utility->display.setFont();

    for (int i = 0; i < numPlayers; i++)
    {
        Snake *s = snakes[i];

        utility->display.setCursor(1, (s->player) * 8 + 1);
        utility->display.print("P");
        utility->display.print(s->player);
        utility->display.print(": ");
        utility->display.print(s->score);
    }

    resetSnakes();

    for (int i = 0; i < numApples; i++)
    {
        resetApple(i);
    }

    updateDelay = MAX_DELAY;

    delay(GAME_OVER_DELAY);
}

bool SnakeGame::loopGame()
{
    if (numPlayers == 0)
    {
        return autoLoopGame();
    }
    if (justStarted)
    {
        utility->display.clearDisplay();
        drawFrame();
        drawSnakes();
        for (int i = 0; i < numApples; i++)
        {
            drawApple(i);
        }
        delay(1000);
        justStarted = false;
    }

    checkForPause();

    if (!paused)
    {
        updateSnakeDirections();
        if ((millis() - msPrevious) > updateDelay)
        {
            msPrevious = millis();

            bool appleEatenByAnySnake = false; // would it be better if these were declared outside of the loop? so it isn't
            bool snakeCollision = false;       // allocating new memory every time the loop runs

            for (int i = 0; i < numPlayers; i++)
            {
                Snake *s = snakes[i];

                Point nextPoint = s->getNextPosition();
                if (s->isNextPointValid(nextPoint))
                {
                    s->segments.add(0, nextPoint);
                    s->lastDirectionMoved = s->aimedDirection;
                    bool gotApple = false;

                    for (int i = 0; i < numApples; i++)
                    {
                        if (applePositions[i].isEqual(nextPoint.x, nextPoint.y)) // check if snake got the apple
                        {
                            resetApple(i);
                            drawApple(i);
                            s->score++;
                            gotApple = true;
                            appleEatenByAnySnake = true;
                            break;
                        }
                    }
                    if (!gotApple)
                    {
                        clearTail(s->segments.pop());
                    }
                }
                else // snake has hit something
                {
                    if (numPlayers == 2)
                    {
                        if (s->score - 1 >= 0)
                        {
                            s->score -= 1;
                        }
                        else
                        {
                            s->score = 0;
                        }
                    }
                    snakeCollision = true;
                }
            }

            if (appleEatenByAnySnake)
            {
                increaseSpeed();
            }

            if (snakeCollision)
            {
                delay(3000);

                gameOver();
                return false;
            }
            else
            {
                drawScore();
                drawSnakeHeads();
            }
        }
    }
    return true;
}

void SnakeGame::autoDrawSnake() // snake never gets shorter, so colors don't need recalculated every time it is drawn
{                               // could store colors after they are calculated and then just calculate if a new segment was added
    Snake *s = snakes[0];
    uint8_t colorIncrement = 256 / (PxMATRIX_COLOR_DEPTH + 1); // 30
    uint8_t colorMax = colorIncrement * PxMATRIX_COLOR_DEPTH;  // 240
    uint8_t colorMin = colorIncrement / 2;                     // 0
    uint8_t _r = colorMin;
    uint8_t _g = colorMin;
    uint8_t _b = colorMax;

    uint8_t whichMoving = 1; // indicates if r g or b is currently changing
    bool movingUp = true;    // indicates if whichMoving is increasing or decreasing

    for (int i = 0; i < s->segments.size(); i++)
    {
        Point p = s->segments.get(i);

        if (i != 0) // first one is always white
        {
            if (whichMoving == 1)
            {
                if (movingUp)
                {
                    if (_r < colorMax)
                    {
                        _r += colorIncrement;
                        // Serial.print("_r: ");
                        // Serial.println(_r);
                    }
                    else
                    {
                        whichMoving = 3;
                        movingUp = false;
                    }
                }
                else
                {
                    if (_r > colorMin)
                    {
                        _r -= colorIncrement;
                        // Serial.print("_r: ");
                        // Serial.println(_r);
                    }
                    else
                    {
                        whichMoving = 3;
                        movingUp = true;
                    }
                }
            }
            else if (whichMoving == 2)
            {
                if (movingUp)
                {
                    if (_g < colorMax)
                    {
                        _g += colorIncrement;
                        // Serial.print("_g: ");
                        // Serial.println(_g);
                    }
                    else
                    {
                        whichMoving = 1;
                        movingUp = false;
                    }
                }
                else
                {
                    if (_g > colorMin)
                    {
                        _g -= colorIncrement;
                        // Serial.print("_g: ");
                        // Serial.println(_g);
                    }
                    else
                    {
                        whichMoving = 1;
                        movingUp = true;
                    }
                }
            }
            else
            {
                if (movingUp)
                {
                    if (_b < colorMax)
                    {
                        _b += colorIncrement;
                        // Serial.print("_b: ");
                        // Serial.println(_b);
                    }
                    else
                    {
                        whichMoving = 2;
                        movingUp = false;
                    }
                }
                else
                {
                    if (_b > colorMin)
                    {
                        _b -= colorIncrement;
                        // Serial.print("_b: ");
                        // Serial.println(_b);
                    }
                    else
                    {
                        whichMoving = 2;
                        movingUp = true;
                    }
                }
            }
        }

        for (int rowOffset = 0; rowOffset < PIXEL_SIZE; rowOffset++)
        {
            for (int colOffset = 0; colOffset < PIXEL_SIZE; colOffset++)
            {
                if (i == 0)
                {
                    utility->display.drawPixelRGB888(PIXEL_SIZE * p.x + colOffset,
                                                     PIXEL_SIZE * p.y + rowOffset,
                                                     255, 255, 255);
                }
                else
                {
                    utility->display.drawPixelRGB888(PIXEL_SIZE * p.x + colOffset,
                                                     PIXEL_SIZE * p.y + rowOffset,
                                                     _r, _g, _b);
                }
            }
        }
    }
}

void SnakeGame::autoDrawScore()
{
    Snake *s = snakes[0];
    if (s->score <= MATRIX_WIDTH)
    {
        for (int i = 0; i < s->score; i++)
        {
            utility->display.drawPixel(i, MATRIX_HEIGHT - 1, utility->colors.magenta);
        }
    }
    else if (s->score <= MATRIX_WIDTH + MATRIX_HEIGHT - 1)
    {
        for (int i = 0; i < MATRIX_WIDTH; i++)
        {
            utility->display.drawPixel(i, MATRIX_HEIGHT - 1, utility->colors.magenta);
        }
        for (int i = 0; i < s->score - MATRIX_WIDTH; i++)
        {
            utility->display.drawPixel(MATRIX_WIDTH - 1, MATRIX_HEIGHT - 1 - i, utility->colors.magenta);
        }
    }
}

bool SnakeGame::autoCheckForQuit()
{
    if (utility->inputs.B_P1 || utility->inputs.B_P2)
    {
        Snake *s = snakes[0];

        while (s->segments.size() > 0)
        {
            s->segments.pop();
        }
        s->segments.add(s->getInitialPosition());
        s->score = 0;

        s->aimedDirection = UP;
        s->lastDirectionMoved = UP;
        s->priorToLastDirectionMoved = RIGHT;

        for (int i = 0; i < numApples; i++)
        {
            resetApple(i);
        }
        return false;
    }
    return true;
}

bool SnakeGame::autoLoopGame()
{
    if (justStarted)
    {
        utility->display.clearDisplay();
        autoDrawSnake();
        for (int i = 0; i < numApples; i++)
        {
            drawApple(i);
        }
        delay(1000);
        justStarted = false;
    }

    checkForPause();
    if (!autoCheckForQuit())
    {
        return false;
    }

    /*
    Issues:
    -apples spawn in snake

    */

    if (!paused)
    {
        if (!autoDirectionSet)
        {
            Snake *s = snakes[0];
            Point nextPoint = s->getNextPosition();

            if (s->isNextPointValid(nextPoint))
            {
                distanceToApple = 100;
                autoCollision = false;

                Point snakeHead = snakes[0]->segments.get(0);
                for (int i = 0; i < numApples; i++)
                {
                    if (s->lastDirectionMoved == LEFT || s->lastDirectionMoved == RIGHT)
                    {
                        if (applePositions[i].x == snakeHead.x)
                        {
                            if (applePositions[i].y > snakeHead.y)
                            {
                                if (applePositions[i].y - snakeHead.y < distanceToApple)
                                {
                                    snakes[0]->aimedDirection = DOWN;
                                    distanceToApple = applePositions[i].y - snakeHead.y;
                                }
                            }
                            else
                            {
                                if (snakeHead.y - applePositions[i].y < distanceToApple)
                                {
                                    snakes[0]->aimedDirection = UP;
                                    distanceToApple = snakeHead.y - applePositions[i].y;
                                }
                            }
                        }
                        else if (applePositions[i].y == snakeHead.y)
                        {
                            if (applePositions[i].x > snakeHead.x)
                            {
                                if (applePositions[i].x - snakeHead.x < distanceToApple && snakes[0]->lastDirectionMoved != LEFT)
                                {
                                    snakes[0]->aimedDirection = RIGHT;
                                    distanceToApple = applePositions[i].x - snakeHead.x;
                                }
                            }
                            else
                            {
                                if (snakeHead.x - applePositions[i].x < distanceToApple && snakes[0]->lastDirectionMoved != RIGHT)
                                {
                                    snakes[0]->aimedDirection = LEFT;
                                    distanceToApple = snakeHead.x - applePositions[i].x;
                                }
                            }
                        }
                    }
                    else if (s->lastDirectionMoved == UP || s->lastDirectionMoved == DOWN)
                    {
                        if (applePositions[i].y == snakeHead.y)
                        {
                            if (applePositions[i].x > snakeHead.x)
                            {
                                if (applePositions[i].x - snakeHead.x < distanceToApple)
                                {
                                    snakes[0]->aimedDirection = RIGHT;
                                    distanceToApple = applePositions[i].x - snakeHead.x;
                                }
                            }
                            else
                            {
                                if (snakeHead.x - applePositions[i].x < distanceToApple)
                                {
                                    snakes[0]->aimedDirection = LEFT;
                                    distanceToApple = snakeHead.x - applePositions[i].x;
                                }
                            }
                        }
                        else if (applePositions[i].x == snakeHead.x)
                        {
                            if (applePositions[i].y > snakeHead.y)
                            {
                                if (applePositions[i].y - snakeHead.y < distanceToApple && snakes[0]->lastDirectionMoved != UP)
                                {
                                    snakes[0]->aimedDirection = DOWN;
                                    distanceToApple = applePositions[i].y - snakeHead.y;
                                }
                            }
                            else
                            {
                                if (snakeHead.y - applePositions[i].y < distanceToApple && snakes[0]->lastDirectionMoved != DOWN)
                                {
                                    snakes[0]->aimedDirection = UP;
                                    distanceToApple = snakeHead.y - applePositions[i].y;
                                }
                            }
                        }
                    }
                }
            }

            uint8_t badDirs = 0;
            nextPoint = s->getNextPosition();
            while (!s->isNextPointValid(nextPoint))
            {
                badDirs++;
                if (badDirs == 3)
                {
                    autoCollision = true;
                    break;
                }
                else if (badDirs == 1)
                {
                    s->firstAimedDirectionAttempted = s->aimedDirection;
                    if (s->lastDirectionMoved == s->aimedDirection)
                    {
                        s->aimedDirection = s->priorToLastDirectionMoved;
                    }
                    else
                    {
                        s->aimedDirection = s->lastDirectionMoved;
                    }
                }
                else
                {
                    switch (s->firstAimedDirectionAttempted)
                    {
                    case UP:
                        s->aimedDirection = s->aimedDirection == LEFT ? RIGHT : LEFT;
                        break;
                    case DOWN:
                        s->aimedDirection = s->aimedDirection == LEFT ? RIGHT : LEFT;
                        break;
                    case LEFT:
                        s->aimedDirection = s->aimedDirection == UP ? DOWN : UP;
                        break;
                    case RIGHT:
                        s->aimedDirection = s->aimedDirection == UP ? DOWN : UP;
                        break;
                    }
                }
                nextPoint = s->getNextPosition();
            }

            autoDirectionSet = true;
        }

        if ((millis() - msPrevious) > updateDelay)
        {
            msPrevious = millis();

            bool snakeGotApple = false; // would it be better if this was declared outside of the loop?
            autoDirectionSet = false;

            Snake *s = snakes[0];

            Point nextPoint = s->getNextPosition();
            if (!autoCollision) // can use this to set direction
            {
                s->segments.add(0, nextPoint);
                if (s->aimedDirection != s->lastDirectionMoved)
                {
                    s->priorToLastDirectionMoved = s->lastDirectionMoved;
                }

                s->lastDirectionMoved = s->aimedDirection;

                for (int i = 0; i < numApples; i++)
                {
                    if (applePositions[i].isEqual(nextPoint.x, nextPoint.y)) // check if snake got the apple
                    {
                        s->score++;
                        resetApple(i);
                        snakeGotApple = true;
                        break;
                    }
                }
                if (!snakeGotApple)
                {
                    s->segments.pop();
                }
                utility->display.clearDisplay();
                autoDrawSnake();
                for (int i = 0; i < numApples; i++)
                {
                    drawApple(i);
                }
                // autoDrawScore();
            }
            else // snake has hit something
            {
                utility->display.clearDisplay(); // remove all this after only drawing what's necessary, it causes 2nd snake to move once after 1st snake crashes
                autoDrawSnake();
                for (int i = 0; i < numApples; i++)
                {
                    drawApple(i);
                }
                autoDrawScore();

                Snake *s = snakes[0];

                while (s->segments.size() > 0)
                {
                    s->segments.pop();
                }
                s->segments.add(s->getInitialPosition());
                s->score = 0;

                s->aimedDirection = UP;
                s->lastDirectionMoved = UP;
                s->priorToLastDirectionMoved = RIGHT;

                for (int i = 0; i < numApples; i++)
                {
                    resetApple(i);
                }
                delay(GAME_OVER_DELAY);
                utility->display.clearDisplay();
                // gameOver();
                // return false;
            }
        }
    }
    return true;
}