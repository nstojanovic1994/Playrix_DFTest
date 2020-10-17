#include "../GameMode.h"
#include "../Characters/Base/MenuController.h"

//Main class of the game for starting and restarting in game process
class IMainMenu
{
public:
	//Begin inital game process
	void BeginPlay();
	//Load default map
	void NewGame();
	//Load game process - returns false if file is not found
	bool LoadGame(string Name);
	//Check if player is still in inital main menu or has started the game
	bool InMenu = false;

private:
	//Initiate console render class
	void InitRender();
	//Unique pointer for console class
	unique_ptr<IConsoleRender> ConsoleRender;
	//Unique pointer for current GM - Easier to load the game by restarting game mode process
	unique_ptr<AGameMode> CurrentGM;
	//Unique pointer for main menu controller - Deleted after the game has started
	unique_ptr<AMenuController> MenuPC;
};