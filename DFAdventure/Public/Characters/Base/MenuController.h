#pragma once
#include <stdio.h>
#include <iostream>
#include <string>

class IMainMenu;
class IConsoleRender;

//Controller class for navigating through the start menu of the game
class AMenuController
{
public:
	//Receive input from the game
	void ReceiveInput();

	//Set main menu reference
	void SetMainMenu(IMainMenu& NewMM);
	//Get current main menu reference
	IMainMenu& GetMainMenu();

	//Set console render reference
	void SetConsoleRender(IConsoleRender& NewRender);
	//Get console render reference
	IConsoleRender& GetConsoleRender();
	//Switcher between navigating the menu and typing load game name
	bool Typing = false;
	//Switcher between loading game/map
	std::string LoadType = "Saves/";

private:
	//Save input from typing the name
	std::string fileName = "";
	//Current position of the cursor while navigating
	int MenuY = 1;
	//Main Menu Process
	IMainMenu* MainMenu;
	//Console render
	IConsoleRender* ConsoleRender;
};

