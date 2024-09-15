#ifndef _pacman_
#define _pacman_

#include <BaseGame.h>
/*
https://hackaday.com/2015/06/01/1-pixel-pacman/
https://upload.wikimedia.org/wikipedia/en/thumb/5/59/Pac-man.png/220px-Pac-man.png
https://www.reddit.com/r/Pacman/comments/16p82dk/pacman_64x64_with_score_and_lives/

https://pacman.fandom.com/wiki/Pac-Man_(game)

To have gameplay closer to the original (on 64x64), the view would need to be zoomed in and then follow Pac-Man as he
navigates the board.

Positioning:
-I could store an integer that corresponds to a position on the board for each item that needs a position.
    -300 possible positions (with larger matrices there could be multiple spaces between pellets so this number would be larger)
-Or I could use coordinates.
    -51x57 possible positions (with larger matrices there could be multiple spaces between pellets so this number would be larger)
    -(22,4) would be 3rd dash on 4th row.
-With these two options, I need a function to convert the coordinates to the actual matrix coordinates.
-Otherwise, I store the entire map with walls

-Store 28x31 board map in memory.
-Step through board and save every possible Pac-Man/ghost position (pellet or space) to a list (ignore walls).
    -Ideally the index in the list uniquely refers to the position to keep the list small and avoid searching the list and
     storing x and y coordinates in each object
        -300 possible positions (besides ghost home positions)
        -Upon encountering one of [.,o, ]:
        Position positions[300];
        int positionCount = 0;
        int charCount = 0;
        for (char in map)
        {
            if (char == '.' || char == 'o' || char == ' ')
            {
                Position p;

            }
            charCount++;
        }
    -Each pellet would have a boolean variable isActive
    -Power pellets could be saved to a separate list or each object in this list could have a boolean variable isPowerPellet
    -struct Pellet
    {
        int xPos, yPos?
        bool hasPellet, isPowerPellet, isPelletActive
        bool wallLeft, wallRight, wallUp, wallDown

    }
-Determine maximum size of board possible for given matrix dimensions.
    -This is based on adding empty spaces between the pellets. The number of spaces between the pellets is saved and just
     used to draw the board.

-Each time Pac-Man is going to move to a new position:
    1. First do something like spacing == 0 || ((xPos % spacing) == 0 && (yPos % spacing) == 0) to determine whether Pac-Man's
       new position is in the basic 28x31 map. If either check is > 0, then the new position is in between those map coords
       (extra spaces on larger matrices) and some checks can be skipped.
        -If true (new position matches a map position):
            -check list of pellets to see if p
    -that position is checked to see if it is active in the list of pellets.
        -Could do something like (xPos % spacing) == 0 && (yPos % spacing) == 0 to reduce the number of checks in the list? But
         this could be slower depending on what type of list is used.


struct PacMan
{
    int currentMapIndex;    // 0-299
    int currentSubspace;    // a number referring to the current space PacMan is on between pellets

}

These are maps equivalent to original:
Smallest possible representation of the board (all positions accessible):
32x32(28x31) with pellets directly next to one another:
############################
#............##............#
#.####.#####.##.#####.####.#
#o####.#####.##.#####.####o#
#.####.#####.##.#####.####.#
#..........................#
#.####.##.########.##.####.#
#.####.##.########.##.####.#
#......##....##....##......#
######.##### ## #####.######
######.##### ## #####.######
######.##          ##.######
######.## ###--### ##.######
######.## ###@@### ##.######
      .   ##@@@@##   .
######.## ######## ##.######
######.## ######## ##.######
######.##          ##.######
######.## ######## ##.######
######.## ######## ##.######
#............##............#
#.####.#####.##.#####.####.#
#.####.#####.##.#####.####.#
#o..##.......  .......##..o#
###.##.##.########.##.##.###
###.##.##.########.##.##.###
#......##....##....##......#
#.##########.##.##########.#
#.##########.##.##########.#
#..........................#
############################

64x64(53x59) with one space in between pellets:
#####################################################
#. . . . . . . . . . . .#####. . . . . . . . . . . .#
# ######### ########### ##### ########### ######### #
#.#########.###########.#####.###########.#########.#
# ######### ########### ##### ########### ######### #
#o#########.###########.#####.###########.#########o#
# ######### ########### ##### ########### ######### #
#.#########.###########.#####.###########.#########.#
# ######### ########### ##### ########### ######### #
#. . . . . . . . . . . . . . . . . . . . . . . . . .#
# ######### ##### ################# ##### ######### #
#.#########.#####.#################.#####.#########.#
# ######### ##### ################# ##### ######### #
#.#########.#####.#################.#####.#########.#
# ######### ##### ################# ##### ######### #
#. . . . . .#####. . . .#####. . . .#####. . . . . .#
########### ##########  #####  ########## ###########
###########.##########  #####  ##########.###########
########### ##########  #####  ########## ###########
###########.##########  #####  ##########.###########
########### ##########  #####  ########## ###########
###########.####                     ####.###########
########### ####  ######@@@@@######  #### ###########
###########.####  #@@@@@@@@@@@@@@@#  ####.###########
########### ####  #@@@@@@@@@@@@@@@#  #### ###########
###########.####  #@@@@@@@@@@@@@@@#  ####.###########
########### ####  #@@@@@@@@@@@@@@@#  #### ###########
           .      #@@@@@@@@@@@@@@@#      .
########### ####  #@@@@@@@@@@@@@@@#  #### ###########
###########.####  #@@@@@@@@@@@@@@@#  ####.###########
########### ####  #@@@@@@@@@@@@@@@#  #### ###########
###########.####  #@@@@@@@@@@@@@@@#  ####.###########
########### ####  #################  #### ###########
###########.####                     ####.###########
########### ####  #################  #### ###########
###########.####  #################  ####.###########
########### ####  #################  #### ###########
###########.####  #################  ####.###########
########### ####  #################  #### ###########
#. . . . . . . . . . . .#####. . . . . . . . . . . .#
# ######### ########### ##### ########### ######### #
#.#########.###########.#####.###########.#########.#
# ######### ########### ##### ########### ######### #
#.#########.###########.#####.###########.#########.#
# ######### ########### ##### ########### ######### #
#o . .#####. . . . . . .     . . . . . . .#####. . o#
##### ##### ##### ################# ##### ##### #####
#####.#####.#####.#################.#####.#####.#####
##### ##### ##### ################# ##### ##### #####
#####.#####.#####.#################.#####.#####.#####
##### ##### ##### ################# ##### ##### #####
#. . . . . .#####. . . .#####. . . .#####. . . . . .#
# ##################### ##### ##################### #
#.#####################.#####.#####################.#
# ##################### ##### ##################### #
#.#####################.#####.#####################.#
# ##################### ##### ##################### #
#. . . . . . . . . . . . . . . . . . . . . . . . . .#
#####################################################

newWidth = priorWidth + priorWidth - 2(frameSize) - 1
53 = 28 + 28 - 2 - 1

same formula for height
59 = 31 + 31 - 2 - 1

// determines largest number of spaces between pellets possible
// -should individually check height and width
// -should use FRAME_SIZE instead of - 2
// -should account for leaving space for score
int8_t stretch(uint16_t height, uint16_t width)
{
    uint16_t _smallestDimension = height < width ? height : width;
    uint16_t initialSize = 31; //using larger dimension (height)
    uint16_t nextSize = initialSize * 2 - 2 - 1;
    uint8_t count = 0;
    while (_smallestDimension > nextSize)
    {
        count++;
        nextSize = nextSize * 2 - 2 - 1;
    }
}





For a 64x64 game:
Pac-Man, ghosts, and fruit are 5x5
Pellets are 1x1 (power pellets can maybe be 3x3 x or + different color, maybe animated pulsing?)


The map constrains Pac-Man's movement

Game needs to track:
-Pac-Man's position, direction, and animation frame
-all 4 ghosts' position, direction, and animation frame
-pellets still present on board

Multiplayer:
https://www.reddit.com/r/Pacman/comments/zkgs0b/is_there_a_2_player_pacman/
-Cooperative and competitive modes
-Ms. Pac-Man competitive: https://www.youtube.com/watch?v=6PA-dOXkFk0
    -when P1 gets a power pellet, P2 can't interact with pellets or ghosts for same duration as ghosts are vulnerable
        -if P2 goes over a power pellet, it goes back to normal where both players can interact
    -if P1 and P2 collide in either state, they bounce away from each other
-Ms. Pac-Man cooperative: https://www.youtube.com/watch?v=lDmUkMi2_DI
    -normal gameplay but collisions make them bounce away from each other


*/

class Pacman : public BaseGame
{
public:
    Pacman(Utility &utility, uint8_t numPlayers);
    void setPlayers(uint8_t players);
    bool loopGame();

private:
    static const uint8_t MAX_PLAYERS_SUPPORTED = 2;
    static const uint8_t BASE_BOARD_WIDTH = 28;
    static const uint8_t BASE_BOARD_HEIGHT = 31;

    struct Position
    {
        uint8_t x, y;
        bool hasPellet = false, isPowerPellet = false, isPelletActive = false;
        bool isGhostHomeArea = false;
        bool wallLeft = true, wallRight = true, wallUp = true, wallDown = true;
        bool warpLeft = false, warpRight = false, ghostAreaDown = false;
        Position *posLeft = nullptr;
        Position *posRight = nullptr;
        Position *posUp = nullptr;
        Position *posDown = nullptr;
    };

    struct Mover
    {
    };

    Position *positions[300];
    Mover ghosts[4];
    Mover players[MAX_PLAYERS_SUPPORTED];
    uint8_t boardRowPositions[BASE_BOARD_HEIGHT];

    const char board[BASE_BOARD_HEIGHT][BASE_BOARD_WIDTH + 1] = // + 1 because of null ending char '\0'
        {{"############################"},
         {"#............##............#"},
         {"#.####.#####.##.#####.####.#"},
         {"#o####.#####.##.#####.####o#"},
         {"#.####.#####.##.#####.####.#"},
         {"#..........................#"},
         {"#.####.##.########.##.####.#"},
         {"#.####.##.########.##.####.#"},
         {"#......##....##....##......#"},
         {"######.##### ## #####.######"},
         {"######.##### ## #####.######"},
         {"######.##          ##.######"},
         {"######.## ###--### ##.######"},
         {"######.## ###@@### ##.######"},
         {"      .   ##@@@@##   .      "},
         {"######.## ######## ##.######"},
         {"######.## ######## ##.######"},
         {"######.##          ##.######"},
         {"######.## ######## ##.######"},
         {"######.## ######## ##.######"},
         {"#............##............#"},
         {"#.####.#####.##.#####.####.#"},
         {"#.####.#####.##.#####.####.#"},
         {"#o..##.......  .......##..o#"},
         {"###.##.##.########.##.##.###"},
         {"###.##.##.########.##.##.###"},
         {"#......##....##....##......#"},
         {"#.##########.##.##########.#"},
         {"#.##########.##.##########.#"},
         {"#..........................#"},
         {"############################"}};
    // const char board2[BASE_BOARD_HEIGHT * BASE_BOARD_WIDTH + 1] = // + 1 because of null ending char '\0'
    //     "############################"
    //     "#............##............#"
    //     "#.####.#####.##.#####.####.#"
    //     "#o####.#####.##.#####.####o#"
    //     "#.####.#####.##.#####.####.#"
    //     "#..........................#"
    //     "#.####.##.########.##.####.#"
    //     "#.####.##.########.##.####.#"
    //     "#......##....##....##......#"
    //     "######.##### ## #####.######"
    //     "######.##### ## #####.######"
    //     "######.##          ##.######"
    //     "######.## ###--### ##.######"
    //     "######.## ###@@### ##.######"
    //     "      .   ##@@@@##   .      "
    //     "######.## ######## ##.######"
    //     "######.## ######## ##.######"
    //     "######.##          ##.######"
    //     "######.## ######## ##.######"
    //     "######.## ######## ##.######"
    //     "#............##............#"
    //     "#.####.#####.##.#####.####.#"
    //     "#.####.#####.##.#####.####.#"
    //     "#o..##.......  .......##..o#"
    //     "###.##.##.########.##.##.###"
    //     "###.##.##.########.##.##.###"
    //     "#......##....##....##......#"
    //     "#.##########.##.##########.#"
    //     "#.##########.##.##########.#"
    //     "#..........................#"
    //     "############################";

    void buildBoardObjects();
};

#endif