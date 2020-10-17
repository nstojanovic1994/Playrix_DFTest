#pragma once
#include "..\..\..\Public\Characters\Base\Pawn.h"
class APlayerPawn : public APawn
{
public:
	APlayerPawn();


	//Death process of player pawn
	void Death() override;
	//Attack process of player pawn
	void Attack(int X, int Y) override;

	//Check if pickup item is present on targeted tile before moving
	bool CheckPickup(int X, int Y) override;
	//Equip selected item to the corresponding slot
	void EquipItem(ItemData Item) override;
	//Switch the item from the inventory with the one in the corresponding slot
	void SwitchItems(int X, int Y) override;
	//Function for dropping and destroying items in order to free space in inventory
	void DestroyItem(int X, int Y) override;
	//Apply equied item effects
	void ItemEffects(ItemData Item, bool Add);
	//Assign a skill point to the selected stat in the character screen
	bool AssignSkillPoint(int X) override;
	//Switch attack mode stance
	void SetAttackMode(bool Switch) override;

private:
	//Color for visiual presentation of player attack mode
	int PawnAttackColor = PlayerAttackColor;
};