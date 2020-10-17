#pragma once
#include "Pawn.h"
#include <stdio.h>
#include <iostream>
using namespace std;

//Enum class for the current state in which controller operates
enum class MenuOpened : int
{
	Game = 0,
	Inventory = 1,
	Character = 2,
	MainMenu = 3
};

//Base controller class for receiving input to manipulate pawn
class AController
{
public:
	//Get currently controlled pawn
	APawn& GetPawn() const;
	//Set new controlled pawn
	virtual void SetPawn(APawn& NewPawn);
	//Input update from the game
	virtual void ReceiveInput() {};

	//Set current controller state
	void SetCurrentState(MenuOpened NewState);
	//Get current controller state
	MenuOpened GetCurrentState();

	//Main Menu vars
	//Switcher between saving and loading game/map
	bool Saving = false;
	string LoadType = "Saves/";
	//Switcher between navigating menu and typing save/load game name
	bool Typing = false;
	//Save input from typing the name
	string fileName = "";

private:
	//Pawn that will receive the input from this controller
	APawn* ControlledPawn;
	//Current controller state
	MenuOpened CurrentState = MenuOpened::Game;
};

