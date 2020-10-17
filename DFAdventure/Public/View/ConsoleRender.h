#pragma once
#include "DataStruct.h"

using namespace std;


class IConsoleRender
{

public:
	//Start Console Render process
	void BeginPlay(MapData LoadedMap);

	//Update map with the given character and position to perform pawn step
	bool UpdateMap(int X, int Y, int MoveX, int MoveY, char UpdateChar);

	//Check if tile is obstructed
	bool CheckTile(int X, int Y);

	//Check if enemy is present on tile
	bool CheckForCharacter(int X, int Y, char CharToCheck);

	//Refresh map from array for switching between UI
	void DrawMapFromArray();

	//Clear characters from current array
	void ClearCharactersFromArray();

	//Draw pawn info 
	void DrawCharacterUI(int StartX, int StartY, CharacterData CharacterInfo, bool Enemy, string Direction);
	//Clear drawn pawn info
	void ClearCharacterUI(int StartX, int StartY);

	//Clear character aftet death
	void ClearTile(int X, int Y);

	//Spawn item at location
	void SpawnItem(int X, int Y);

	//Display player attack mode
	void ChangePawnColor(Vec2D Position, int Color, char PawnChar);

	//Character & Inventory Screen
	//Draw Inventory screen and items from player inventory
	void DrawInventory(CharacterData PawnData);
	//Move UI cursor through inventory items
	void MoveInventoryCursor(int X, int Y, int MoveX, int MoveY, CharacterData PawnData);
	//Draw the information for the currently hovered item to compare it with eqiped one
	void DrawItemInfo(ItemData Item, bool Equiped);
	//Switch inventory and equiped item in the UI
	void SwitchedItems(int X, int Y, CharacterData PawnData);
	//Clear item info if none is hovered or equiped
	void ClearItemInfo();
	//Draw character stats and information screen
	void DrawCharacterStats();
	//Update drawn character screen
	void UpdateCharacterStats(CharacterData Data);
	//Switch navigation between inventory and character screen
	void SwitchInvStat(int X, int Y, bool Inv);
	//Move navigation cursor through character screen
	void MoveCharacterCursor(int Y, int MoveY);

	//Main menu - InGame determines if Save Game option will be presented
	void DrawMainMenu(bool InGame);
	//Move cursor through main menu UI
	void MoveMMCursor(int Y, int MoveY);
	//Show Game Over message above Main Menu UI
	void EndGameUI(bool Victory);
	//Show main menu screen for loading game/map or saving the game
	void MainMenuTypeUI(string Type);
	//Show error above load game UI if save game is not found with input name
	void LoadError();

	//Returns color for drawing based on item rarity
	int GetRarityColor(ItemData Item);

	//Move console cursor in order to draw
	void MoveCursor(int X, int Y);

private:
	//Load map data into an array
	void LoadMap(MapData LoadedMap);
	//Draw legend and controls
	void DrawLegend();
	
	//Set windows size based on the map so UI would be visible.
	void SetWindowSize(int SizeX, int SizeY);
	
	//Draw straight lines on the map
	void LineDraw(vector<MapLine> StraightLines);
	//Draw diagonal lines on the map
	void DiagDraw(vector<MapLine> DiagonalLines);
	//2D dynamic map for loading from XML
	vector<Map2D> map;


	
};

