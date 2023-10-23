#include "Tetris.h"
#include <algorithm>

Tetris::TetrisPiece::TetrisPiece()
{
    Serial.println("!!!!!TetrisPiece default constructor!!!!!");
}

Tetris::TetrisPiece::TetrisPiece(uint16_t color, uint8_t length, const bool *orientations, bool rotatable,
                                 int8_t spawnXOffset, int8_t spawnYOffset) : color(color),
                                                                             length(length),
                                                                             orientations((bool *)orientations),
                                                                             rotatable(rotatable),
                                                                             spawnXOffset(spawnXOffset),
                                                                             spawnYOffset(spawnYOffset)
{
}

int Tetris::TetrisBoard::getRandom(int i)
{
    return random(i);
}

bool Tetris::TetrisBoard::addCurrentPieceToBoard()
{
    bool pieceFits = true;

    uint8_t count = 0;

    for (int row = 0; row < currentPiece.length; row++)
    {
        for (int col = 0; col < currentPiece.length; col++)
        {
            if (currentPiece.orientations[row * currentPiece.length + col])
            {
                if (row + currentPiece.spawnYOffset < 0 || SPAWN_X_OFFSET + col + currentPiece.spawnXOffset < 0 ||
                    SPAWN_X_OFFSET + col + currentPiece.spawnXOffset >= BOARD_WIDTH)
                { // prevents crash if trying to access outside of board array
                    return false;
                }
                if (board[row + currentPiece.spawnYOffset][SPAWN_X_OFFSET + col + currentPiece.spawnXOffset] != 0)
                {
                    pieceFits = false;
                }
                board[row + currentPiece.spawnYOffset][SPAWN_X_OFFSET + col + currentPiece.spawnXOffset] = currentPiece.color;
                currentPieceCoords[count][0] = row + currentPiece.spawnYOffset;
                currentPieceCoords[count][1] = SPAWN_X_OFFSET + col + currentPiece.spawnXOffset;
                count++;
            }
            if (count == NUM_PIECE_SEGMENTS)
            {
                break;
            }
        }
        if (count == NUM_PIECE_SEGMENTS)
        {
            break;
        }
    }

    return pieceFits;
}

void Tetris::TetrisBoard::checkForLineClear() // this mistakenly assumes line clear will be on the bottom always
{
    /*
    Starting from bottom > check if line is full
    If full
        note first full row #
        check up to 3 more lines above for fullness
        note # of full lines
        break if not full
    If empty
        break
    */
    const uint8_t MAX_POSSIBLE_ROWS_CLEARED = 4;

    uint8_t lowestFullRow = 0;
    uint8_t linesCleared = 0;
    bool lineEmpty;
    bool lineFull;

    for (int _row = BOARD_HEIGHT - 1; _row > 0 && linesCleared < 4; _row--)
    {
        lineEmpty = true;
        lineFull = true;
        for (int _col = 0; _col < BOARD_WIDTH; _col++)
        {
            if (board[_row][_col] == 0)
            {
                lineFull = false;
            }
            else
            {
                lineEmpty = false;
            }
        }
        if (lineEmpty)
        {
            break;
        }
        if (lineFull)
        {
            if (_row > lowestFullRow)
            {
                lowestFullRow = _row;
            }
            linesCleared++;
        }
    }

    Serial.print("linesCleared: ");
    Serial.println(linesCleared);

    if (linesCleared > 0)
    {
        for (int _row = 0; _row < linesCleared; _row++)
        {
            for (int _col = 0; _col < BOARD_WIDTH; _col++)
            {
                board[lowestFullRow - _row][_col] = 0;
            }
        }
        std::rotate(&board[0][0], &board[lowestFullRow + 1 - linesCleared][0], &board[lowestFullRow + 1][0]);
        addLineClearScore(linesCleared);
    }
}

void Tetris::TetrisBoard::addLineClearScore(u_int8_t linesCleared)
{
    //     1 point for each block in a cleared line, unless multiple lines are cleared at the same time
    // Multiplier: lines x mult x blocks = score
    //     1 line:     1 x 1   x 10 = 10    1 x (10)      = 10
    //     2 lines:    2 x 1.5 x 10 = 30    2 x (10 + 5)  = 30
    //     3 lines:    3 x 2   x 10 = 60    3 x (10 + 10) = 60
    //     4 lines:    4 x 2.5 x 10 = 100   4 x (10 + 15) = 100
    score += linesCleared * (10 + (linesCleared - 1) * 5);
}

Tetris::TetrisBoard::TetrisBoard(uint8_t player, const TetrisPiece &iPiece, const TetrisPiece &jPiece, const TetrisPiece &lPiece,
                                 const TetrisPiece &oPiece, const TetrisPiece &sPiece, const TetrisPiece &tPiece,
                                 const TetrisPiece &zPiece) : player(player),
                                                              bag{iPiece, jPiece, lPiece, oPiece,
                                                                  sPiece, tPiece, zPiece}
{
    score = 0;
    nextPieceBagPosition = 0;
    currentOrientation = 0;
    shuffleBag();
    currentPiece = bag[nextPieceBagPosition];
    addCurrentPieceToBoard();

    nextPieceBagPosition++;
    nextPiece = bag[nextPieceBagPosition];
}

void Tetris::TetrisBoard::shuffleBag()
{
    std::random_shuffle(std::begin(bag), std::end(bag), Tetris::TetrisBoard::getRandom);
}

bool Tetris::TetrisBoard::tryGetNewPiece()
{
    currentPiece = nextPiece;
    currentOrientation = 0;
    nextPieceBagPosition++;
    if (nextPieceBagPosition == NUM_PIECES)
    {
        shuffleBag();
        nextPieceBagPosition = 0;
    }
    nextPiece = bag[nextPieceBagPosition];

    return addCurrentPieceToBoard();
}

bool Tetris::TetrisBoard::tryRotatePiece(bool clockwise)    // return condition used to know if board needs updated
{
    if (!currentPiece.rotatable)
    {
        return false;
    }
    
    uint8_t tempOrientation = currentOrientation;
    if (clockwise)
    {
        tempOrientation = currentOrientation < NUM_ROTATIONS - 1 ? currentOrientation + 1 : 0;
    }
    else
    {
        tempOrientation = currentOrientation > 0 ? currentOrientation - 1 : NUM_ROTATIONS - 1;
    }

    bool pieceFits = true;

    uint8_t count = 0;

    for (int row = 0; row < currentPiece.length; row++)
    {
        for (int col = 0; col < currentPiece.length; col++)
        {
            if (currentPiece.orientations[tempOrientation * currentPiece.length * currentPiece.length + row * currentPiece.length + col])
            {
                /*
                Need to check if segment was already occupied by this piece. If so, ignore/skip that segment.
                
                Check if it is out of bounds. For each separate out of bounds condition, kick the piece in the necessary direction and check again
                
                If not, check if board space is occupied.
                    If it is, pieceFits = false and break
                */
                if (currentPiece.orientations[currentOrientation * currentPiece.length * currentPiece.length + row * currentPiece.length + col])
                {
                    continue;
                }
                
                // if (row + currentPiece.spawnYOffset < 0 || SPAWN_X_OFFSET + col + currentPiece.spawnXOffset < 0 ||
                //     SPAWN_X_OFFSET + col + currentPiece.spawnXOffset >= BOARD_WIDTH)
                // { // prevents crash if trying to access outside of board array
                //     return false;
                // }
                // if (board[row + currentPiece.spawnYOffset][SPAWN_X_OFFSET + col + currentPiece.spawnXOffset] != 0)
                // {
                //     pieceFits = false;
                // }
                // board[row + currentPiece.spawnYOffset][SPAWN_X_OFFSET + col + currentPiece.spawnXOffset] = currentPiece.color;
                // currentPieceCoords[count][0] = row + currentPiece.spawnYOffset;
                // currentPieceCoords[count][1] = SPAWN_X_OFFSET + col + currentPiece.spawnXOffset;
                // count++;
            }
            if (count == NUM_PIECE_SEGMENTS)
            {
                break;
            }
        }
        if (count == NUM_PIECE_SEGMENTS)
        {
            break;
        }
    }

    return pieceFits;
    return true;
}

bool Tetris::TetrisBoard::tryMovePiece(bool left)
{
    /*
    Find leftmost points
        For each unique row #, find the lowest column #
    Check if board is occupied 1 space left each of those points or left of board
    */
    bool canMove = true;

    uint8_t _col;
    uint8_t _row;

    for (int i = 0; i < NUM_PIECE_SEGMENTS; i++) // search from beginning of list if moving left, end of list if moving right
    {
        _row = currentPieceCoords[left ? NUM_PIECE_SEGMENTS - 1 - i : i][0];
        _col = currentPieceCoords[left ? NUM_PIECE_SEGMENTS - 1 - i : i][1];

        //      T-piece spawn position
        //                      [0][0],[0][1]=0,4
        // [1][0],[1][1]=1,3    [2][0],[2][1]=1,4    [3][0],[3][1]=1,5

        bool foundOtherSegmentInRow = false;
        for (int j = i + 1; j < NUM_PIECE_SEGMENTS; j++) // search the rest for a lefter segment in same row
        {
            if (currentPieceCoords[left ? NUM_PIECE_SEGMENTS - 1 - j : j][0] == _row) // same row
            {
                foundOtherSegmentInRow = true; // coords are ordered so a match has to be a righter/lefter column
                break;                         // just break cuz we'll get to it later
            }
        }
        if (!foundOtherSegmentInRow)
        {
            if (left)
            {
                if (_col - 1 == -1)
                {
                    canMove = false;
                    break;
                }
            }
            else
            {
                if (_col + 1 == BOARD_WIDTH)
                {
                    canMove = false;
                    break;
                }
            }
            if (board[_row][left ? _col - 1 : _col + 1] != 0)
            {
                canMove = false;
                break;
            }
        }
    }

    if (canMove)
    {
        for (int i = 0; i < NUM_PIECE_SEGMENTS; i++)
        {
            board[currentPieceCoords[i][0]][currentPieceCoords[i][1]] = 0;
        }
        for (int i = 0; i < NUM_PIECE_SEGMENTS; i++)
        {
            if (left)
            {
                currentPieceCoords[i][1] -= 1;
                board[currentPieceCoords[i][0]][currentPieceCoords[i][1]] = currentPiece.color;
            }
            else
            {
                currentPieceCoords[i][1] += 1;
                board[currentPieceCoords[i][0]][currentPieceCoords[i][1]] = currentPiece.color;
            }
        }
    }

    return canMove;
}

bool Tetris::TetrisBoard::tryLowerPiece()
{
    /*
    Find lowest points
        For each unique column #, find the highest row #
    Check if board is occupied 1 space below each of those points or bottom of board
    */
    bool canLower = true;

    uint8_t _col;
    uint8_t _row;

    for (int i = 0; i < NUM_PIECE_SEGMENTS; i++)
    {
        _row = currentPieceCoords[i][0];
        _col = currentPieceCoords[i][1];

        bool foundLowerSegment = false;
        for (int j = i + 1; j < NUM_PIECE_SEGMENTS; j++) // search the rest for a lower segment in same column
        {
            if (currentPieceCoords[j][1] == _col) // same column
            {
                foundLowerSegment = true; // coords are ordered by row so a match has to be a lower row
                break;                    // just break cuz we'll get to it later
            }
        }
        if (!foundLowerSegment)
        {
            if (_row + 1 == BOARD_HEIGHT)
            {
                canLower = false;
                break;
            }
            else if (board[_row + 1][_col] != 0)
            {
                canLower = false;
                break;
            }
        }
    }

    if (canLower)
    {
        for (int i = 0; i < NUM_PIECE_SEGMENTS; i++)
        {
            board[currentPieceCoords[i][0]][currentPieceCoords[i][1]] = 0;
        }
        for (int i = 0; i < NUM_PIECE_SEGMENTS; i++)
        {
            currentPieceCoords[i][0] += 1;
            board[currentPieceCoords[i][0]][currentPieceCoords[i][1]] = currentPiece.color;
        }
    }
    else
    {
        score += 1;
        checkForLineClear();
    }

    return canLower;
}

void Tetris::TetrisBoard::resetBoard()
{
    score = 0;
    for (int _row = 0; _row < BOARD_HEIGHT; _row++)
    {
        for (int _col = 0; _col < BOARD_WIDTH; _col++)
        {
            board[_row][_col] = 0;
        }
    }

    nextPieceBagPosition = 0;
    currentOrientation = 0;
    shuffleBag();
    currentPiece = bag[nextPieceBagPosition];
    addCurrentPieceToBoard();

    nextPieceBagPosition++;
    nextPiece = bag[nextPieceBagPosition];
}

Tetris::Tetris(Utility &utility, uint8_t numPlayers) : BaseGame{utility, numPlayers},
                                                       iPiece(utility.colors.blueLight, sizeof(iPieceRaw[0]) / sizeof(iPieceRaw[0][0]), &(iPieceRaw[0][0][0]), true, 0, -1),
                                                       jPiece(utility.colors.blueDark, sizeof(jPieceRaw[0]) / sizeof(jPieceRaw[0][0]), &(jPieceRaw[0][0][0]), true, 0, 0),
                                                       lPiece(utility.colors.orange, sizeof(lPieceRaw[0]) / sizeof(lPieceRaw[0][0]), &(lPieceRaw[0][0][0]), true, 0, 0),
                                                       oPiece(utility.colors.yellow, sizeof(oPieceRaw) / sizeof(oPieceRaw[0]), &(oPieceRaw[0][0]), false, 1, 0),
                                                       sPiece(utility.colors.green, sizeof(sPieceRaw[0]) / sizeof(sPieceRaw[0][0]), &(sPieceRaw[0][0][0]), true, 0, 0),
                                                       tPiece(utility.colors.pink, sizeof(tPieceRaw[0]) / sizeof(tPieceRaw[0][0]), &(tPieceRaw[0][0][0]), true, 0, 0),
                                                       zPiece(utility.colors.red, sizeof(zPieceRaw[0]) / sizeof(zPieceRaw[0][0]), &(zPieceRaw[0][0][0]), true, 0, 0)
{
    MIN_DELAY = 10;
    MAX_DELAY = 100; // max value for uint8_t is 255
    SPEED_LOSS = 5;
    GAME_OVER_DELAY = 10000;

    updateDelay = 255; // MAX_DELAY

    msCurrent = 0;
    msPrevious = 0;

    paused = false;
    justStarted = true;

    for (int i = 0; i < numPlayers; i++)
    {
        boards[i] = new TetrisBoard(1, iPiece, jPiece, lPiece, oPiece, sPiece, tPiece, zPiece);
    }
}

void Tetris::setPlayers(uint8_t numPlayers)
{
    this->numPlayers = numPlayers;
}

void Tetris::drawFrame()
{
    uint8_t width = 22;
    uint8_t height = 42;
    for (int i = 0; i < numPlayers; i++)
    {
        display.setRotation(i * 2);
        display.drawRect(9, 22, width, height, paused ? utility->colors.yellow : utility->colors.red);
        display.setCursor(2, 16);
        display.print("Next:");
    }
}

void Tetris::drawScore()
{
    display.fillRect(2, 2, 19, 5, 0);
    display.setCursor(2, 6);
    display.print(boards[0]->score);
}

void Tetris::clearNextPiece()
{
    display.fillRect(NEXTPIECE_X_POSITION, NEXTPIECE_Y_POSITION, 8, 4, 0);
}

void Tetris::clearBoard()
{
    display.fillRect(BOARD_X_POSITION, BOARD_Y_POSITION, BOARD_WIDTH * 2, BOARD_HEIGHT * 2, 0);
}

void Tetris::drawNextPiece()
{
    clearNextPiece();
    TetrisPiece piece = boards[0]->nextPiece;
    uint8_t segmentsDrawn = 0;

    for (int row = 0; row < piece.length; row++)
    {
        for (int col = 0; col < piece.length; col++)
        {
            if (piece.orientations[row * piece.length + col])
            {
                for (int rowOffset = 0; rowOffset < BOARD_PIXEL_SIZE; rowOffset++)
                {
                    for (int colOffset = 0; colOffset < BOARD_PIXEL_SIZE; colOffset++)
                    {
                        display.drawPixel(NEXTPIECE_X_POSITION + (BOARD_PIXEL_SIZE * col) + rowOffset,
                                          NEXTPIECE_Y_POSITION + (BOARD_PIXEL_SIZE * row) + colOffset, piece.color);
                    }
                }
                segmentsDrawn++;
            }
            if (segmentsDrawn == NUM_PIECE_SEGMENTS)
            {
                break;
            }
        }
        if (segmentsDrawn == NUM_PIECE_SEGMENTS)
        {
            break;
        }
    }
}

void Tetris::drawBoard()
{
    for (int row = 0; row < BOARD_HEIGHT; row++)
    {
        for (int col = 0; col < BOARD_WIDTH; col++)
        {
            if (tempBoards[0][row][col] != boards[0]->board[row][col])
            {
                tempBoards[0][row][col] = boards[0]->board[row][col];

                for (int rowOffset = 0; rowOffset < BOARD_PIXEL_SIZE; rowOffset++)
                {
                    for (int colOffset = 0; colOffset < BOARD_PIXEL_SIZE; colOffset++)
                    {
                        display.drawPixel(BOARD_X_POSITION + (BOARD_PIXEL_SIZE * col) + colOffset,
                                          BOARD_Y_POSITION + (BOARD_PIXEL_SIZE * row) + rowOffset, boards[0]->board[row][col]);
                    }
                }
            }
        }
    }
}

void Tetris::checkForInput()
{
    utility->inputs.update2(utility->inputs.pins.p1Buttons);
    utility->inputs.update2(utility->inputs.pins.p1Directions);
    if (utility->inputs.LEFT_P1)
    {
        if (boards[0]->tryMovePiece(true))
        {
            // clearBoard();
            drawBoard();
        }
    }
    else if (utility->inputs.RIGHT_P1)
    {
        if (boards[0]->tryMovePiece(false))
        {
            // clearBoard();
            drawBoard();
        }
    }
    if (utility->inputs.A_P1)
    {
        if (boards[0]->tryRotatePiece(true))
        {
            // clearBoard();
            drawBoard();
        }
    }
    else if (utility->inputs.A_P2)
    {
        if (boards[0]->tryRotatePiece(false))
        {
            // clearBoard();
            drawBoard();
        }
    }
}

void Tetris::checkForPause()
{
    utility->inputs.update2(utility->inputs.pins.startButton);

    if (utility->inputs.START)
    {
        paused = !paused;
        display.clearDisplay();
        drawFrame();
        drawNextPiece();
        msPrevious = millis();
    }
}

void Tetris::gameOver()
{
    // display.fillScreen(utility->colors.greenCyan);
    // delay(250);
    // display.clearDisplay();
    // delay(250);
    // display.fillScreen(utility->colors.blueLight);
    // delay(250);
    // display.clearDisplay();
    boards[0]->resetBoard();
    delay(GAME_OVER_DELAY);
}

bool Tetris::loopGame()
{
    if (justStarted)
    {
        display.clearDisplay();
        display.setFont(utility->fonts.my5x5round);
        drawFrame();
        drawNextPiece();
        drawBoard();
        drawScore();
        delay(1000);
        justStarted = false;
    }

    checkForPause();

    if (!paused)
    {
        /*
            get inputs
                if inputs are movement or rotation
                    check if valid >
                        update board, draw new board
                if input is hard drop
                    do that > jump to checking for line clear
                if input is soft drop
                    speed up updateDelay (could halve it, probably need an extra variable to track the faster speed)

            check if current updateDelay has passed
                check if current piece can move down 1
                    if yes
                        update board, draw new board
                    if no
                        check for line clear
                        move nextPiece to currentPiece, get new nextPiece
                        draw nextPiece
                        update board, draw new board
        */
        checkForInput();
        msCurrent = millis();
        if ((msCurrent - msPrevious) > updateDelay)
        {
            if (!boards[0]->tryLowerPiece())
            {
                drawScore();
                if (!boards[0]->tryGetNewPiece())
                {
                    // clearBoard();
                    drawNextPiece();
                    drawBoard();
                    delay(updateDelay);
                    gameOver();
                    return false;
                }
                else
                {
                    drawNextPiece();
                }
            }
            // clearBoard();
            // drawNextPiece();
            drawBoard();

            msPrevious = msCurrent;
        }
    }

    // if (!paused)
    // {
    //     utility->inputs.update2(utility->inputs.pins.p2Buttons);
    //     if (utility->inputs.A_P2)
    //     {
    //         clearBoard();
    //         boards[0]->tryGetNewPiece();
    //         drawNextPiece();
    //         drawBoard();
    //     }
    //     if (utility->inputs.B_P2)
    //     {
    //         clearBoard();
    //         boards[0]->tryRotatePiece(true);
    //         drawNextPiece();
    //         drawBoard();
    //         // ESP.restart();
    //         // return false;
    //     }
    // }
    return true;
}
