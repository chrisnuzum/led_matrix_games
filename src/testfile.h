#ifndef _mytestfile_
#define _mytestfile_

// #include <Arduino.h>
// #include <PxMatrix.h>
// #include <Utility.h>
// #include <map>
// #include <functional>
// #include <string>

// typedef std::map<std::string, std::string> TMap;

// TMap m;
// m["k2"] = "v2";
// m["k3"] = "v3";
// m["k1"] = "v1";

#include <map>
#include <iostream>

void testtesttest() {};

void do_map_test78()
{
    std::map<std::string, int> m;

    m.insert({"a", 1});
    
    m["b"] = 19;

    m.insert(std::make_pair("a", 1));
    m.insert(std::pair<std::string, int>("b", 2));

    std::map<std::string, int>::iterator it;

    for (it = m.begin(); it != m.end(); it++)
    {
        std::cout << it->first.c_str();
        std::cout << ": ";
        std::cout << it->second;
        std::cout << "\n";
    }

    for (std::pair<std::string, int> element : m)
    {
        std::cout << element.first.c_str();
        std::cout << ": ";
        std::cout << element.second;
        std::cout << "\n";
    }
    
    
    
    std::map<const char *, void (*)()> testMap;
    int blah = 8;
    
    testMap.insert({"hello", &testtesttest});
}

// template <size_t rows, size_t cols>
// void process_2d_array_template(int (&array)[rows][cols])
// {
//     std::cout << __func__ << std::endl;
//     for (size_t i = 0; i < rows; ++i)
//     {
//         std::cout << i << ": ";
//         for (size_t j = 0; j < cols; ++j)
//             std::cout << array[i][j] << '\t';
//         std::cout << std::endl;
//     }
// }

// void te72837()
// {
//     int myarray[4][5];
//     process_2d_array_template(myarray);
// }

#endif

/*
// typedef void (*Menu_Processing_Function_Pointer)(void);

// struct Menu_Option
// {
//     char choice;
//     char const *p_selection_text;
//     // void (*p_processing_function) (void);
//     std::function<void(void)> func;
// };

// void Process_Selection_One();
// void Process_Selection_Two()
// {
//     uint8_t number_test = 5;
// };

// Utility utility_test;

// std::map<std::string, void (*)()> test_items5;

// void tester_f()
// {
//     Menu_Option main_menu[] =
//         {
//             {'1', "Option 1", Process_Selection_One},
//             {'2', "Option 2", Process_Selection_Two},
//             {'3', "Option 3", [this]()
//              { this->utility_test.inputs.update(); }}};
// };

// // test_items["hello"] = &tester_f;
*/

/*
    // typedef void (*Menu_Processing_Function_Pointer)(void);

    void exit_me();
    void help();

    void blahblahtest()
    {
        std::map<std::string, int> m;

        m.insert({"a", 1});
        m["b"] = 10;

        m.insert(std::make_pair("a", 1));
        m.insert(std::pair<std::string, int>("b", 2));

        std::map<std::string, void (Menu::*)()> menu7;
        menu7.insert(std::make_pair("exit", &exit_me));
        menu7["exit"] = &exit_me;
        menu7["help"] = &help;
    }

    // typedef std::map<std::string, std::string> TMap;

    // TMap m;
    // m["k2"] = "v2";
    // m["k3"] = "v3";
    // m["k1"] = "v1";

    // std::map<std::string, std::function<void()>> menu7;
    // menu7["exit"] = &exit_me;
    // menu7["help"] = &help;

    struct Menu_Option
    {
        Menu_Option(std::__cxx11::string name, std::function<void()> func) : name(name),
                                                                             func(func)
        {
        }
        std::__cxx11::string name;
        // void (*p_processing_function) (void);
        std::function<void()> func;
    };

    void funcA();
    void Process_Selection_Two()
    {
        uint8_t number_test = 5;
    };

    // Utility utility_test;

    // std::map<std::string, void (*)()> test_items5;
    // int test_function()
    // {
    //     Menu_Option opt1{std::string("Game1"), funcA};

    //     return 0;
    // }

    // Menu_Option main_menu[] =
    //     {
    //         {"Option 1", Process_Selection_One},
    //         {"Option 2", Process_Selection_Two},
    //         {"Option 3", [this]()
    //          { this->utility_test.inputs.update(); }}};

    // std::map<const char[], void (*)()> test_items;
    // std::map<int, int> items2;

    // void tester_f()
    // {
    // }

    // items2[2] = 3;
    // test_items5["hello"] = &tester_f;

*/