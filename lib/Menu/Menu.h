#ifndef _mymenu_
#define _mymenu_

#include <Arduino.h>
// #include <PxMatrix.h>
#include <Utility.h>
// #include <BaseGame.h>

// #include <map>
// #include <functional>
// #include <string>

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
*/
/*
MENU:
    -ideally, there would be a gameplay preview immediately showing the selected game
        -this would be easiest to do at the bottom of the screen but might be hard for Tetris
    -each game would define its own options within its .h file
    -these would be read once the game is selected
    -using 5x5 font, 64 width should fit at least 12 characters across, but most characters are 3 wide, 5 tall, so 20ish should fit across
    -utility->display.getTextBounds() to get coordinates and size of space that a string will fill
        -could use this to see if a string will fit on a line
        -last 4 parameters are 2 int16_t for x & y and 2 uint16_t for w & h, these are passed by reference so will update the passed vars

>Snake
   Auto  1P  2P
 Tetris

    -player options only show up for selected game
    -after selecting a game, a selection arrow appears next to 1P

>Snake
  >Auto  1P  2P
 Tetris

    -use left and right arrows to select the number of players

Snake  1P

>Apples  3
 Start speed  3
 Max speed  10
 Back

    -first use up and down arrows to choose an option
    -upon selecting an option, a selection arrow appears next to the number

Snake  1P

>Apples >3
 Start speed  3
 Max speed  10
 Back

    -use up and down arrows to change number
    -upon pressing A button, selection arrow disappears next to number (returns to previous view)
*/

/*
Each game needs a display name, supported playerOptions list (Auto, 1P, 2P), and options with default values as well as min/max.
To update options, it seems like a container(std::map aka dictionary) of option names as keys and function pointers to setters as values
    -map.insert
Have the ability to check if options are valid and display an error message/change font color to indicate issue
    -for instance if max speed is lower than start speed (probably isn't an issue right now)
Dependent options as in one option isn't available unless another is active
    -example: Snake casual mode which doesn't increase speed, so if casual is active then max speed isn't available
*/

enum ItemType
{
    basicItem,
    itemWithSubitems,
    itemWithValue
};

class MenuItem // all-encompassing item object with type
{              // might also want a boolean type, as well as certain options that depend on the state of another option to
private:       //   determine if they are enabled or not
    ItemType type = ItemType::basicItem;
    const char *itemName;

    const char **subitems;
    uint8_t numSubitems;
    int8_t selectedSubitem;

    bool isSelected;
    uint8_t value;
    uint8_t minValue; // const?
    uint8_t maxValue; // const?

public:
    MenuItem() = default;
    MenuItem(const char *name) : itemName(name)
    {
    }

    void makeItemWithSubitems(const char **subs, const uint8_t numSubs, int8_t selectedSub) // can probably get rid of passing selectedSub and just set it to -1
    {
        type = ItemType::itemWithSubitems;
        subitems = subs;
        numSubitems = numSubs;
        selectedSubitem = selectedSub;
    }

    void makeItemWithValue(uint8_t val, uint8_t minVal, uint8_t maxVal) // could be special constructor
    {
        type = ItemType::itemWithValue;
        isSelected = false;
        value = val;
        minValue = minVal;
        maxValue = maxVal;
    }

    const char *getName()
    {
        return itemName;
    }

    uint8_t getValue()
    {
        return value;
    }

    friend class Menu;
};

class Menu
{
private:
    Utility *utility;
    // PxMATRIX display;
    // const uint8_t MATRIX_WIDTH;
    // const uint8_t MATRIX_HEIGHT;

    const uint8_t FIRST_X_COORD = 1;
    const uint8_t FIRST_Y_COORD = 5;
    const uint8_t ROW_OFFSET = 6;
    const uint8_t HORIZONTAL_VALUE_OFFSET = 52;
    const uint16_t SELECTED_COLOR = utility->colors.white;
    const uint16_t UNSELECTED_COLOR = utility->colors.cyan;

    void displayOptionsMenu(MenuItem items[], uint8_t numItems, uint8_t focusedItem)
    {
        utility->display.clearDisplay();
        for (int i = 0; i < numItems; i++)
        {
            utility->display.setCursor(FIRST_X_COORD, (i * ROW_OFFSET) + FIRST_Y_COORD);
            if (i == focusedItem)
            {
                utility->display.setTextColor(SELECTED_COLOR);
                utility->display.print(">");
                utility->display.print(items[i].itemName);
                if (items[i].type != ItemType::basicItem)
                {
                    if (!items[i].isSelected)
                    {
                        utility->display.setTextColor(UNSELECTED_COLOR);
                    }
                    utility->display.setCursor(FIRST_X_COORD + HORIZONTAL_VALUE_OFFSET, (i * ROW_OFFSET) + FIRST_Y_COORD);
                    utility->display.print(items[i].value);
                }
            }
            else
            {
                utility->display.setTextColor(UNSELECTED_COLOR);
                utility->display.print("  "); // number of spaces is based on current font's width of a space and > character
                utility->display.print(items[i].itemName);
                if (items[i].type != ItemType::basicItem)
                {
                    utility->display.setCursor(FIRST_X_COORD + HORIZONTAL_VALUE_OFFSET, (i * ROW_OFFSET) + FIRST_Y_COORD);
                    utility->display.print(items[i].value);
                }
            }
        }
    }

    int8_t setOptions(MenuItem items[], uint8_t numItems, int8_t previousFocusedItemIndex)
    {
        uint8_t focusedItemIndex = previousFocusedItemIndex >= 0 ? previousFocusedItemIndex : 0;
        displayOptionsMenu(items, numItems, focusedItemIndex);
        while (true)
        {
            utility->inputs.update();
            bool _up = utility->inputs.UP_P1 || utility->inputs.UP_P2;
            bool _down = utility->inputs.DOWN_P1 || utility->inputs.DOWN_P2;
            bool _a = utility->inputs.A_P1 || utility->inputs.A_P2;
            bool _b = utility->inputs.B_P1 || utility->inputs.B_P2;
            if (items[focusedItemIndex].isSelected)
            {
                if (_a || _b)
                {
                    items[focusedItemIndex].isSelected = false;
                    displayOptionsMenu(items, numItems, focusedItemIndex);
                    return focusedItemIndex;
                }
                else if (_up)
                {
                    if (items[focusedItemIndex].value < items[focusedItemIndex].maxValue)
                    {
                        items[focusedItemIndex].value += 1;
                        displayOptionsMenu(items, numItems, focusedItemIndex);
                    }
                }
                else if (_down)
                {
                    if (items[focusedItemIndex].value > items[focusedItemIndex].minValue)
                    {
                        items[focusedItemIndex].value -= 1;
                        displayOptionsMenu(items, numItems, focusedItemIndex);
                    }
                }
            }
            else
            {
                if (_a)
                {
                    if (items[focusedItemIndex].type == ItemType::basicItem)
                    {
                        return focusedItemIndex;
                    }
                    else
                    {
                        items[focusedItemIndex].isSelected = true;
                        displayOptionsMenu(items, numItems, focusedItemIndex);
                    }
                }
                else if (_b)
                {
                    return -1;
                }
                else if (_up)
                {
                    if (focusedItemIndex > 0)
                    {
                        focusedItemIndex -= 1;
                        displayOptionsMenu(items, numItems, focusedItemIndex);
                    }
                }
                else if (_down)
                {
                    if (focusedItemIndex < numItems - 1)
                    {
                        focusedItemIndex += 1;
                        displayOptionsMenu(items, numItems, focusedItemIndex);
                    }
                }
            }
        }
    }

    /*
     Item0
    >Item1
       Subitem0  Subitem1 >Subitem2  Subitem3
    Item2
    */
    void displayMenuWithOptionsBelow(MenuItem items[], uint8_t numItems, uint8_t selectedItem)
    {
        utility->display.clearDisplay();
        bool drewSelected = false;
        for (int i = 0; i < numItems; i++)
        {
            utility->display.setCursor(FIRST_X_COORD, ((i + drewSelected) * ROW_OFFSET) + FIRST_Y_COORD);
            if (i == selectedItem)
            {
                utility->display.setTextColor(SELECTED_COLOR);
                utility->display.print(">");
                utility->display.print(items[i].itemName);
                if (items[i].numSubitems > 0)
                {
                    utility->display.setCursor(FIRST_X_COORD, ((i + 1) * ROW_OFFSET) + FIRST_Y_COORD);
                    utility->display.print("    ");
                    for (int j = 0; j < items[i].numSubitems; j++)
                    {
                        if (items[i].subitems[j] != "")
                        {
                            if (j > 0)
                            {
                                utility->display.print("  ");
                            }
                            if (items[i].selectedSubitem == j)
                            {
                                utility->display.setTextColor(SELECTED_COLOR);
                                utility->display.print(">");
                            }
                            else
                            {
                                utility->display.setTextColor(UNSELECTED_COLOR);
                                utility->display.print("  ");
                            }
                            utility->display.print(items[i].subitems[j]);
                        }
                    }
                    drewSelected = true;
                }
            }
            else
            {
                utility->display.setTextColor(UNSELECTED_COLOR);
                utility->display.print("  "); // number of spaces is based on current font's width of a space and > character
                utility->display.print(items[i].itemName);
            }
        }
    }

    uint8_t selectItem(MenuItem items[], uint8_t numItems)
    {
        uint8_t focusedItemIndex = 0;
        displayMenuWithOptionsBelow(items, numItems, focusedItemIndex);
        while (true)
        {
            utility->inputs.update();
            bool _up = utility->inputs.UP_P1 || utility->inputs.UP_P2;
            bool _down = utility->inputs.DOWN_P1 || utility->inputs.DOWN_P2;
            bool _a = utility->inputs.A_P1 || utility->inputs.A_P2;
            // bool _b = utility->inputs.B_P1 || utility->inputs.B_P2;
            if (_a)
            {
                items[focusedItemIndex].selectedSubitem = 0;
                return focusedItemIndex;
            }
            if (_up)
            {
                if (focusedItemIndex > 0)
                {
                    focusedItemIndex -= 1;
                    displayMenuWithOptionsBelow(items, numItems, focusedItemIndex);
                }
            }
            else if (_down)
            {
                if (focusedItemIndex < numItems - 1)
                {
                    focusedItemIndex += 1;
                    displayMenuWithOptionsBelow(items, numItems, focusedItemIndex);
                }
            }
        }
    }

    int8_t selectSubitem(MenuItem items[], uint8_t numItems, uint8_t selectedItem)
    {
        int8_t focusedSubitemIndex = items[selectedItem].selectedSubitem;
        displayMenuWithOptionsBelow(items, numItems, selectedItem); // probably not necessary because it should already be displayed

        while (true)
        {
            utility->inputs.update();
            bool _left = utility->inputs.LEFT_P1 || utility->inputs.LEFT_P2;
            bool _right = utility->inputs.RIGHT_P1 || utility->inputs.RIGHT_P2;
            bool _a = utility->inputs.A_P1 || utility->inputs.A_P2;
            bool _b = utility->inputs.B_P1 || utility->inputs.B_P2;
            if (_a)
            {
                items[selectedItem].selectedSubitem = -1;
                return focusedSubitemIndex;
            }
            if (_b)
            {
                items[selectedItem].selectedSubitem = -1;
                return -1;
            }
            if (_left)
            {
                int8_t tempFocusedSubitemIndex = focusedSubitemIndex;
                while (tempFocusedSubitemIndex > 0)
                {
                    tempFocusedSubitemIndex -= 1;
                    if (items[selectedItem].subitems[tempFocusedSubitemIndex] != "")
                    {
                        focusedSubitemIndex = tempFocusedSubitemIndex;
                        items[selectedItem].selectedSubitem = focusedSubitemIndex;
                        displayMenuWithOptionsBelow(items, numItems, selectedItem);
                        break;
                    }
                }
            }
            else if (_right)
            {
                int8_t tempFocusedSubitemIndex = focusedSubitemIndex;
                while (tempFocusedSubitemIndex < items[selectedItem].numSubitems - 1)
                {
                    tempFocusedSubitemIndex += 1;
                    if (items[selectedItem].subitems[tempFocusedSubitemIndex] != "")
                    {
                        focusedSubitemIndex = tempFocusedSubitemIndex;
                        items[selectedItem].selectedSubitem = focusedSubitemIndex;
                        displayMenuWithOptionsBelow(items, numItems, selectedItem);
                        break;
                    }
                }
            }
        }
    }

public:
    Menu(Utility &utility) : utility(&utility) //,
                                               //  display(utility.display) //,
                                               //  MATRIX_WIDTH(utility.MATRIX_WIDTH),
                                               //  MATRIX_HEIGHT(utility.MATRIX_HEIGHT)
    {
    }

    // void setDisplay(PxMATRIX d)
    // {
    //     this->display = d;
    // }

    template <size_t num_items>                                                              // could add this same template to the private functions
    void doMenu(MenuItem (&items)[num_items], int8_t &selectedItem, int8_t &selectedSubitem) // = NULL)
    {
        // if (selectedSubitem != NULL)
        // {
        int8_t tempSelectedItem;
        int8_t tempSelectedSubitem = -1;

        while (tempSelectedSubitem == -1)
        {
            tempSelectedItem = selectItem(items, num_items);
            tempSelectedSubitem = selectSubitem(items, num_items, tempSelectedItem);
        }

        selectedItem = tempSelectedItem;
        selectedSubitem = tempSelectedSubitem;
        // }
        // else
        // {
        // }
    }

    template <size_t num_items>
    bool doOptionsMenu(MenuItem (&items)[num_items], int8_t &changedItem)
    {
        // I was going to just use this function for all menus...I probably could make it return a boolean
        // and if it returns true during the options menu it starts the game, otherwise returns false...
        // Never mind, the whole point of this is to return an option that changed value to main.cpp so that
        // main can control calling the relevant setter functions in the games (leaving this blurb here for future self)

        int8_t tempChangedOption;
        tempChangedOption = setOptions(items, num_items, changedItem);
        if (tempChangedOption == -1) // This means B was pressed and user wants to go to previous menu
        {
            return false;
        }
        else
        {
            changedItem = tempChangedOption;
            return true;
        }
        // ^this needs to receive the focused option. Each option (MenuItem) itself tracks whether it is selected or not
        // and handles changing the value. Only after an option's value is changed and then A or B is pressed should that
        // function return the index of the option that was changed, all the way back to main. main will then call the
        // relevant setter function and give it the value of the MenuItem.

        // When an option is selected:
        //    if it has a value:
        //        allow editing value and check for change
        //        if it changes, return the index
        //    if it is basic:
        //        return the index
        //    could also just have the suboptions in as well? but that needs the other variable which wouldn't be passed
    }
};

#endif

// struct ItemWithSubitems
// {
//     ItemWithSubitems() = default;
//     ItemWithSubitems(const char *name, const char **subs, const uint8_t numSubs, int8_t selectedSub) : itemName(name), subitems(subs), numSubitems(numSubs), selectedSubitem(selectedSub)
//     {
//     }
//     const char *itemName;
//     const char **subitems;
//     uint8_t numSubitems;
//     int8_t selectedSubitem;
// };

// struct ItemWithValue
// {
//     const char *itemName;
//     bool isSelected;
//     uint8_t value;
//     const uint8_t minValue;
//     const uint8_t maxValue;
//     void (*valueSetterFunction)(uint8_t newValue);
// };

// void displaySelectMenu(const char *items[], uint8_t num_items, uint8_t selected)
// {
//     utility->display.clearDisplay();
//     for (int i = 0; i < num_items; i++)
//     {
//         utility->display.setCursor(1, (i * 6) + 5);
//         if ((i + 1) == selected)
//         {
//             utility->display.setTextColor(utility->colors.white);
//             utility->display.print(">");
//         }
//         else
//         {
//             utility->display.setTextColor(utility->colors.cyan);
//             utility->display.print("  ");
//         }
//         utility->display.print(items[i]);
//     }
// }

// uint8_t selectMenuItem(const char *items[], uint8_t num_items)
// {
//     uint8_t selected = 1;
//     displaySelectMenu(items, num_items, selected);

//     while (true)
//     {
//         utility->inputs.update();
//         bool _up = utility->inputs.UP_P1 || utility->inputs.UP_P2;
//         bool _down = utility->inputs.DOWN_P1 || utility->inputs.DOWN_P2;
//         bool _start = utility->inputs.A_P1 || utility->inputs.A_P2;
//         if (_start)
//         {
//             return selected;
//         }
//         if (_up)
//         {
//             if (selected > 1)
//             {
//                 selected -= 1;
//                 displaySelectMenu(items, num_items, selected);
//             }
//         }
//         else if (_down)
//         {
//             if (selected < num_items)
//             {
//                 selected += 1;
//                 displaySelectMenu(items, num_items, selected);
//             }
//         }
//     }
// }

// void chooseGame()
// {
//     // selected_game = selectMenuItem(game_strings, NUMBER_OF_GAMES);
// }

// void setUpMenu(BaseGame **games, uint8_t numGames)
// {
//     // pointers need to point to already constructed game objects
//     for (int i = 0; i < numGames; i++)
//     {
//         games[i]->loopGame();
//     }
// }

// typedef std::map<const char *, const char **> OptionWithSuboptions;

//     void setupStuff()
//     {
//         std::map<const char *, const char **> myMap;
//         myMap.insert(std::make_pair("game", player_strings));
//         const char **wordd = myMap["game"];
//         utility->display.print(wordd[2]);

//         OptionWithSuboptions testoptions;
//         testoptions.insert(std::make_pair("blah", player_strings));
//         const char **blahh = testoptions["blah"];
//         utility->display.print(blahh[2]);
//     }

//     void displayMenuWithOptionsBelowMap(OptionWithSuboptions options, uint8_t numSuboptions, uint8_t selected)
//     {
//         utility->display.clearDisplay();
//         bool drewSelected = false;
//         int count = 0;
//         for (const auto &[key, value] : options)
//         {
//             utility->display.setCursor(FIRST_X_COORD, ((count + drewSelected) * ROW_OFFSET) + FIRST_Y_COORD);
//             if ((count + 1) == selected)
//             {
//                 utility->display.setTextColor(SELECTED_COLOR);
//                 utility->display.print(">");
//                 utility->display.print(key);
//                 utility->display.setCursor(FIRST_X_COORD, ((count + 1) * ROW_OFFSET) + FIRST_Y_COORD);
//                 utility->display.print("      ");
//                 for (int i = 0; i < numSuboptions; i++)
//                 {
//                     if (value[i] != "") // would need to track which suboption is selected as well?
//                     {
//                         utility->display.print(value[i]);
//                         utility->display.print(" ");
//                     }
//                 }
//                 drewSelected = true;
//             }
//             else
//             {
//                 utility->display.setTextColor(UNSELECTED_COLOR);
//                 utility->display.print("  "); // number of spaces is based on current font's width of a space and > character
//             }
//             count++;
//         }
//     }

// template <size_t rows, size_t cols>
// void setUpMainMenuWithSubitems(uint8_t numOptions, const char *options[rows], bool (&suboptions)[rows][cols], uint8_t maxSuboptions, const char *suboptionNames[cols])
// {
//     Serial.println("begin setupmainmenu");
//     Serial.printf("Address is %p\n", (void *)suboptions);
//     for (int i = 0; i < numOptions; i++)
//     {
//         Serial.println("begin i for loop");
//         uint8_t initialSelectedSuboption = -1;
//         bool setInitial = false;
//         const char *subs[cols];
//         for (int j = 0; j < maxSuboptions; j++)
//         {
//             Serial.println("begin j for loop");
//             Serial.print("i: ");
//             Serial.print(i);
//             Serial.print(" j: ");
//             Serial.print(j);
//             Serial.print(" suboption: ");
//             Serial.println(suboptions[i][j]);
//             if (suboptions[i][j])
//             {
//                 Serial.println("found a suboption");
//                 if (!setInitial)
//                 {
//                     initialSelectedSuboption = j;
//                     setInitial = true;
//                 }
//                 subs[j] = suboptionNames[j];
//             }
//             else
//             {
//                 subs[j] = "";
//             }
//         }
//         Serial.println("Before adding an item to mainMenu object");
//         if (initialSelectedSuboption == -1)
//         {
//             Serial.println("no suboptions");
//             // ItemWithSubitems item(options[i], suboptions[i], 0, -1);
//             ItemWithSubitems item(options[i], subs, 0, -1);
//             Serial.println("After creating item");
//             mainMenu[i] = &item;
//             Serial.println("After adding an item to mainMenu object");
//         }
//         else
//         {
//             Serial.println("has suboptions");
//             // ItemWithSubitems item(options[i], suboptions[i], maxSuboptions, initialSelectedSuboption);
//             ItemWithSubitems item(options[i], subs, maxSuboptions, initialSelectedSuboption);
//             Serial.println("After creating item");
//             mainMenu[i] = &item;
//             Serial.println("After adding an item to mainMenu object");
//         }
//     }
// }

// void setUpMainMenuWithSubitems(uint8_t numOptions, const char *options[], const char **suboptions[], uint8_t maxSuboptions)
// {
//     Serial.println("begin setupmainmenu");
//     Serial.printf("Address is %p\n", (void *)suboptions);
//     for (int i = 0; i < numOptions; i++)
//     {
//         Serial.println("begin i for loop");
//         uint8_t initialSelectedSuboption = -1;
//         for (int j = 0; j < maxSuboptions; j++)
//         {
//             Serial.println("begin j for loop");
//             Serial.print("i: ");
//             Serial.print(i);
//             Serial.print(" j: ");
//             Serial.print(j);
//             Serial.print(" suboption: ");
//             Serial.println(suboptions[i][j]);
//             if (suboptions[i][j] != "")
//             {
//                 Serial.println("found a suboption");
//                 initialSelectedSuboption = j;
//                 break;
//             }
//         }
//         Serial.println("Before adding an item to mainMenu object");
//         if (initialSelectedSuboption = -1)
//         {
//             Serial.println("no suboptions");
//             ItemWithSubitems item(options[i], suboptions[i], 0, -1);
//             Serial.println("After creating item");
//             mainMenu[i] = &item;
//             Serial.println("After adding an item to mainMenu object");
//         }
//         else
//         {
//             Serial.println("has suboptions");
//             ItemWithSubitems item(options[i], suboptions[i], maxSuboptions, initialSelectedSuboption);
//             Serial.println("After creating item");
//             mainMenu[i] = &item;
//             Serial.println("After adding an item to mainMenu object");
//         }
//     }
// }

// void doMainMenu(uint8_t numOptions, uint8_t &selectedItem, uint8_t &selectedSubitem)
// {
//     selectItem(mainMenu, numOptions);
// }