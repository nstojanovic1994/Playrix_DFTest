// DFAdventure.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "Windows.h"
#include "Public/View/MainMenu.h"

//Remove typing underline cursor from the console
void DisableCursor()
{
    HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO     cursorInfo;
    GetConsoleCursorInfo(out, &cursorInfo);
    cursorInfo.bVisible = false; // set the cursor visibility
    SetConsoleCursorInfo(out, &cursorInfo);
}


int main()
{
    //Disable typing cursor
    DisableCursor();
    //Create new MainMenu class and begin play
    std::unique_ptr<IMainMenu> MainMenu(new IMainMenu);
    MainMenu->BeginPlay();

}
