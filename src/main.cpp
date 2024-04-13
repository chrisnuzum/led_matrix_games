
#define PxMATRIX_double_buffer true // this doesn't do anything, it is all CAPSed in PxMatrix.h

// #include <testfile.h>
#include <Arduino.h>
#include <PxMatrix.h> // if issues down the road, PlatformIO Library manager installs latest release,
                      //    but ArduinoIDE installs the lastest version of the master branch
                      //    only the latest version works, maybe consider manually adding library to project
                      // Definitely need to add my own because I made edits on line 766+ and 13
                      // Should probably fork PxMatrix, push new commit, then use that
                      // !!! DO THIS SOON
#include <Utility.h>
#include <Menu.h>
#include <SnakeGame.h>
#include <Tetris.h>

// esp-idf extension in VSCode should have more functionality, but not sure if a lot of things will break vs PlatformIO
// that should allow the commands to burn eFuses to be able to use GPIO (I think) 12
// also heap tracing: https://github.com/espressif/vscode-esp-idf-extension/blob/HEAD/docs/tutorial/heap_tracing.md

/*
Ideas:

-default controls and screen orientation based on controller that selects the game
    -at the main menu both controllers are active
    -once a game and number of players is selected, only the controller that confirmed the game is active
    -that player becomes player 1 and the screen is rotated towards them, or there is an option to choose the screen rotation


TO-DO:
-add autoplay/screensavers for each game with options for speed/size
    -Snake
        -basic autoplay added, fails when the snake gets decently long
        -probably bugs in the current desired implementation
        -add options for number of apples, size of grid/pixels, and colors
    -Tetris
        -basic autoplay added, just fills screen with tiny pieces
        -placement is random, could add functionality to maybe fill the first half of the screen randomly and then start trying to pick placement and clear lines
        -add options for pixel size, colors, show/hide score
-add Back option to menus
-make more robust menu system with options and a preview of game being played
-have B button exit autoplays
-add game options

ISSUES:
-The way PxMatrix does color seems odd.
  -Color depth by default is set to 4.
  -Colors passed to fillMatrixBuffer are uint_8t r, g, b values (so max of 255)
  -But, each value is bitwise right-shifted by (8 - colorDepth)
        -This means <15 becomes 0, 60 becomes 3, 120 becomes 7, 255 becomes 15
  -figure out what the interlacing does (1001 - 1016)
    -starting with rightmost bit of each color value (least significant bit) just checks if that bit is 1 and then adds it to the buffer
  -check lines 1022-1024
    -
-Figure out what drawPixelRGB565 does to get individual RGB values from a uint16_t color (line 1022)
    -this still would have the same shift done
*/

// Pins for LED MATRIX
#define P_LAT 22
#define P_A 19
#define P_B 23
#define P_C 18
#define P_D 5
#define P_E 15
#define P_OE 16
hw_timer_t *timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;
uint8_t display_draw_time = 1; // 30-60 is usually fine              brightness but only used if fastupdate isn't used

uint8_t MATRIX_WIDTH = 64;
uint8_t MATRIX_HEIGHT = 64;
PxMATRIX display(MATRIX_WIDTH, MATRIX_HEIGHT, P_LAT, P_OE, P_A, P_B, P_C, P_D, P_E);

// uint16_t loops = 0;
// uint16_t fps = 0;
// unsigned long loop_time = 0;

bool gameRunning = false;

void IRAM_ATTR display_updater()
{
    // Increment the counter and set the time of ISR
    portENTER_CRITICAL_ISR(&timerMux);
    display.display(display_draw_time);
    portEXIT_CRITICAL_ISR(&timerMux);
}

void display_update_enable(bool is_enable)
{
    if (is_enable)
    {
        timer = timerBegin(0, 80, true);
        timerAttachInterrupt(timer, &display_updater, true);
        timerAlarmWrite(timer, 2000, true);
        timerAlarmEnable(timer);
    }
    else
    {
        timerDetachInterrupt(timer);
        timerAlarmDisable(timer);
    }
}

SnakeGame *snakeGame = nullptr; // display wouldn't work right if not defining this later (in loop()) - see notes on 136/139 in setup() about .setDisplay()
Tetris *tetris = nullptr;
const uint8_t NUMBER_OF_GAMES = 2;
Utility utility(MATRIX_WIDTH, MATRIX_HEIGHT, display);
Menu menu(utility);
int8_t selectedGame = -1;         // passed by ref
int8_t selectedPlayerOption = -1; // passed by ref
int8_t lastPlayedGame = -1;
BaseGame *games[NUMBER_OF_GAMES] = {snakeGame, tetris};
BaseGame *game = nullptr;

bool gameExists[2] = {false, false};

MenuItem gameMenuItems[NUMBER_OF_GAMES];
MenuItem snakeMenuItem("SNAKE");
MenuItem tetrisMenuItem("TETRIS");
const char *snakePlayerOptions[MAX_NUM_PLAYER_OPTIONS] = {"Auto", "1P", "2P"};
const char *tetrisPlayerOptions[MAX_NUM_PLAYER_OPTIONS] = {"Auto", "1P", "2P"};

/*  change menu to be simpler, build here (no GameOption struct in games, just setter functions that a loop will call here)
    individual games can have player option modes as booleans and name of game in BaseGame (consts?) -this would only work if they are static
    This function will grab these values and build the appropriate Menu objects.
    Any options will need to be manually configured here. The option name, value, min, and max can be passed to Menu.h
    and then the user will select an option and change the value. The I can return the Item object that was modified back to here (should work?)
    and use a switch statement to identify which option it was, and call the relevant setter function and pass the new value.
    Another option of "Start Game" needs to be added to the end of the list in order to actually start the game after the options are set

*/
void setUpMainMenu()
{
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

    snakeMenuItem.makeItemWithSubitems(snakePlayerOptions, MAX_NUM_PLAYER_OPTIONS, -1);
    tetrisMenuItem.makeItemWithSubitems(tetrisPlayerOptions, MAX_NUM_PLAYER_OPTIONS, -1);

    gameMenuItems[0] = snakeMenuItem;
    gameMenuItems[1] = tetrisMenuItem;
}

void setup()
{
    Serial.begin(115200); //  Serial being active slows down main loop a lot
    Serial.println("Serial connection started");

    // Define your display layout here, e.g. 1/8 step, and optional SPI inputs begin(row_pattern, CLK, MOSI, MISO, SS) //display.begin(8, 14, 13, 12, 4);
    display.begin(32);
    display.setFastUpdate(true);

    display.setRotation(0);

    display.clearDisplay();
    display_update_enable(true);
    // delay(2000);
    display.setFont(utility.fonts.my5x5round);
    utility.setDisplay(display); // why is this necessary? needs re-set after the update_enable. Also setting font after this line won't register
    setUpMainMenu();
    menu.setDisplay(display); // this is also necessary like line 136 unless Menu object is defined in same block...
}

void loop()
{                     // https://stackoverflow.com/questions/32002392/assigning-a-derived-object-to-a-base-class-object-without-object-slicing
    if (!gameRunning) // TODO: cleanup/destruct games when switching to one after playing another
    {
        menu.doMenu(gameMenuItems, selectedGame, selectedPlayerOption);
        Serial.print("Game index: ");
        Serial.println(selectedGame);
        Serial.print("Player option index: ");
        Serial.println(selectedPlayerOption);
        if (selectedGame == 0)
        {
            if (lastPlayedGame != 0) // if game has switched, delete previous game to conserve RAM
            {
                delete game;
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
                    snakeGame->setStartSpeed(options[0].getValue());
                    break;
                case 2:
                    snakeGame->setMaxSpeed(options[0].getValue());
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
            if (lastPlayedGame != 1) // if game has switched, delete previous game to conserve RAM
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

// possibility to separate display from the Game files: game source code compiles a list/array of points (with colors) to draw
// and passes that back to loop function
// Point drawPoints[] = snakeGame.loopGame();       //couldn't actually be an array cuz of set size unless it contains all points in matrix
// for (Point p : drawPoints)
// {
//  display.drawPixel(p.x, p.y, p.color);
// }

// void scroll_text(uint8_t ypos, unsigned long scroll_delay, String text, uint8_t colorR, uint8_t colorG, uint8_t colorB) {
//   uint16_t text_length = text.length();
//   display.setTextWrap(false);  // we don't wrap text so it scrolls nicely
//   display.setTextSize(1);
//   display.setRotation(0);
//   display.setTextColor(display.color565(colorR, colorG, colorB));

//   // Assuming 5 pixel average character width
//   for (int xpos = MATRIX_WIDTH; xpos > -(MATRIX_WIDTH + text_length * 5); xpos--) {
//     display.setTextColor(display.color565(colorR, colorG, colorB));
//     display.clearDisplay();
//     display.setCursor(xpos, ypos);
//     display.println(text);
//     delay(scroll_delay);
//     yield();

//     // This might smooth the transition a bit if we go slow
//     // display.setTextColor(display.color565(colorR/4,colorG/4,colorB/4));
//     // display.setCursor(xpos-1,ypos);
//     // display.println(text);

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