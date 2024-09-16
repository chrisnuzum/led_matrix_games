#ifndef _led_arcade_
#define _led_arcade_

// esp-idf extension in VSCode should have more functionality, but not sure if a lot of things will break vs PlatformIO
// that should allow the commands to burn eFuses to be able to use GPIO (I think) 12
// also heap tracing: https://github.com/espressif/vscode-esp-idf-extension/blob/HEAD/docs/tutorial/heap_tracing.md

/*
Ideas:
-Main menu shows preview of selected game, maybe even preview selected options
-Main menu could include a screen to setup where each player is sitting
    -Players could move an arrow around the screen to point to where they are sitting
    -Games could take this into account to determine input directions (maybe just send this to utility?),
        orientation of screen items, and starting movement directions (snake)
    -This setup could also include ability to add a color and/or name for each player

BUGS:
-delete game does not seem to be working
    -try to implement "game" object functionality
    -https://stackoverflow.com/questions/32002392/assigning-a-derived-object-to-a-base-class-object-without-object-slicing

TO-DO:
-Change ALL MATRIX_WIDTHs and MATRIX_HEIGHTs to uint16_ts for larger displays.
-Replace all unsigned longs with uint_32t
-Fork PxMatrix and push changes to it, use that version in MyDisplay.h (see note there)
-Address all the warnings when building the Windows version with the makefile
-Display
    -get rid of matrix interrupt in MyDisplay.h - DONE
    -add display.display() in the main loop() with #ifdef - DONE
    -added display.display() in Menu functions - DONE
    -implement my own delay function that is a while() loop that still calls display.display() every 2ms - DONE
-Games in general
    -pause screen can have a text menu overlay to resume, quit, or restart
-Snake Game
    -see SnakeGame.h for current bugs
    -implement pixel size and color selection options
    -autoplay currently ignores the speed options
    -on PC, movement doesn't zigzag when holding 2 directions at once
-Tetris
    -see Tetris.h for current bugs
    -no options yet implemented: pixel size, color palette, game speed, board size (hide score for bigger board?)
        -completely random pieces vs the repeating bag
    -implement an actual autoplay AI
        -https://www.vidarholen.net/contents/junk/tetris/   see Downloads/Tetris AI/
-Menus
    -See Menu.h for more detail
    -Put name of game and number of players selected at top of Options screen
    -Add "Back" option to menu screens (while keeping B for back functionality)
    -Game over screen (and pause screens) should have options to replay or exit to the main menu
    -Options that are displayed should take into account the number of players selected (autoplay might have different options)
    -Implement more option types: color chooser, pixel size, boolean, as well as dependent option types (are only enabled if another option is enabled)
*/
/*
Interesting reads for better code:
https://stackoverflow.com/questions/23258650/sleep1-and-sdl-delay1-takes-15-ms
https://stackoverflow.com/questions/34854951/sleep-or-wait-that-doesnt-stop-the-whole-program
*/

// #define PC_BUILD

#include <Utility.h>
#include <Menu.h>
#include <SnakeGame.h>
#include <Tetris.h>

bool gameRunning = false;
SnakeGame *snakeGame = nullptr; // display wouldn't work right if not defining this later (in loop()) - see notes on 136/139 in setup() about .setDisplay()
Tetris *tetris = nullptr;
const uint8_t NUMBER_OF_GAMES = 2;
Utility utility = Utility();
Menu menu(utility);
int8_t selectedGame = -1;         // passed by ref
int8_t selectedPlayerOption = -1; // passed by ref
int8_t lastPlayedGame = -1;
BaseGame *games[NUMBER_OF_GAMES] = {snakeGame, tetris};
BaseGame *game = nullptr;

bool gameExists[NUMBER_OF_GAMES] = {false, false};

MenuItem gameMenuItems[NUMBER_OF_GAMES + 1];
MenuItem snakeMenuItem("SNAKE");
MenuItem tetrisMenuItem("TETRIS");
MenuItem utilityMenuItem("Tests");
const char *snakePlayerOptions[MAX_NUM_PLAYER_OPTIONS] = {"Auto", "1P", "2P"};
const char *tetrisPlayerOptions[MAX_NUM_PLAYER_OPTIONS] = {"Auto", "1P", "2P"};
const char *utilityOptions[2] = {"Input", "Colors"};

void setUpMainMenu()
{
    snakeMenuItem.makeItemWithSubitems(snakePlayerOptions, MAX_NUM_PLAYER_OPTIONS, -1);
    tetrisMenuItem.makeItemWithSubitems(tetrisPlayerOptions, MAX_NUM_PLAYER_OPTIONS, -1);
    utilityMenuItem.makeItemWithSubitems(utilityOptions, 2, -1);

    gameMenuItems[0] = snakeMenuItem;
    gameMenuItems[1] = tetrisMenuItem;
    gameMenuItems[2] = utilityMenuItem;
}
/*
Main Menu

>Games
 Tests
 Options

Games
>Snake
  >Auto  1P  2P
 Tetris
 Pac-Man

Tests
>Colors
 Inputs

Options
>Set Player Positions
 Set Player Names


*/

#ifdef PC_BUILD
Uint32 lastDisplayUpdate = SDL_GetTicks();
#else
unsigned long lastDisplayUpdate = 0;
#endif

void setup()
{
    // Serial.begin(115200); //  Serial being active slows down main loop a lot
    Serial.println("Serial connection started");

    initDisplay();
    Serial.println("Display initialized");
    utility.setDisplay();

    utility.display.setRotation(0);
    utility.display.setFont(utility.fonts.my5x5round);
    setUpMainMenu();
}

void loop()
{
#ifdef PC_BUILD
    SDL_Delay(1); // this could help with high CPU usage
    if (SDL_GetTicks() - lastDisplayUpdate > DISPLAY_UPDATE_TIME)
    {
        utility.display.display();
        lastDisplayUpdate = SDL_GetTicks();
    }
#else
    delay(1);
    if (millis() - lastDisplayUpdate > DISPLAY_UPDATE_TIME)
    {
        utility.display.display();
        lastDisplayUpdate = millis();
    }
#endif

    if (!gameRunning) // TODO: cleanup/destruct games when switching to one after playing another
    {
        utility.display.setRotation(0);
        utility.display.setFont(utility.fonts.my5x5round);
        menu.doMenu(gameMenuItems, selectedGame, selectedPlayerOption);
        Serial.print("Game index: ");
        Serial.println(selectedGame);
        Serial.print("Player option index: ");
        Serial.println(selectedPlayerOption);
        if (selectedGame == 0)
        {
            if (lastPlayedGame != 0 && gameExists[lastPlayedGame]) // if game has switched, delete previous game to conserve RAM
            {
                delete game; // not working
                gameExists[lastPlayedGame] = false;
                lastPlayedGame = 0;
            }

            if (!gameExists[selectedGame])
            {
                snakeGame = new SnakeGame(utility, selectedPlayerOption);
                game = snakeGame;
                gameExists[selectedGame] = true;
            }
            else
            {
                snakeGame->setPlayers(selectedPlayerOption);
                snakeGame->justStarted = true;
            }
            // do options menu
            MenuItem numberOfApples("Num apples");
            numberOfApples.makeItemWithValue(3, 1, 5);
            MenuItem startSpeed("Start speed");
            startSpeed.makeItemWithValue(5, 1, 10);
            MenuItem maxSpeed("Max speed");
            maxSpeed.makeItemWithValue(7, 1, 10);
            MenuItem startGame("Start game");

            MenuItem options[4] = {numberOfApples, startSpeed, maxSpeed, startGame};
            int8_t chosenOption = -1;

            while (true)
            {
                if (!menu.doOptionsMenu(options, chosenOption)) // This means user wants to back out to previous menu
                {
                    break;
                }

                switch (chosenOption)
                {
                case 0:
                    snakeGame->setNumApples(options[0].getValue());
                    break;
                case 1:
                    snakeGame->setStartSpeed(options[1].getValue());
                    break;
                case 2:
                    snakeGame->setMaxSpeed(options[2].getValue());
                    break;
                case 3:
                    gameRunning = true;
                    break;
                }
                if (chosenOption == 3)
                {
                    break;
                }
            }
        }
        else if (selectedGame == 1)
        {
            if (lastPlayedGame != 1 && gameExists[lastPlayedGame]) // if game has switched, delete previous game to conserve RAM
            {
                delete game;
                gameExists[lastPlayedGame] = false;
                lastPlayedGame = 1;
            }

            if (!gameExists[selectedGame])
            {
                tetris = new Tetris(utility, selectedPlayerOption);
                game = tetris;
                gameExists[selectedGame] = true;
            }
            else
            {
                tetris->setPlayers(selectedPlayerOption);
                tetris->justStarted = true;
            }
            gameRunning = true;
        }
        else if (selectedGame == 2)
        {
            if (lastPlayedGame != 2 && gameExists[lastPlayedGame]) // if game has switched, delete previous game to conserve RAM
            {
                delete game;
                gameExists[lastPlayedGame] = false;
                lastPlayedGame = 2;
            }
            if (selectedPlayerOption == 0)
            {
                utility.inputTest();
            }
            else if (selectedPlayerOption == 1)
            {
                utility.colorTest();
            }
        }

        // game->setPlayers(selectedPlayerOption);
        // game->justStarted = true;
    }
    else // game is running
    {
        if (selectedGame == 0)
        {
            gameRunning = snakeGame->loopGame();
        }
        else if (selectedGame == 1)
        {
            gameRunning = tetris->loopGame();
        }
        // gameRunning = game->loopGame();  // crashes
    }
}

// for (int i = 0; i < NUMBER_OF_GAMES; i++)
// {
//     bool foundSuboption = false;
//     for (int j = 0; j < MAX_NUM_PLAYER_OPTIONS; j++)
//     {
//         if (menus[i].playerOptions[j])
//         {
//             foundSuboption = true;
//             break;
//         }
//     }
//     ItemWithSubitems item(menus[i].gameDisplayName, menus[i].playerOptions, foundSuboption ? MAX_NUM_PLAYER_OPTIONS : 0, -1);
//     items[i] = item;
// }

// possibility to separate display from the Game files: game source code compiles a list/array of points (with colors) to draw
// and passes that back to loop function
// Point drawPoints[] = snakeGame.loopGame();       //couldn't actually be an array cuz of set size unless it contains all points in matrix
// for (Point p : drawPoints)
// {
//  utility.display.drawPixel(p.x, p.y, p.color);
// }

// void scroll_text(uint8_t ypos, unsigned long scroll_delay, String text, uint8_t colorR, uint8_t colorG, uint8_t colorB) {
//   uint16_t text_length = text.length();
//   utility.display.setTextWrap(false);  // we don't wrap text so it scrolls nicely
//   utility.display.setTextSize(1);
//   utility.display.setRotation(0);
//   utility.display.setTextColor(utility.display.color565(colorR, colorG, colorB));

//   // Assuming 5 pixel average character width
//   for (int xpos = MATRIX_WIDTH; xpos > -(MATRIX_WIDTH + text_length * 5); xpos--) {
//     utility.display.setTextColor(utility.display.color565(colorR, colorG, colorB));
//     utility.display.clearDisplay();
//     utility.display.setCursor(xpos, ypos);
//     utility.display.println(text);
//     delay(scroll_delay);
//     yield();

//     // This might smooth the transition a bit if we go slow
//     // utility.display.setTextColor(utility.display.color565(colorR/4,colorG/4,colorB/4));
//     // utility.display.setCursor(xpos-1,ypos);
//     // utility.display.println(text);

//     delay(scroll_delay / 5);
//     yield();
//   }
// }

// if (!gameRunning) // TODO: cleanup/destruct games when switching to one after playing another
// {
//     selected_game = selectMenuItem(game_strings, NUMBER_OF_GAMES);
//     selected_players = selectMenuItem(player_strings, num_player_options);

//     if (selected_game == 1)
//     {
//         if (snakeGame == nullptr) // if game has switched, maybe delete/set previous game to nullptr to conserve RAM
//         {
//             snakeGame = new SnakeGame(utility, selected_players == 3 ? 0 : selected_players);
//         }
//         else
//         {
//             snakeGame->setPlayers(selected_players == 3 ? 0 : selected_players);
//             snakeGame->justStarted = true;
//         }
//         gameRunning = true;
//     }
//     else if (selected_game == 2)
//     {
//         if (tetris == nullptr) // if game has switched, maybe delete/set previous game to nullptr to conserve RAM
//         {
//             tetris = new Tetris(utility, selected_players == 3 ? 0 : selected_players);
//         }
//         else
//         {
//             tetris->setPlayers(selected_players == 3 ? 0 : selected_players);
//             tetris->justStarted = true;
//         }
//         gameRunning = true;
//     }
// }
// else // game is running
// {
//     if (selected_game == 1)
//     {
//         gameRunning = snakeGame->loopGame();
//     }
//     else if (selected_game == 2)
//     {
//         gameRunning = tetris->loopGame();
//     }
// }

// MenuInfo menus[] = {getSnakeMenu(), getTetrisMenu()};
// const char *gameNames[NUMBER_OF_GAMES];
// const char *gamePlayerOptions[NUMBER_OF_GAMES][NUM_GAME_MODES];

// might be better to just build the ItemWithSubitems objects here !!!!!!!!!!!!!!!!
// for (int i = 0; i < NUMBER_OF_GAMES; i++)
// {
//     gameNames[i] = menus[i].gameDisplayName;
//     Serial.println("set gameNames[i]");
//     // gamePlayerOptions[i] = menus[i].playerOptions;
//     for (int j = 0; j < NUM_GAME_MODES; j++)
//     {
//         Serial.println("beginning of loop");
//         gamePlayerOptions[i][j] = menus[i].playerOptions[j];
//         Serial.println("set gamePlayerOptions[i][j]");
//     }
// }

// menu.setUpMainMenuWithSubitems(NUMBER_OF_GAMES, gameNames, gamePlayerOptions, NUM_GAME_MODES, menus->playerOptionNames);

//     menu.doMenu(items, selectedGame, selectedPlayerOption);

//     if (games[selectedGame] == nullptr) // if game has switched, maybe delete/set previous game to nullptr to conserve RAM
//     {
//         if (games[selectedGame] == snakeGame) // not sure if this is checking if they're actually the same object or equal to
//         {                                     // same value, at the beginning both will be equal to nullptr. Might need to use & to check that addresses match?
//             games[selectedGame] = new SnakeGame(utility, selectedPlayerOption);
//         }
//         else if (games[selectedGame] == tetris)
//         {
//             games[selectedGame] = new Tetris(utility, selectedPlayerOption);
//         }
//     }
//     else
//     {
//         games[selectedGame]->setPlayers(selectedPlayerOption);
//         games[selectedGame]->justStarted = true;
//     }
//     gameRunning = true;
// }
// else // game is running
// {
//     gameRunning = games[selectedGame]->loopGame();
// }

// int8_t initialSelectedSuboption = -1;
// for (int j = 0; j < NUM_GAME_MODES; j++)
// {
//     if (menus[i].playerOptions[j])
//     {
//         initialSelectedSuboption = j;
//         break;
//     }
// }
// if (initialSelectedSuboption == -1)
// {
//     Serial.println("no suboptions");
//     ItemWithSubitems item(menus[i].gameDisplayName, menus[i].playerOptions, 0, -1); // could get rid of if/else by changing end to: , initialSelectedSuboption == -1 ? 0 : NUM_GAME_MODES, initialSelectedSuboption);
//     items[i] = item;
// }
// else
// {
//     Serial.println("has suboptions");
//     ItemWithSubitems item(menus[i].gameDisplayName, menus[i].playerOptions, NUM_GAME_MODES, initialSelectedSuboption);
//     Serial.println("made item");
//     items[i] = item;
//     Serial.println("added item to items");
// }

#endif