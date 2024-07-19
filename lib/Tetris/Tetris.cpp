#include "Tetris.h"
#include <algorithm>

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

    currentPieceTopLeft[0] = currentPiece.spawnYOffset;
    currentPieceTopLeft[1] = SPAWN_X_OFFSET + currentPiece.spawnXOffset;

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
                currentPieceCoordinates[count][0] = row + currentPiece.spawnYOffset;
                currentPieceCoordinates[count][1] = SPAWN_X_OFFSET + col + currentPiece.spawnXOffset;
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

void Tetris::TetrisBoard::checkForLineClear()
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

    for (int _row = BOARD_HEIGHT - 1; _row > 0 && linesCleared < MAX_POSSIBLE_ROWS_CLEARED; _row--)
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

void Tetris::TetrisBoard::addLineClearScore(uint8_t linesCleared)
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
                                                                  sPiece, tPiece, zPiece},
                                                              currentPiece(bag[0]), // seems unnecessary
                                                              nextPiece(bag[1])     // seems unnecessary
{
    score = 0;
    nextPieceBagPosition = 0;
    currentOrientation = 0;
    gameOver = false;
    softDrop = false;
    updateFlipper = true;
    hardDrop = false;
    shuffleBag();
    currentPiece = bag[nextPieceBagPosition];
    addCurrentPieceToBoard();

    nextPieceBagPosition++;
    nextPiece = bag[nextPieceBagPosition];
}

void Tetris::TetrisBoard::shuffleBag()
{
    std::random_shuffle(std::begin(bag), std::end(bag), Tetris::TetrisBoard::getRandom); // can I just put contents of getRandom here?
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

bool Tetris::TetrisBoard::tryRotatePiece(bool clockwise) // return condition used to know if board needs updated
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

    // int8_t shiftHorizontal = 0;
    // int8_t shiftVertical = 0;

    for (int row = 0; row < currentPiece.length; row++)
    {
        for (int col = 0; col < currentPiece.length; col++)
        {
            if (currentPiece.orientations[tempOrientation * currentPiece.length * currentPiece.length + row * currentPiece.length + col])
            {
                /*
                Need to check if segment was already occupied by this piece. If so, ignore/skip that segment.

                Check if it is out of bounds. If so, pieceFits = false and break;
                    //For each separate out of bounds condition, kick the piece in the necessary direction and check again

                If not, check if board space is occupied.
                    If it is, pieceFits = false and break;
                */
                if (currentPiece.orientations[currentOrientation * currentPiece.length * currentPiece.length + row * currentPiece.length + col])
                {            // last orientation had a segment here so we know it is safe
                    count++; // or make other ifs else ifs
                    continue;
                }
                if (currentPieceTopLeft[0] + row >= BOARD_HEIGHT || currentPieceTopLeft[0] + row < 0 ||
                    currentPieceTopLeft[1] + col < 0 || currentPieceTopLeft[1] + col >= BOARD_WIDTH)
                { // segment off board, don't rotate
                    pieceFits = false;
                    break;
                }
                if (board[currentPieceTopLeft[0] + row][currentPieceTopLeft[1] + col] != 0)
                {
                    pieceFits = false;
                    break;
                }

                count++;
            }
            if (count == NUM_PIECE_SEGMENTS)
            {
                break;
            }
        }
        if (count == NUM_PIECE_SEGMENTS || !pieceFits)
        {
            break;
        }
    }

    if (pieceFits)
    {
        for (int i = 0; i < NUM_PIECE_SEGMENTS; i++)
        {
            board[currentPieceCoordinates[i][0]][currentPieceCoordinates[i][1]] = 0;
        }

        currentOrientation = tempOrientation;
        count = 0;
        for (int row = 0; row < currentPiece.length; row++)
        {
            for (int col = 0; col < currentPiece.length; col++)
            {
                if (currentPiece.orientations[currentOrientation * currentPiece.length * currentPiece.length + row * currentPiece.length + col])
                {
                    board[currentPieceTopLeft[0] + row][currentPieceTopLeft[1] + col] = currentPiece.color;
                    currentPieceCoordinates[count][0] = currentPieceTopLeft[0] + row;
                    currentPieceCoordinates[count][1] = currentPieceTopLeft[1] + col;

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
    }

    return pieceFits;
}

bool Tetris::TetrisBoard::tryMovePiece(direction dir)
{
    /*
    Find piece segments bordering the desired movement direction
    Check the board spaces 1 point beyond those segments
        if any of those points are occupied
        if any of those points are off of the board
    Update board and currentPieceCoordinates
    */
    bool canMove = true;

    uint8_t _col;
    uint8_t _row;

    for (int i = 0; i < NUM_PIECE_SEGMENTS; i++) // search from beginning of list if moving left, end of list if moving right
    {
        _row = currentPieceCoordinates[dir == LEFT ? NUM_PIECE_SEGMENTS - 1 - i : i][0];
        _col = currentPieceCoordinates[dir == LEFT ? NUM_PIECE_SEGMENTS - 1 - i : i][1];

        //      T-piece spawn position
        //                      [0][0],[0][1]=0,4
        // [1][0],[1][1]=1,3    [2][0],[2][1]=1,4    [3][0],[3][1]=1,5

        bool foundBetterSegment = false;
        for (int j = i + 1; j < NUM_PIECE_SEGMENTS; j++) // search the rest for a lefter segment in same row
        {
            if (dir == DOWN)
            {
                if (currentPieceCoordinates[j][1] == _col) // same column
                {
                    foundBetterSegment = true; // coords are ordered by row so a match has to be a lower row
                    break;                     // just break cuz we'll get to it later
                }
            }
            else if (currentPieceCoordinates[dir == LEFT ? NUM_PIECE_SEGMENTS - 1 - j : j][0] == _row) // same row
            {
                foundBetterSegment = true; // coords are ordered so a match has to be a righter/lefter column
                break;                     // just break cuz we'll get to it later
            }
        }
        if (!foundBetterSegment)
        {
            if (dir == DOWN)
            {
                if (_row + 1 == BOARD_HEIGHT || board[_row + 1][_col] != 0)
                {
                    canMove = false;
                    break;
                }
            }
            else if (dir == LEFT)
            {
                if (_col - 1 == -1 || board[_row][_col - 1] != 0)
                {
                    canMove = false;
                    break;
                }
            }
            else if (_col + 1 == BOARD_WIDTH || board[_row][_col + 1] != 0)
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
            board[currentPieceCoordinates[i][0]][currentPieceCoordinates[i][1]] = 0;
        }
        if (dir == DOWN)
        {
            currentPieceTopLeft[0] += 1;
        }
        else
        {
            currentPieceTopLeft[1] += dir == LEFT ? -1 : 1;
        }

        for (int i = 0; i < NUM_PIECE_SEGMENTS; i++)
        {
            if (dir == DOWN)
            {
                currentPieceCoordinates[i][0] += 1;
            }
            else
            {
                currentPieceCoordinates[i][1] += dir == LEFT ? -1 : 1;
            }
            board[currentPieceCoordinates[i][0]][currentPieceCoordinates[i][1]] = currentPiece.color;
        }
    }
    else if (dir == DOWN)
    {
        score += 1;
        checkForLineClear();
    }

    return canMove;
}

void Tetris::TetrisBoard::resetBoard()
{
    score = 0;
    gameOver = false;
    updateFlipper = true;
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

Tetris::Tetris(Utility &utility, uint8_t numPlayers) : BaseGame{utility},
                                                       iPiece(utility.colors.blueLight, sizeof(iPieceRaw[0]) / sizeof(iPieceRaw[0][0]), &(iPieceRaw[0][0][0]), true, 0, -1),
                                                       jPiece(utility.colors.blueDark, sizeof(jPieceRaw[0]) / sizeof(jPieceRaw[0][0]), &(jPieceRaw[0][0][0]), true, 0, 0),
                                                       lPiece(utility.colors.orange, sizeof(lPieceRaw[0]) / sizeof(lPieceRaw[0][0]), &(lPieceRaw[0][0][0]), true, 0, 0),
                                                       oPiece(utility.colors.yellow, sizeof(oPieceRaw) / sizeof(oPieceRaw[0]), &(oPieceRaw[0][0]), false, 1, 0),
                                                       sPiece(utility.colors.green, sizeof(sPieceRaw[0]) / sizeof(sPieceRaw[0][0]), &(sPieceRaw[0][0][0]), true, 0, 0),
                                                       tPiece(utility.colors.pink, sizeof(tPieceRaw[0]) / sizeof(tPieceRaw[0][0]), &(tPieceRaw[0][0][0]), true, 0, 0),
                                                       zPiece(utility.colors.red, sizeof(zPieceRaw[0]) / sizeof(zPieceRaw[0][0]), &(zPieceRaw[0][0][0]), true, 0, 0)
{
    // autoplayMode = true;
    // onePlayerMode = true;
    // twoPlayerMode = true;
    // gameDisplayName = "Tetris";

    MIN_DELAY = 10;
    MAX_DELAY = 100; // max value for uint8_t is 255
    SPEED_LOSS = 5;
    MOVE_DELAY = 100;
    GAME_OVER_DELAY = 10000;

    updateDelay = 180; // piece moves down at 2 * updateDelay unless soft drop button is held

    msPrevious = 0;

    paused = false;
    justStarted = true;

    setPlayers(numPlayers);
}

void Tetris::setPlayers(uint8_t numPlayers)
{
    this->numPlayers = numPlayers;
    for (int i = 0; i < numPlayers; i++)
    {
        if (boards[i] == nullptr)
        {
            boards[i] = new TetrisBoard(i + 1, iPiece, jPiece, lPiece, oPiece, sPiece, tPiece, zPiece);
        }
    }
    if (numPlayers == 1)
    {
        BOARD_PIXEL_SIZE = BOARD_PIXEL_SIZE_1P;
        BOARD_X_POSITION = BOARD_X_POSITION_1P;
        BOARD_Y_POSITION = BOARD_Y_POSITION_1P;
        NEXTPIECE_X_POSITION = NEXTPIECE_X_POSITION_1P;
        NEXTPIECE_Y_POSITION = NEXTPIECE_Y_POSITION_1P;
    }
    else if (numPlayers == 2)
    {
        BOARD_PIXEL_SIZE = BOARD_PIXEL_SIZE_2P;
        BOARD_X_POSITION = BOARD_X_POSITION_2P;
        BOARD_Y_POSITION = BOARD_Y_POSITION_2P;
        NEXTPIECE_X_POSITION = NEXTPIECE_X_POSITION_2P;
        NEXTPIECE_Y_POSITION = NEXTPIECE_Y_POSITION_2P;
    }
    else if (numPlayers == 0)
    {
        if (autoBoard == nullptr)
        {
            Serial.println("creating autoboard");
            autoBoard = new AutoTetrisBoard(iPiece, jPiece, lPiece, oPiece, sPiece, tPiece, zPiece);
        }
        updateDelay = 20;
    }
}

void Tetris::drawFrame()
{
    uint8_t width = BOARD_WIDTH * BOARD_PIXEL_SIZE + 2 * FRAME_THICKNESS;
    uint8_t height = BOARD_HEIGHT * BOARD_PIXEL_SIZE + 2 * FRAME_THICKNESS;
    for (int i = 0; i < numPlayers; i++)
    {
        utility->display.setRotation(i * 2);
        utility->display.setCursor(2, 16);
        utility->display.print("Next:");
        for (int j = 0; j < FRAME_THICKNESS; j++)
        {
            if (i == 1)
            {
                utility->display.drawRect(BOARD_X_POSITION - FRAME_THICKNESS + j, BOARD_Y_POSITION - FRAME_THICKNESS + j,
                                          width - 2 * j, height - 2 * j, paused ? utility->colors.pink : utility->colors.green);
            }
            utility->display.drawRect(BOARD_X_POSITION - FRAME_THICKNESS + j, BOARD_Y_POSITION - FRAME_THICKNESS + j,
                                      width - 2 * j, height - 2 * j, paused ? utility->colors.yellow : utility->colors.red);
        }
    }
}

void Tetris::drawScore(uint8_t player)
{
    utility->display.setRotation((player - 1) * 2);
    utility->display.fillRect(2, 2, 19, 5, 0);
    utility->display.setCursor(2, 6);
    utility->display.print(boards[player - 1]->score);
}

void Tetris::clearNextPiece()
{
    utility->display.fillRect(NEXTPIECE_X_POSITION, NEXTPIECE_Y_POSITION, 4 * BOARD_PIXEL_SIZE, 2 * BOARD_PIXEL_SIZE, 0);
}

// void Tetris::clearBoard()    // could be used in an animation/multiplayer attack where board disappears for a second
// {
//     utility->display.fillRect(BOARD_X_POSITION, BOARD_Y_POSITION, BOARD_WIDTH * BOARD_PIXEL_SIZE, BOARD_HEIGHT * BOARD_PIXEL_SIZE, 0);
// }

void Tetris::drawNextPiece(uint8_t player)
{
    utility->display.setRotation((player - 1) * 2);
    clearNextPiece();
    TetrisPiece piece = boards[player - 1]->nextPiece;
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
                        utility->display.drawPixel(NEXTPIECE_X_POSITION + (BOARD_PIXEL_SIZE * col) + colOffset,
                                                   NEXTPIECE_Y_POSITION + (BOARD_PIXEL_SIZE * row) + rowOffset, piece.color);
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

void Tetris::drawBoard(uint8_t player)
{
    utility->display.setRotation((player - 1) * 2);
    for (int row = 0; row < BOARD_HEIGHT; row++)
    {
        for (int col = 0; col < BOARD_WIDTH; col++)
        {
            if (tempBoards[player - 1][row][col] != boards[player - 1]->board[row][col])
            {
                tempBoards[player - 1][row][col] = boards[player - 1]->board[row][col];

                for (int rowOffset = 0; rowOffset < BOARD_PIXEL_SIZE; rowOffset++)
                {
                    for (int colOffset = 0; colOffset < BOARD_PIXEL_SIZE; colOffset++)
                    {
                        utility->display.drawPixel(BOARD_X_POSITION + (BOARD_PIXEL_SIZE * col) + colOffset,
                                                   BOARD_Y_POSITION + (BOARD_PIXEL_SIZE * row) + rowOffset, boards[player - 1]->board[row][col]);
                    }
                }
            }
        }
    }
}

void Tetris::checkForInput()
{
    for (int i = 0; i < numPlayers; i++)
    {
        utility->display.setRotation(i * 2);

        bool _up;
        bool _down;
        bool _left;
        bool _right;
        bool _a;
        bool _b;

        if (boards[i]->player == 1)
        {
            utility->inputs.update2(utility->inputs.pins.p1Buttons);
            utility->inputs.update2(utility->inputs.pins.p1Directions);
            _up = utility->inputs.UP_P1;
            _down = utility->inputs.DOWN_P1_pressed;
            _left = utility->inputs.LEFT_P1_pressed;
            _right = utility->inputs.RIGHT_P1_pressed;
            _a = utility->inputs.A_P1;
            _b = utility->inputs.B_P1; // TODO: This is not being detected!

            if (_a == true)
            {
                Serial.println("A WAS PRESSED");
            }
            if (_b == true)
            {
                Serial.println("B WAS PRESSED");
            }
        }
        else if (boards[i]->player == 2) // controls are flipped for P2
        {
            utility->inputs.update2(utility->inputs.pins.p2Buttons);
            utility->inputs.update2(utility->inputs.pins.p2Directions);
            _up = utility->inputs.DOWN_P2;
            _down = utility->inputs.UP_P2_pressed;
            _left = utility->inputs.RIGHT_P2_pressed;
            _right = utility->inputs.LEFT_P2_pressed;
            _a = utility->inputs.A_P2;
            _b = utility->inputs.B_P2;
        }

        if (millis() - boards[i]->lastMove > MOVE_DELAY)
        {
            if (_left)
            {
                if (boards[i]->tryMovePiece(LEFT))
                {
                    boards[i]->lastMove = millis();
                    drawBoard(boards[i]->player);
                }
            }
            else if (_right)
            {
                if (boards[i]->tryMovePiece(RIGHT))
                {
                    boards[i]->lastMove = millis();
                    drawBoard(boards[i]->player);
                }
            }
        }

        if (_up)
        {
            boards[i]->hardDrop = true;
        }
        else if (_down)
        {
            boards[i]->softDrop = true;
        }
        else
        {
            boards[i]->softDrop = false;
        }

        if (_a)
        {
            Serial.println("rotate piece clockwise");
            if (boards[i]->tryRotatePiece(true))
            {
                drawBoard(boards[i]->player);
            }
        }
        else if (_b)
        {
            Serial.println("rotate piece COUNTER-clockwise");
            if (boards[i]->tryRotatePiece(false))
            {
                drawBoard(boards[i]->player);
            }
        }
    }
}

void Tetris::checkForPause()
{
    utility->inputs.update2(utility->inputs.pins.startButton);
    // utility->inputs.update2(utility->inputs.pins.p1Buttons);

    // if (utility->inputs.A_P1)
    if (utility->inputs.START)
    {
        paused = !paused;
        drawFrame();
        msPrevious = millis();
    }
}

void Tetris::gameOver()
{
    // utility->display.fillScreen(utility->colors.greenCyan);
    // delay(250);
    // utility->display.clearDisplay();
    // delay(250);
    // utility->display.fillScreen(utility->colors.blueLight);
    // delay(250);
    // utility->display.clearDisplay();
    for (int i = 0; i < numPlayers; i++)
    {
        boards[i]->resetBoard();
        for (int _row = 0; _row < BOARD_HEIGHT; _row++)
        {
            for (int _col = 0; _col < BOARD_WIDTH; _col++)
            {
                tempBoards[i][_row][_col] = 0;
            }
        }
    }
    delay(GAME_OVER_DELAY);
}

bool Tetris::loopGame()
{
    if (numPlayers == 0)
    {
        return autoLoopGame();
    }
    else
    {
        if (justStarted)
        {
            utility->display.clearDisplay();
            utility->display.setFont(utility->fonts.my5x5round);
            drawFrame();
            for (int i = 0; i < numPlayers; i++)
            {
                drawNextPiece(i + 1);
                drawBoard(i + 1);
                drawScore(i + 1);
            }
            delay(1000);
            justStarted = false;
        }

        // checkForPause();

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

            if ((millis() - msPrevious) > updateDelay)
            {
                msPrevious = millis();
                for (int i = 0; i < numPlayers; i++)
                {
                    if (!boards[i]->gameOver)
                    {
                        bool pieceAtBottom = false;
                        if (boards[i]->hardDrop)
                        {
                            while (!pieceAtBottom)
                            {
                                pieceAtBottom = !boards[i]->tryMovePiece(DOWN);
                            }
                            boards[i]->hardDrop = false;
                        }
                        else if (boards[i]->softDrop)
                        {
                            pieceAtBottom = !boards[i]->tryMovePiece(DOWN);
                            boards[i]->updateFlipper = true;
                        }
                        else if (boards[i]->updateFlipper)
                        {
                            pieceAtBottom = !boards[i]->tryMovePiece(DOWN);
                        }
                        boards[i]->updateFlipper = !boards[i]->updateFlipper;
                        if (pieceAtBottom)
                        {
                            drawScore(boards[i]->player);
                            if (!boards[i]->tryGetNewPiece())
                            {
                                drawNextPiece(boards[i]->player);

                                boards[i]->gameOver = true;
                            }
                            else
                            {
                                drawNextPiece(boards[i]->player);
                            }
                        }
                        drawBoard(boards[i]->player);
                    }
                }
                bool allGameOver = true;
                for (int i = 0; i < numPlayers; i++)
                {
                    if (boards[i]->gameOver == false)
                    {
                        allGameOver = false;
                    }
                }
                if (allGameOver)
                {
                    gameOver();
                    return false;
                }
            }
        }
        return true;
    }
}

bool Tetris::AutoTetrisBoard::addCurrentPieceToBoard()
{
    bool pieceFits = true;

    uint8_t count = 0;

    currentPieceTopLeft[0] = 0;
    currentPieceTopLeft[1] = random(65 - currentPiece.length);
    if (currentPiece.rotatable)
    {
        currentOrientation = random(4);
    }
    else
    {
        currentOrientation = 0;
    }

    for (int row = 0; row < currentPiece.length; row++)
    {
        for (int col = 0; col < currentPiece.length; col++)
        {
            if (currentPiece.orientations[currentOrientation * currentPiece.length * currentPiece.length + row * currentPiece.length + col])
            {
                if (row + currentPieceTopLeft[0] < 0 || currentPieceTopLeft[1] + col < 0 ||
                    currentPieceTopLeft[1] + col >= 64)
                { // prevents crash if trying to access outside of board array
                    return false;
                }
                if (aBoard[row + currentPieceTopLeft[0]][currentPieceTopLeft[1] + col] != 0)
                {
                    pieceFits = false;
                }
                aBoard[row + currentPieceTopLeft[0]][currentPieceTopLeft[1] + col] = currentPiece.color;
                currentPieceCoordinates[count][0] = row + currentPieceTopLeft[0];
                currentPieceCoordinates[count][1] = currentPieceTopLeft[1] + col;
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

void Tetris::AutoTetrisBoard::checkForLineClear()
{
    const uint8_t MAX_POSSIBLE_ROWS_CLEARED = 4;

    uint8_t lowestFullRow = 0;
    uint8_t linesCleared = 0;
    bool lineEmpty;
    bool lineFull;

    for (int _row = 64 - 1; _row > 0 && linesCleared < MAX_POSSIBLE_ROWS_CLEARED; _row--)
    {
        lineEmpty = true;
        lineFull = true;
        for (int _col = 0; _col < 64; _col++)
        {
            if (aBoard[_row][_col] == 0)
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

    if (linesCleared > 0)
    {
        for (int _row = 0; _row < linesCleared; _row++)
        {
            for (int _col = 0; _col < 64; _col++)
            {
                aBoard[lowestFullRow - _row][_col] = 0;
            }
        }
        std::rotate(&aBoard[0][0], &aBoard[lowestFullRow + 1 - linesCleared][0], &aBoard[lowestFullRow + 1][0]);
    }
}

int Tetris::AutoTetrisBoard::getRandom(int i)
{
    return random(i);
}

void Tetris::AutoTetrisBoard::shuffleBag()
{
    std::random_shuffle(std::begin(bag), std::end(bag), Tetris::AutoTetrisBoard::getRandom);
}

Tetris::AutoTetrisBoard::AutoTetrisBoard(const TetrisPiece &iPiece, const TetrisPiece &jPiece, const TetrisPiece &lPiece,
                                         const TetrisPiece &oPiece, const TetrisPiece &sPiece, const TetrisPiece &tPiece,
                                         const TetrisPiece &zPiece) : bag{iPiece, jPiece, lPiece, oPiece,
                                                                          sPiece, tPiece, zPiece},
                                                                      currentPiece(bag[0]),
                                                                      nextPiece(bag[1])
{
    nextPieceBagPosition = 0;
    currentOrientation = 0;
    gameOver = false;
    shuffleBag();
    currentPiece = bag[nextPieceBagPosition];
    addCurrentPieceToBoard();

    nextPieceBagPosition++;
    nextPiece = bag[nextPieceBagPosition];
}

bool Tetris::AutoTetrisBoard::tryGetNewPiece()
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

bool Tetris::AutoTetrisBoard::tryLowerPiece()
{
    bool canMove = true;

    uint8_t _col;
    uint8_t _row;

    for (int i = 0; i < NUM_PIECE_SEGMENTS; i++) // search from beginning of list if moving left, end of list if moving right
    {
        _row = currentPieceCoordinates[i][0];
        _col = currentPieceCoordinates[i][1];

        //      T-piece spawn position
        //                      [0][0],[0][1]=0,4
        // [1][0],[1][1]=1,3    [2][0],[2][1]=1,4    [3][0],[3][1]=1,5

        bool foundBetterSegment = false;
        for (int j = i + 1; j < NUM_PIECE_SEGMENTS; j++) // search the rest for a lefter segment in same row
        {
            if (currentPieceCoordinates[j][1] == _col) // same column
            {
                foundBetterSegment = true; // coords are ordered by row so a match has to be a lower row
                break;                     // just break cuz we'll get to it later
            }
        }
        if (!foundBetterSegment)
        {
            if (_row + 1 == 64 || aBoard[_row + 1][_col] != 0)
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
            aBoard[currentPieceCoordinates[i][0]][currentPieceCoordinates[i][1]] = 0;
        }
        currentPieceTopLeft[0] += 1;

        for (int i = 0; i < NUM_PIECE_SEGMENTS; i++)
        {
            currentPieceCoordinates[i][0] += 1;
            aBoard[currentPieceCoordinates[i][0]][currentPieceCoordinates[i][1]] = currentPiece.color;
        }
    }
    else
    {
        checkForLineClear();
    }

    return canMove;
}

void Tetris::AutoTetrisBoard::resetBoard()
{
    gameOver = false;
    for (int _row = 0; _row < 64; _row++)
    {
        for (int _col = 0; _col < 64; _col++)
        {
            aBoard[_row][_col] = 0;
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

void Tetris::autoDrawBoard()
{
    for (int row = 0; row < 64; row++)
    {
        for (int col = 0; col < 64; col++)
        {
            if (autoTempBoard[row][col] != autoBoard->aBoard[row][col])
            {
                autoTempBoard[row][col] = autoBoard->aBoard[row][col];

                for (int rowOffset = 0; rowOffset < BOARD_PIXEL_SIZE; rowOffset++)
                {
                    for (int colOffset = 0; colOffset < BOARD_PIXEL_SIZE; colOffset++)
                    {
                        utility->display.drawPixel(BOARD_X_POSITION + (BOARD_PIXEL_SIZE * col) + colOffset,
                                                   BOARD_Y_POSITION + (BOARD_PIXEL_SIZE * row) + rowOffset, autoBoard->aBoard[row][col]);
                    }
                }
            }
        }
    }
}

bool Tetris::autoLoopGame()
{
    if (justStarted)
    {
        utility->display.clearDisplay();
        BOARD_PIXEL_SIZE = 1;
        BOARD_X_POSITION = 0;
        BOARD_Y_POSITION = 0;
        autoDrawBoard();
        delay(1000);
        justStarted = false;
    }

    checkForPause();

    if (!paused)
    {
        if ((millis() - msPrevious) > updateDelay / 2)
        {
            msPrevious = millis();
            if (!autoBoard->gameOver)
            {
                bool pieceAtBottom = false;
                pieceAtBottom = !autoBoard->tryLowerPiece();
                if (pieceAtBottom)
                {
                    if (!autoBoard->tryGetNewPiece())
                    {
                        autoBoard->gameOver = true;
                    }
                }
                autoDrawBoard();
            }
            else
            {
                autoBoard->resetBoard();
                for (int _row = 0; _row < 64; _row++)
                {
                    for (int _col = 0; _col < 64; _col++)
                    {
                        autoTempBoard[_row][_col] = 0;
                    }
                }
                delay(GAME_OVER_DELAY);
                utility->display.clearDisplay();
            }
        }
    }
    else
    {
        utility->inputs.update2(utility->inputs.pins.p2Buttons);
        if (utility->inputs.B_P2_pressed)
        {
            return false;
        }
    }

    return true;
}