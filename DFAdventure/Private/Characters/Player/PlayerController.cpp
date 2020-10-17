#include "..\..\..\Public\Characters\Player\PlayerController.h"
#include "..\..\..\Public\GameMode.h"
#include "..\..\..\Public\View\MainMenu.h"
#include <conio.h>

void APlayerController::ReceiveInput()
{
	//Catches input without enter
	char key = _getch();
	//If special keys are pressed (such as arrow keys) ignore first special character input and capture the second real input code
	if (key < 0 || key == 224 || key > 255)
	{
		key = _getch();
	}
	//If current state is in game
	if (GetCurrentState() == MenuOpened::Game)
	{
		//If player pawn is in attack mode perform attack in the given direction
		if (GetPawn().AttackMode)
		{
			//Attack upwards
			if (key == 72 or key == 'w')
				GetPawn().Attack(0, -1);
			//Attack downwards
			if (key == 80 or key == 's')
				GetPawn().Attack(0, 1);
			//Attack right
			if (key == 77 or key == 'd')
				GetPawn().Attack(1, 0);
			//Attack left
			if (key == 75 or key == 'a')
				GetPawn().Attack(-1, 0);

		}
		//If player pawn is in normal (movement mode) receive input for moving
		else
		{
			//Move Up and end turn if attacking is not possible
			if (key == 72 or key == 'w')
				if (GetPawn().Move(MovementDirection::UP))
				{
					if (!GetPawn().AttackMode)
					{
						GetPawn().EndTurn();
					}
				}
			//Move Down and end turn if attacking is not possible
			if (key == 80 or key == 's')
				if (GetPawn().Move(MovementDirection::DOWN))
				{
					if (!GetPawn().AttackMode)
					{
						GetPawn().EndTurn();
					}
				}
			//Move Right and end turn if attacking is not possible
			if (key == 77 or key == 'd')
				if (GetPawn().Move(MovementDirection::RIGHT))
				{
					if (!GetPawn().AttackMode)
					{
						GetPawn().EndTurn();
					}
				}
			//Move Left and end turn if attacking is not possible
			if (key == 75 or key == 'a')
				if (GetPawn().Move(MovementDirection::LEFT))
				{
					if (!GetPawn().AttackMode)
					{
						GetPawn().EndTurn();
					}
				}
		}
		//Use key (space)
		if (isspace(key))
		{
			//If player pawn can attack and is not currently in attack mode turn off fortify and enter attack mode
			if (GetPawn().CanAttack() && !GetPawn().AttackMode)
			{
				GetPawn().SetFortify(false);
				GetPawn().SetAttackMode(true);
			}
			//If pawn is already in attack mode turn the attack mode off
			else if (GetPawn().AttackMode)
			{
				GetPawn().SetAttackMode(false);
			}
		}
		//Switch to fortify stance for better defense and healing
		if (key == 'f')
		{
			GetPawn().SetFortify(!GetPawn().GetFortify());
			if (GetPawn().GetFortify())
			{
				GetPawn().EndTurn();
			}
		}
		//End turn manually in case player does not want to do anything
		if (key == 'x')
		{
			GetPawn().EndTurn();
		}
		//Open inventory/character screen
		if (key == 'i')
		{
			GetPawn().GetConsoleRender().DrawCharacterStats();
			GetPawn().GetConsoleRender().UpdateCharacterStats(GetPawn().GetPawnData());
			GetPawn().GetConsoleRender().DrawInventory(GetPawn().GetPawnData());
			SetCurrentState(MenuOpened::Inventory);
		}
		//Open in game main menu
		if (key == 27)
		{
			GetPawn().GetConsoleRender().DrawMainMenu(true);
			GetPawn().GetConsoleRender().MoveMMCursor(1, 0);
			SetCurrentState(MenuOpened::MainMenu);
			//Reset cursor to 0,0 to show the whole screen
			GetPawn().GetConsoleRender().MoveCursor(0, 0);
		}
	}
	//If navigating the inventory menu
	else if (GetCurrentState() == MenuOpened::Inventory)
	{
		//Navigate up through the grid
		if (key == 72 or key == 'w')
		{
			if (MenuY > 1)
			{
				GetPawn().GetConsoleRender().MoveInventoryCursor(MenuX, MenuY, 0, -1, GetPawn().GetPawnData());
				MenuY--;
			}
		}
		//Navigate down through the grid
		if (key == 80 or key == 's')
		{
			if (MenuY < 4)
			{
				GetPawn().GetConsoleRender().MoveInventoryCursor(MenuX, MenuY, 0, 1, GetPawn().GetPawnData());
				MenuY++;
			}
		}
		//Navigate right through the grid
		if (key == 77 or key == 'd')
		{
			if (MenuX < 4)
			{
				GetPawn().GetConsoleRender().MoveInventoryCursor(MenuX, MenuY, 1, 0, GetPawn().GetPawnData());
				MenuX++;
			}
		}
		//Navigate left through the grid
		if (key == 75 or key == 'a')
		{
			if (MenuX > 1)
			{
				GetPawn().GetConsoleRender().MoveInventoryCursor(MenuX, MenuY, -1, 0, GetPawn().GetPawnData());
				MenuX--;
			}
		}
		//Use key (space) - If possibel equip the hovered item
		if (isspace(key))
		{
			GetPawn().SwitchItems(MenuX, MenuY);
			GetPawn().GetConsoleRender().UpdateCharacterStats(GetPawn().GetPawnData());
			GetPawn().GetConsoleRender().MoveInventoryCursor(MenuX, MenuY, 0, 0, GetPawn().GetPawnData());

		}
		//Switch to character screen navigation
		if (key == 'i')
		{
			GetPawn().GetConsoleRender().SwitchInvStat(MenuX, MenuY, true);
			MenuX = MenuY = 1;
			SetCurrentState(MenuOpened::Character);
		}
		//Escape - return to game
		if (key == 27)
		{
			GetPawn().GetConsoleRender().DrawMapFromArray();
			GetPawn().GetGameMode().EndTurnUI();
			SetCurrentState(MenuOpened::Game);
			MenuX = MenuY = 1;
			GetPawn().GetConsoleRender().MoveCursor(0, 0);
		}
		//Drop (destroy) hovered item to free space
		if (key == 'q')
		{
			GetPawn().DestroyItem(MenuX, MenuY);
			GetPawn().GetConsoleRender().MoveInventoryCursor(MenuX, MenuY, 0, 0, GetPawn().GetPawnData());
		}
	}
	//Navigatin the character screen
	else if (GetCurrentState() == MenuOpened::Character)
	{
		//Switch to inventory screen
		if (key == 'i')
		{
			GetPawn().GetConsoleRender().SwitchInvStat(MenuX, MenuY, false);
			GetPawn().GetConsoleRender().MoveInventoryCursor(1, 1, 0, 0, GetPawn().GetPawnData());
			MenuX = MenuY = 1;
			SetCurrentState(MenuOpened::Inventory);
		}
		//Navigate up through the stats
		if (key == 72 or key == 'w')
		{
			if (MenuY > 1)
			{
				GetPawn().GetConsoleRender().MoveCharacterCursor(MenuY, -1);
				MenuY--;
			}
		}
		//Navigate down through the stats
		if (key == 80 or key == 's')
		{
			if (MenuY < 3)
			{
				GetPawn().GetConsoleRender().MoveCharacterCursor(MenuY, 1);
				MenuY++;
			}
		}
		//Use key (space) - assign skill point if any are available to the currently hovered stat
		if (isspace(key))
		{
			GetPawn().AssignSkillPoint(MenuY);
			GetPawn().GetConsoleRender().UpdateCharacterStats(GetPawn().GetPawnData());
		}
		//Escape to return to the game
		if (key == 27)
		{
			GetPawn().GetConsoleRender().DrawMapFromArray();
			GetPawn().GetGameMode().EndTurnUI();
			SetCurrentState(MenuOpened::Game);
			MenuX = MenuY = 1;
			//Reset cursor to 0,0 to show the whole screen
			GetPawn().GetConsoleRender().MoveCursor(0, 0);
		}
	}
	//If navigating in game main menu
	else if (GetCurrentState() == MenuOpened::MainMenu)
	{
		//If controller is in typing mode for save/load
		if (Typing)
		{
			//Use key (space)
			if (isspace(key))
			{
				//Saving - switch between input for loading and saving screen
				Typing = false;
				if (Saving)
				{
					//Save the game with the given name
					GetPawn().GetGameMode().SaveGame("Saves/" + fileName + ".json");
					GetPawn().GetConsoleRender().DrawMainMenu(true);
					GetPawn().GetConsoleRender().MoveMMCursor(1, 0);
					MenuY = 0;
				}
				else
				{

					//Try to load the game with the given name
					if (!GetPawn().GetGameMode().GetMainMenu().LoadGame(LoadType + fileName + ".json"));
					{
						//Print load error and return to typing
						GetPawn().GetConsoleRender().LoadError();
						Typing = true;
					}
	
				}
				//Clear input name for retyping
				fileName = "";
			}
			//Escape to return the in game main menu - clear typing and fileName if user returns to save/load screen
			else if (key == 27)
			{
				fileName = "";
				Typing = false;
				GetPawn().GetConsoleRender().DrawMainMenu(true);
				GetPawn().GetConsoleRender().MoveMMCursor(1, 0);
				MenuX = MenuY = 1;
			}
			//If typing letters draw them and save to the fileName
			else
			{
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
					GetPawn().GetConsoleRender().MoveMMCursor(MenuY, -1);
					MenuY--;
				}
			}
			//Down navigation
			if (key == 80 or key == 's')
			{
				//Switch between death/victory and in game menu screen - User cannot save while on death/victory screen
				if (!GetPawn().GetGameMode().GetEndGameState())
				{
					if (MenuY < 5)
					{
						GetPawn().GetConsoleRender().MoveMMCursor(MenuY, 1);
						MenuY++;
					}
				}
				else
				{
					if (MenuY < 4)
					{
						GetPawn().GetConsoleRender().MoveMMCursor(MenuY, 1);
						MenuY++;
					}
				}
			}
			//Use key (space) - choose an option
			if (isspace(key))
			{
				switch (MenuY)
				{
				case 1:
					//If top option is chosen start a new game with default map
					GetPawn().GetGameMode().GetMainMenu().NewGame();
					break;
				case 2:
					//If load options is chosen opet load input screen
					GetPawn().GetConsoleRender().MainMenuTypeUI("Load Map");
					Typing = true;
					LoadType = "Maps/";
					Saving = false;
					break;
				case 3:
					//If load options is chosen opet load input screen
					GetPawn().GetConsoleRender().MainMenuTypeUI("Load Game");
					Typing = true;
					LoadType = "Saves/";
					Saving = false;
					break;
				case 4:
					//If save options is chosen open save input screen
					if (GetPawn().GetPawnData().CurrentHealth > 0)
					{
						GetPawn().GetConsoleRender().MainMenuTypeUI("Save Game");
						Typing = true;
						Saving = true;
					}
					else
					{
						GetPawn().GetGameMode().InGame = false;
					}
					break;
				case 5:
					//Quit the game by interupting while process in the game mode process
					GetPawn().GetGameMode().InGame = false;
					break;
				}
			}
			//Escape key - return to the game if not on death screen
			if (key == 27)
			{
				if (!GetPawn().GetGameMode().GetEndGameState())
				{
					GetPawn().GetConsoleRender().DrawMapFromArray();
					GetPawn().GetGameMode().EndTurnUI();
					SetCurrentState(MenuOpened::Game);
					//Reset cursor to 0,0 to show the whole screen
					GetPawn().GetConsoleRender().MoveCursor(0, 0);
				}
			}
		}
	}
	//Quick save while alive
	if (key == 'k')
	{
		if (!GetPawn().GetGameMode().GetEndGameState())
		{
			GetPawn().GetGameMode().QuickSave();
		}
	}
	//Quick load
	if (key == 'l')
	{
		GetPawn().GetGameMode().QuickLoad();
	}
}
