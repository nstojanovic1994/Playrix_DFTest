#include "..\..\..\Public\Characters\Base\MenuController.h"
#include "..\..\..\Public\View\MainMenu.h"
#include "..\..\..\Public\View\ConsoleRender.h"
#include <conio.h>

//Receive input from main menu while process
void AMenuController::ReceiveInput()
{
	//Catches input without enter
	char key = _getch();
	//If special keys are pressed (such as arrow keys) ignore first special character input and capture the second real input code
	if (key < 0 || key == 224 || key > 255)
	{
		key = _getch();
	}
	//If controller is in typing mode for save/load
	if (Typing)
	{
		if (key == 8)
		{
			if (!fileName.empty())
			{
				cout << '\b';
				cout << " ";
				fileName.pop_back();
			}
		}
		//Use key (space)
		else if (isspace(key))
		{
			//Try to load the game with the given name
			Typing = false;
			if (!GetMainMenu().LoadGame(LoadType + fileName + ".json"))
			{
				//Print load error and return to typing
				GetConsoleRender().LoadError();
				Typing = true;
				GetMainMenu().InMenu = true;
			}
			//Clear input name for retyping
			fileName = "";
		}
		//Escape key - go back a menu
		else if (key == 27)
		{
			//Clear input name in case user returns to load screen
			fileName = "";
			//Clear typing mode
			Typing = false;
			//Draw main menu and set the cursor to the first location
			GetConsoleRender().DrawMainMenu(false);
			GetConsoleRender().MoveMMCursor(1, 0);
			MenuY = 1;
		}
		else
		{
			//If typing letters draw them and save to the fileName
			cout << char(key);
			fileName += char(key);
		}
	}
	//Navigation mode
	else
	{
		//Up navigation
		if (key == 72 or key == 'w')
		{
			if (MenuY > 1)
			{
				GetConsoleRender().MoveMMCursor(MenuY, -1);
				MenuY--;
			}
		}
		//Down navigation
		if (key == 80 or key == 's')
		{
			if (MenuY < 4)
			{
				GetConsoleRender().MoveMMCursor(MenuY, 1);
				MenuY++;
			}
		}
		//Use key (space)
		if (isspace(key))
		{
			switch (MenuY)
			{
			case 1:
				//If top option is chosen start a new game with default map
				GetMainMenu().NewGame();
				break;
			case 2:
				GetConsoleRender().MainMenuTypeUI("Load Map");
				Typing = true;
				LoadType = "Maps/";
				break;
			case 3:
				//If load options is chosen opet load input screen
				GetConsoleRender().MainMenuTypeUI("Load Game");
				Typing = true;
				LoadType = "Saves/";
				break;
			case 4:
				//Quit the game by interupting while process in the main menu
				GetMainMenu().InMenu = false;
				break;
			}
		}
	}
}

//Set reference to the main menu
void AMenuController::SetMainMenu(IMainMenu& NewMM)
{
	MainMenu = &NewMM;
}

//Get reference to the main menu
IMainMenu& AMenuController::GetMainMenu()
{
	return *MainMenu;
}

//Set reference to the console render
void AMenuController::SetConsoleRender(IConsoleRender& NewRender)
{
	ConsoleRender = &NewRender;
}

//Get reference to the console render
IConsoleRender& AMenuController::GetConsoleRender()
{
	return *ConsoleRender;
}
