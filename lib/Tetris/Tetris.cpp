#include "Tetris.h"
// game board is 10 wide by 20 tall, so "pixels" can be 2x2 and still have room to fit 2 players side-by-side
//   with a 1 pixel border around the games and a 1 pixel border between them = 45 horizontally x 42 vertically
//   this leaves 19 horizontally x 22 vertically for score & next piece info
// keep array of arrays to track the game board
//   array of size 10 for each line
//   array items correspond to the color of that pixel (or none)

// controls:
//   joystick:
//     left/right: moves piece left or right
//     down: soft drop, moves piece down faster
//     [maybe] up: hard drop, moves to bottom and locks immediately
//   A: hard drop, moves to bottom and locks immediately OR [maybe] rotate piece counter-clockwise
//   B: rotate piece clockwise

// pieces will be stored in variables ahead of time
// hard-coded or calculated rotation?
//    here is the official rotations with the spawn orientation on the left: https://static.wikia.nocookie.net/tetrisconcept/images/3/3d/SRS-pieces.png/revision/latest?cb=20060626173148
//    I and O spawn in center, others favor left of center
// piece colors:
//    I: light blue/teal
//    J: dark blue
//    L: orange
//    O: yellow
//    S: green (not dark)
//    T: pink
//    Z: red
// "bag" of all seven pieces is randomly ordered from the start
// this ensures that a streak of bad pieces does not occur

// as a piece falls (no input):
//   check to see that each spot under the piece is not occupied
//   if good > drop piece after the current pause
//   if not > lock in current position after the current pause
//   could show where hard drop would land in a lighter color

// when a rotate is requested:
//   check rotation table for desired position of piece
//   if good > display the rotation immediately
//   if rotation won't fit to left(or right) > try rotated position shifted 1 to the right(or left)
//   to kick off of the sides most pieces would only need to shift 1, but I piece could need to shift 2
//   if not good, rotation fails (could try shifting piece up or down like the kick tables)

Tetris::Tetris(uint8_t MATRIX_WIDTH, uint8_t MATRIX_HEIGHT, PxMATRIX display, Utility &utility) : MATRIX_WIDTH(MATRIX_WIDTH),
                                                                                                     MATRIX_HEIGHT(MATRIX_HEIGHT),
                                                                                                     display(display),
                                                                                                     utility(&utility)
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
        
    }
}

void Tetris::setPlayers(uint8_t numPlayers)
{
    this->numPlayers = numPlayers;
}

void Tetris::checkForPause()
{
    utility->inputs.update();
    bool tempState = utility->inputs.START;
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
            // drawSnakes();
            // drawApple();
            msPrevious = millis();
            doPauseToggle = false;
        }
    }
    lastTempState = tempState;
}

void Tetris::loopGame()
{
    checkForPause();

    if (!paused)
    {

    }
}