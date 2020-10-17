#include "..\..\Public\View\MainMenu.h"
#include "..\..\Public\Characters\Base\MenuController.h"
#include <Windows.h>
#include <fstream>
#include <iostream>

void IMainMenu::BeginPlay()
{
	//Setup console renderer
	InitRender();
	ConsoleRender->DrawMainMenu(false);
	ConsoleRender->MoveMMCursor(1, 0);

	//Add menu controller
	unique_ptr<AMenuController> NewMenuPC(new AMenuController());
	MenuPC = move(NewMenuPC);
	MenuPC->SetConsoleRender(*ConsoleRender);
	MenuPC->SetMainMenu(*this);

	//Start while loop process for sending input to the controller
	InMenu = true;
	while (InMenu)
	{
		MenuPC->ReceiveInput();
	}
}

void IMainMenu::NewGame()
{	
	//LoadGame("maps/defaultmap.json");
	LoadGame("maps/level1.json");
}

bool IMainMenu::LoadGame(string Name)
{
	//Check if load game file exists in order to return load error to UI
	struct stat buffer;
	bool exists = (stat((Name).c_str(), &buffer) == 0);
	if (!exists) return false;

	//Interupt while process in the main menu'
	InMenu = false;

	//Delete old game mode if present
	if (CurrentGM)
	{
		delete CurrentGM.get();
		CurrentGM.release();
	}
	//Remove main menu controller
	if (MenuPC)
	{
		InMenu = false;
		delete MenuPC.get();
		MenuPC.release();
	}
	//Create unique pointer to the game mode and pass it to global class variable
	unique_ptr<AGameMode> NewGM(new AGameMode());
	CurrentGM = move(NewGM);

	//Setup console and main menu reference in GM
	CurrentGM->SetConsoleRender(*ConsoleRender);
	CurrentGM->SetMainMenu(*this);
	//Load game and start game process
	CurrentGM->LoadGame(Name);
	return true;
}

void IMainMenu::InitRender()
{
	//Create unique pointer to the console render and pass it to global class variable
	unique_ptr<IConsoleRender> NewCR(new IConsoleRender());
	ConsoleRender = move(NewCR);
}
