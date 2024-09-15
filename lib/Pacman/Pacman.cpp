#include "Pacman.h"

void Pacman::buildBoardObjects()
{
    uint16_t positionCount = 0;
    uint16_t charCount = 0;
    uint16_t firstPositionInRow[BASE_BOARD_HEIGHT];
    for (uint8_t currentRow = 0; currentRow < BASE_BOARD_HEIGHT; currentRow++)
    {
        firstPositionInRow[currentRow] = positionCount;
        uint8_t positionsInRow = 0;
        for (const char &c : board[currentRow])
        {
            if (c == '.' || c == 'o' || c == ' ' || c == '-' || c == '@') // if a valid player spot; TODO: need to handle ghost home area
            {
                positionCount++;
                positionsInRow++;
                positions[positionCount] = new Position;
                Position *p = positions[positionCount];

                if (c == '.')
                {
                    p->hasPellet = true;
                    p->isPelletActive = true;
                }
                else if (c == 'o')
                {
                    p->hasPellet = true;
                    p->isPowerPellet = true;
                    p->isPelletActive = true;
                }
                else if (c == ' ')
                {
                }
                else if (c == '-' || c == '@')
                {
                    p->isGhostHomeArea == true;
                }
                p->y = currentRow;
                p->x = charCount % BASE_BOARD_WIDTH;

                // check for walls up and to the left, and if walls are not found set Position pointers for both items
                if (currentRow > 0) // skip top row, nothing above it
                {
                    // if first or last character in row then they are a horizontal warp (edges are normally walls)
                    if (charCount % (BASE_BOARD_WIDTH + 1) == 0) // first character in row
                    {
                        p->warpLeft = true;
                    }
                    else if (charCount % BASE_BOARD_WIDTH == 0) // last character in row
                    {
                        p->warpRight = true;
                        // also find the first Position in this row and update posLeft pointer for that one and posRight pointer for this one
                        p->posRight = positions[firstPositionInRow[currentRow]];
                        positions[firstPositionInRow[currentRow]]->posLeft = p;
                    }
                    else
                    {
                        // search prior row for matching x coordinate
                        if (boardRowPositions[currentRow - 1] > 0)
                        {
                            for (int posInRow = 0; posInRow < boardRowPositions[currentRow - 1]; posInRow++)
                            {
                                Position *tempP = positions[firstPositionInRow[currentRow - 1] + posInRow];
                                if (tempP->x == p->x)
                                {
                                    p->wallUp = false;
                                    p->posUp = tempP;
                                    tempP->wallDown = false;
                                    tempP->posDown = p;
                                    break;
                                }
                            }
                        }
                        if (positionsInRow > 1)
                        {
                            // check if prior position is immediately to the left of current position
                            Position *tempP = positions[positionCount - 1];
                            if (tempP->x == p->x - 1)
                            {
                                p->wallLeft = false;
                                p->posLeft = tempP;
                                tempP->wallRight = false;
                                tempP->posRight = p;
                            }
                        }
                    }
                }
            }
            charCount++;
        }
        boardRowPositions[currentRow] = positionsInRow;

        /* this function is ready to test probably...... */
    }
    // for (const char &c : board2)
    // {
    //     if (c == '.' || c == 'o' || c == ' ')
    //     {
    //         positions[positionCount] = new Position;
    //         Position p = *positions[positionCount];

    //         if (c == '.')
    //         {
    //             p.hasPellet = true;
    //             p.isPelletActive = true;
    //         }
    //         else if (c == 'o')
    //         {
    //             p.hasPellet = true;
    //             p.isPowerPellet = true;
    //             p.isPelletActive = true;
    //         }
    //         else if (c == ' ')
    //         {
    //         }
    //         // each Position needs to check for walls in all 4 directions and if there is not a wall it saves
    //         // a reference to the Position in that direction

    //         // could check for walls up and to the left, and if not found fill Position pointers for both items
    //         if (charCount >= BASE_BOARD_WIDTH)
    //         {
    //             if (board2[charCount - BASE_BOARD_WIDTH] == '#') // check Positions instead?
    //             {
    //                 p.wallUp = true;
    //             }
    //             // OR, walls default to true
    //             if (board2[charCount - BASE_BOARD_WIDTH] != '#') // check Positions instead?
    //             {
    //                 p.wallUp = false;
    //             }
    //         }

    //         positionCount++;
    //     }
    //     charCount++;
    // }
}
