#pragma once
#include "..\..\View\ConsoleRender.h"

//Movement direction enum for readability
enum class MovementDirection : int
{
	UP = 0,
	DOWN = 1,
	RIGHT = 2,
	LEFT = 3
};

//Avoid circular dep
class AGameMode;

class APawn
{
public:
	APawn();
	//Move pawn in the given direction
	bool Move(MovementDirection Direction);

	//Get Console Render
	IConsoleRender& GetConsoleRender() const;

	//Turn process
	void EndTurn();
	virtual void StartTurn() {};
	
	//Set Console Render
	void SetConsoleRender(IConsoleRender* NewConsoleRender);

	//Postion of the pawn
	void SetPosition(int X, int Y);
	Vec2D GetPawnPosition()  const;

	//Pawn char and color
	char PawnChar = PlayerChar;
	uint PawnColor = PlayerColor;
	uint PawnFortifyColor = PlayerFortifyColor;

	//Game mode for pawn interaction
	void SetGameMode(AGameMode* NewGM);
	AGameMode& GetGameMode() const;

	//Stats system
	void SetCharacterData(CharacterData NewData);
	CharacterData& GetPawnData();

	//Take damage from attack
	bool TakeDamage(int DamageAmount);
	//Death process if HP is 0 or below
	virtual void Death() {};

	//Change fortify stance
	void SetFortify(bool Set);
	//Check if the pawn is in fortify stance
	bool GetFortify();

	//Gain experience from kill
	void IncreaseEXP(int Amount);

	//Check if attack direction is valid
	bool CheckAttackDirection(int X, int Y, char CharToCheck);
	//Check if the pawn is currently in attack mode
	mutable bool AttackMode = false;
	//Check if pawn has a valid target for attacking
	bool CanAttack();
	//Switch attack mode stance
	virtual void SetAttackMode(bool Switch);
	//Perfrom attack on the target
	virtual void Attack(int X, int Y) {};

	//Items
	//Check if pickup item is present on targeted tile before moving and if player can pick it up
	virtual bool CheckPickup(int X, int Y) { return true; };
	//Equip selected item to the corresponding slot
	virtual void EquipItem(ItemData Item) {};
	//Switch the item from the inventory with the one in the corresponding slot
	virtual void SwitchItems(int X, int Y) {};
	//Function for dropping and destroying items in order to free space in inventory
	virtual void DestroyItem(int X, int Y) {};
	//Assign a skill point to the selected stat in the character screen
	virtual bool AssignSkillPoint(int X) { return true; };

	//Check if pawn moved or attacked this turn in order to determine the healing process
	bool ShouldHeal = false;

private:
	//Is pawn in fortify stance
	bool IsFortified = false;
	//Pointer to current game mode process
	AGameMode* DFGameMode;
	//Current location of the pawn
	uint CurrentX;
	uint CurrentY;
	//Pawn information regarding stats and items
	CharacterData PawnData;
	//Console Render for updating view
	IConsoleRender* ConsoleRender;
	
};

