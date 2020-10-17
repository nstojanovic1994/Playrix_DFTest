#include "..\..\..\Public\Characters\Player\PlayerPawn.h"
#include "..\..\..\Public\GameMode.h"

//Constructor for setting rendering options
APlayerPawn::APlayerPawn()
{
	PawnChar = PlayerChar;
	PawnColor = PlayerColor;
	PawnFortifyColor = PlayerFortifyColor;
}

//Show game over screen if player died
void APlayerPawn::Death()
{
	GetGameMode().EndGame(false);
}

//Try to perform the attack in the given direction
void APlayerPawn::Attack(int X, int Y)
{
	//Check if enemy is present in the given direction by detecting drawn characters
	if (CheckAttackDirection(GetPawnPosition().X + X, GetPawnPosition().Y + Y, EnemyChar))
	{
		//If enemy is present try to get the reference from GameMode
		APawn* Enemy(GetGameMode().GetEnemy(GetPawnPosition().X + X, GetPawnPosition().Y + Y));
		//If reference is successfully received
		if (Enemy)
		{
			//Notify that the pawn attacked in this turn to disable end turn healing
			ShouldHeal = false;
			//Perform the attack on the enemy
			GetGameMode().PerformAttack(this, Enemy);
		}
		//Disable the attack mode stance and end the turn
		SetAttackMode(false);
		EndTurn();
	}
}

//Check if pickup is present on the given tile
bool APlayerPawn::CheckPickup(int X, int Y)
{
	//Check if item char is printed on the location
	if (GetConsoleRender().CheckForCharacter(GetPawnPosition().X + X, GetPawnPosition().Y + Y, ItemChar))
	{
		//Add the item to the player inventory if there is space and allow player to move
		if (GetPawnData().Inventory.size() < 16)
		{
			GetGameMode().AddToPlayer(GetPawnPosition(), X, Y);
			return true;
		}
		//Block the player from moving to item if there is no space in inventory
		else
		{
			return false;
		}
	}
	//Return true to enable movement if no items are present
	else return true;
}

//Equip item when loading from .json to the corresponding slot
void APlayerPawn::EquipItem(ItemData Item)
{
	if (Item.type == "armor")
	{
		GetPawnData().Equipment.Armor = Item;
		GetPawnData().Equipment.ArmorEquiped = true;
	}
	else if (Item.type == "weapon")
	{
		GetPawnData().Equipment.Weapon = Item;
		GetPawnData().Equipment.WeaponEquiped = true;
		ItemEffects(Item, true);
	}
	else if (Item.type == "helmet")
	{
		GetPawnData().Equipment.Helmet = Item;
		GetPawnData().Equipment.HelmetEquiped = true;
	}
	else if (Item.type == "boots")
	{
		GetPawnData().Equipment.Boots = Item;
		GetPawnData().Equipment.BootsEquiped = true;
	}
	else if (Item.type == "shield")
	{
		GetPawnData().Equipment.Shield = Item;
		GetPawnData().Equipment.ShieldEquiped = true;
	}
	ItemEffects(Item, true);
}

//Equip the item from the inventory to the corresponding slot
void APlayerPawn::SwitchItems(int X, int Y)
{
	//Check if item index is valid
	if ((X - 1) + (Y - 1) * 4 < GetPawnData().Inventory.size())
	{
		uint index = (X - 1) + (Y - 1) * 4;
		ItemData Item = GetPawnData().Inventory.at(index);
		//If player has required stats for using the item
		if (Item.ReqLevel <= GetPawnData().level && Item.ReqDexterity <= GetPawnData().Dexterity && Item.ReqStamina <= GetPawnData().Stamina && Item.ReqStrength <= GetPawnData().Strength)
		{
			if (Item.type == "armor")
			{
				//Switch the item with the equiped one, placing the old equipment in the inventory
				if (GetPawnData().Equipment.ArmorEquiped)
				{
					GetPawnData().Inventory.at(index) = GetPawnData().Equipment.Armor;
					GetPawnData().Inventory.at(index).equipedPlayer = false;
					ItemEffects(GetPawnData().Equipment.Armor, false);
				}
				else
				{
					GetPawnData().Inventory.erase(GetPawnData().Inventory.begin() + index);
				}
				GetPawnData().Equipment.Armor = Item;
				GetPawnData().Equipment.Armor.equipedPlayer = true;
				GetPawnData().Equipment.ArmorEquiped = true;
			}
			else if (Item.type == "weapon")
			{
				//Switch the item with the equiped one, placing the old equipment in the inventory
				if (GetPawnData().Equipment.WeaponEquiped)
				{
					GetPawnData().Inventory.at(index) = GetPawnData().Equipment.Weapon;
					GetPawnData().Inventory.at(index).equipedPlayer = false;
					ItemEffects(GetPawnData().Equipment.Weapon, false);
				}
				else
				{
					GetPawnData().Inventory.erase(GetPawnData().Inventory.begin() + index);
				}
				GetPawnData().Equipment.Weapon = Item;
				GetPawnData().Equipment.Weapon.equipedPlayer = true;
				GetPawnData().Equipment.WeaponEquiped = true;
			}
			else if (Item.type == "helmet")
			{
				//Switch the item with the equiped one, placing the old equipment in the inventory
				if (GetPawnData().Equipment.HelmetEquiped)
				{
					GetPawnData().Inventory.at(index) = GetPawnData().Equipment.Helmet;
					GetPawnData().Inventory.at(index).equipedPlayer = false;
					ItemEffects(GetPawnData().Equipment.Helmet, false);
				}
				//Equipment slot was empty so just move item from inventory to equipment
				else
				{
					GetPawnData().Inventory.erase(GetPawnData().Inventory.begin() + index);
				}
				GetPawnData().Equipment.Helmet = Item;
				GetPawnData().Equipment.Helmet.equipedPlayer = true;
				GetPawnData().Equipment.HelmetEquiped = true;
			}
			else if (Item.type == "boots")
			{
				//Switch the item with the equiped one, placing the old equipment in the inventory
				if (GetPawnData().Equipment.BootsEquiped)
				{
					GetPawnData().Inventory.at(index) = GetPawnData().Equipment.Boots;
					GetPawnData().Inventory.at(index).equipedPlayer = false;
					ItemEffects(GetPawnData().Equipment.Boots, false);
				}
				//Equipment slot was empty so just move item from inventory to equipment
				else
				{
					GetPawnData().Inventory.erase(GetPawnData().Inventory.begin() + index);
				}
				GetPawnData().Equipment.Boots = Item;
				GetPawnData().Equipment.Boots.equipedPlayer = true;
				GetPawnData().Equipment.BootsEquiped = true;
			}
			else if (Item.type == "shield")
			{
				//Switch the item with the equiped one, placing the old equipment in the inventory
				if (GetPawnData().Equipment.ShieldEquiped)
				{
					GetPawnData().Inventory.at(index) = GetPawnData().Equipment.Shield;
					GetPawnData().Inventory.at(index).equipedPlayer = false;
					ItemEffects(GetPawnData().Equipment.Shield, false);
				}
				//Equipment slot was empty so just move item from inventory to equipment
				else
				{
					GetPawnData().Inventory.erase(GetPawnData().Inventory.begin() + index);
				}
				GetPawnData().Equipment.Shield = Item;
				GetPawnData().Equipment.Shield.equipedPlayer = true;
				GetPawnData().Equipment.ShieldEquiped = true;
			}
			//Applying item effects of the new item
			ItemEffects(Item, true);
			//Update inventory UI
			GetConsoleRender().SwitchedItems(X, Y, GetPawnData());
		}	
	}
}

//Drop item from the inventory and destroy it
void APlayerPawn::DestroyItem(int X, int Y)
{
	//Check if index is valid
	if ((X - 1) + (Y - 1) * 4 < GetPawnData().Inventory.size())
	{
		uint index = (X - 1) + (Y - 1) * 4;
		GetPawnData().Inventory.erase(GetPawnData().Inventory.begin() + index);
		GetConsoleRender().SwitchedItems(X, Y, GetPawnData());
	}
}

//Add/Remove item effects if equipped or unequipped
void APlayerPawn::ItemEffects(ItemData Item, bool Add)
{
	//Equip
	if (Add)
	{
		GetPawnData().Armor += Item.AddArmor;
		GetPawnData().Damage += Item.AddDamage;
		GetPawnData().HealthPercentage = ceilf(float(GetPawnData().CurrentHealth) / float(GetPawnData().MaxHealth) * 100);
		GetPawnData().MaxHealth += Item.AddHealth;
		GetPawnData().CurrentHealth = int(ceilf(float(GetPawnData().MaxHealth) * float(GetPawnData().HealthPercentage) / 100.f));
	}
	//Unequip
	else
	{
		GetPawnData().Armor -= Item.AddArmor;
		GetPawnData().Damage -= Item.AddDamage;
		GetPawnData().HealthPercentage = ceilf(float(GetPawnData().CurrentHealth) / float(GetPawnData().MaxHealth) * 100);
		GetPawnData().MaxHealth -= Item.AddHealth;
		GetPawnData().CurrentHealth = int(ceilf(float(GetPawnData().MaxHealth) * float(GetPawnData().HealthPercentage) / 100.f));
	}
}

//Assign skill points to the selected skill if any are available and adjust player stats
bool APlayerPawn::AssignSkillPoint(int X)
{
	if (GetPawnData().SkillPoints > 0)
	{
		switch (X)
		{
		//Increase player strength
		case 1:
			GetPawnData().Strength++;
			GetPawnData().Damage += 3;
			break;
		//Increase player stamina
		case 2:
			ceilf(float(GetPawnData().CurrentHealth) / float(GetPawnData().MaxHealth) * 100);
			GetPawnData().Stamina++;
			GetPawnData().MaxHealth += 5;
			GetPawnData().CurrentHealth += int(ceilf(float(GetPawnData().CurrentHealth) / float(GetPawnData().MaxHealth) * 5.f));
			break;
		//Increase player dexterity
		case 3:
			GetPawnData().Dexterity++;
			GetPawnData().Armor += 2;
			break;
		}
		//Reduce available skill points
		GetPawnData().SkillPoints--;
		//return true if stats changed in order to update UI
		return true;
	}
	else return false;
}

//Switch player attack mode and represent it by changing player color
void APlayerPawn::SetAttackMode(bool Switch)
{
	APawn::SetAttackMode(Switch);
	if (Switch)
	{
		GetConsoleRender().ChangePawnColor(GetPawnPosition(), PawnAttackColor, PawnChar);
	}
	else
	{
		GetConsoleRender().ChangePawnColor(GetPawnPosition(), PawnColor, PawnChar);
	}
}
