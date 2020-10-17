#pragma once
#include ".\Characters\Base\Controller.h"
#include ".\View\ConsoleRender.h"
#include ".\Characters\Base\AbstractCharacterFactory.h"
#include ".\View\DataStruct.h"
#include <json.h>

class IMainMenu;

//Class for controller/pawn communication and in game process
class AGameMode
{
public:
	//Start game process
	void BeginPlay();
	//Show victory or game over screen if player died
	void EndGame(bool Victory);
	//End turn process to loop through characters in order for them to perform turn one by one - if ByPlayer is true GameMode should start looping through enemy charcaters one by one
	void EndTurnProcess(bool ByPlayer);

	//Return the position of player pawn needed for enemy calculations
	Vec2D GetPlayerPos() const;

	//Quick save - Saves only characters and items without the map
	void QuickSave();
	//Quick load - Reverts present characters and items to their saved state and spawns any missing ones - without affecting map
	void QuickLoad();
	//Fully save the game with the map
	void SaveGame(string Name);
	//Load the map with all the characters and items
	void LoadGame(string Name);

	//Returns player controller for easier access to player pawn
	AController& GetPlayerController() const;

	//Calculate the damage between two fighters and apply damage to them
	void PerformAttack(APawn* Attacker, APawn* Target);
	//Returns the enemy pawn at given location for use as a target in the attack
	APawn* GetEnemy(int X, int Y) const;
	//Process the death of enemy by removing it from the map and enemy array
	void EnemyDeath(APawn& Enemy);
	//Function for dropping an item after enemy death based on the stats of that enemy
	void EnemyDrop(CharacterData Enemy, Vec2D Position);
	//Add an item to players inventory if the player walked over it
	void AddToPlayer(Vec2D Position, int X, int Y);
	//Update UI about player and nearby enemies once per turn
	void EndTurnUI();
	//Recive console render from main menu
	void SetConsoleRender(IConsoleRender& NewRender);
	//Returns the private pointer to console render
	IConsoleRender& GetConsoleRender();
	//Set reference to MainMenu
	void SetMainMenu(IMainMenu& NewMM);
	//Returns the private pointer to main menu
	IMainMenu& GetMainMenu();

	//Keeps while loop for the in game process going in order for proper controller to receive input
	bool InGame = true;

	//Get end game state
	bool GetEndGameState();

private:
	//Load all necessary data from XML/JSON
	bool LoadData(bool Map, string filePath);
	//Initiate map and UI
	void InitRender(MapData Map);
	//Spawn all characters
	void SpawnCharacters(SpawnData Spawns);
	//Spawn all items
	void SpawnItems(SpawnData Spawns);
	//Game process
	void GameFlow();

	//Holder for end game state
	bool EndGameState = false;

	//Spawn a character using factory
	AController& CreateCharacter(UAbstractCharacterFactory& Factory, IConsoleRender* ConsoleRender);

	//List of all enemy controllers
	vector<unique_ptr<AController>> EnemyControllers;
	//Player controller
	unique_ptr<AController> PlayerController;
	//Console render
	IConsoleRender* ConsoleRender;
	//Main Menu Process
	IMainMenu* MainMenu;

	//List of all items
	vector<ItemData> SpawnedItems;

	//Item id counter in order to assign unique id to each item
	int ItemIDCounter = 0;
	//Enemy id counter in order to assign unique id to each enemy
	int EnemyIDCounter = 0;
	//Path to the last map/save loaded
	string lastData;

	//Map creation data loaded from json
	MapData LoadedMap;
	//Characters and item spawning data loaded from json
	SpawnData LoadedCharacters;

	//Save current state of the game to the new json defined by filePath - Map is false when used for quick save
	void WriteNewJSON(bool Map, string filePath);

	//End turn
	//Iterator for counting the number of enemies that ended their turn - So enemies would play turns one by one
	int TurnIterator = 0;

	//Holder for how many enemy UI slots have been drawn for their deletion before drawing new
	int NumberOfCharacterSlots = 0; 

	//Quick sort eight closest enemies to the player and present them in UI by distance starting from top of the left column
	int partition(vector<reference_wrapper<AController>> &InArray, Vec2D PlayerPos, int low, int high);
	void QuickSort(vector<reference_wrapper<AController>> &InArray, Vec2D PlayerPos, int low, int high);
};


