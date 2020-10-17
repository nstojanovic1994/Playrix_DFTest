#pragma once
#include <vector>
#include <iostream>
#include <list>
#include <string>
#include <random>

using namespace std;

//Header file for definition of all globals variables, structures as well as FMath class with static functions for various calculations

//Definition of unsigned int for easier usage and readiability
typedef unsigned int uint;


//SETTINGS
//Global settings for visual representation of the player
//Player character for drawing on the screen
static const char PlayerChar = '@';
//Color of the player while in normal mode
static const uint PlayerColor = 11;
//Color of the player while in the attack mode
static const uint PlayerAttackColor = 13;
//Color of the player while in fortify mode
static const uint PlayerFortifyColor = 10;

//Global settings for visual representation of the enemy
//Enemy character for drawing on the screen
static const char EnemyChar = '&';
//Color of the enemy while in normal mode
static const uint EnemyColor = 12;
//Color of the enemy while in fortify mode
static const uint EnemyFortifyColor = 6;

//Global settings for visual representation of items
//Item character for drawing on the screen
static const char ItemChar = '?';
//Color for the item
static const uint ItemColor = 14;

//2nd dimension of 2D dynamic array for map information storing
struct Map2D
{
	vector<char> char_array; 
	char operator[] (int i)
	{
		return char_array.at(i);
	}
	void Add(char new_char)
	{
		char_array.push_back(new_char);
	}
	int GetSize()
	{
		return char_array.size();
	}
	void SetChar(int i, char new_char)
	{
		char_array.at(i) = new_char;
	}
};

//Vector2D storing for easier passing of positional arguments
struct Vec2D
{
	Vec2D()
	{
		X = Y = 0;
	};

	Vec2D(int inX, int inY)
	{
		X = inX;
		Y = inY;
	};
	int X;
	int Y;
};

//Definition of the straight/diagonal line loaded from saved file
struct MapLine
{
	//Begining of the map line
	Vec2D start;
	//End of the map line
	Vec2D end;
	//type - straight or diagonal
	string type;
};

//Definiton of all necessary information from drawing the map from saved file
struct MapData
{
	//Array of the loaded straight lines
	vector<MapLine> StraightLines;
	//Array of the loaded diagonal lines
	vector<MapLine> DiagonalLines;
	//Loaded map size for initialization of 2D map arry
	Vec2D mapsize;
};

//Structure for all required information of an item
struct ItemData
{
	//Unique item id - All IDs start with 22
	int ID = 0;
	//Position to spawn in the world if loaded from saved game
	Vec2D position;
	//Type of the item (weapon, armor, boots, helmet, shield)
	string type;
	//Damage the item will add when equipped
	int AddDamage = 1;
	//Health the item will add when equipped
	int AddHealth = 1;
	//Armor the item will add when equipped
	int AddArmor = 1;
	//Required strenght in order to equip the item
	int ReqStrength = 1;
	//Required stamina in order to equip the item
	int ReqStamina = 1;
	//Required dexterity in order to equip the item
	int ReqDexterity = 1;
	//Required level in order to equip the item
	int ReqLevel = 1;
	//ID of the owner if the item is not on the ground - 12 is on the ground as no character can have that ID
	int ownerID = 12;
	//Is it in player inventory or equipped
	bool equipedPlayer = false;
	//Rarity of the item (legendary, epic, rare, uncommon, common)
	string rarity;
};

//Struct for information about players equipped items
struct PlayerEquipment
{
	//Equipment slots
	ItemData Helmet;
	ItemData Armor;
	ItemData Boots;
	ItemData Shield;
	ItemData Weapon;
	//Checkers for is the slot filled
	bool HelmetEquiped = false;
	bool ArmorEquiped = false;
	bool BootsEquiped = false;
	bool ShieldEquiped = false;
	bool WeaponEquiped = false;
};

struct CharacterData
{
	//Unique ID for characters - 0 is always for player, enemy IDs start with 11
	int ID = 0;
	Vec2D position;
	int level = 1;
	int CurrentHealth = 100;
	int MaxHealth = 100;
	int Damage = 20;
	int Armor = 10;

	int Strength = 1;
	int Stamina = 1;
	int Dexterity = 1;
	int SkillPoints = 0;
	int HealthPercentage;
	vector<ItemData> Inventory = {};
	PlayerEquipment Equipment;
	int ExperiencePoints = 0;
};

//Struct for loading and holding the data of all characters and items
struct SpawnData
{
	CharacterData PlayerInfo;
	vector<CharacterData> EnemyInfo;
	vector<ItemData> ItemInfo;
};

//Struct for holding information about weight table for spawning items based on rarity
struct ItemWeightTable
{
	const vector<string> types = {"legendary", "epic", "rare", "uncommon", "common"};
	const vector<float> weighttable = { 0.05f, 0.2f, 0.4f, 0.7f, 1.1f };
};

//"Math" library for all needed calulcations
class FMath
{
public:

	//Get distance between two 2D Vectors
	static float GetVec2DDistance(Vec2D First, Vec2D Second)
	{
		Vec2D DistanceVector;
		DistanceVector.X = First.X - Second.X;
		DistanceVector.Y = First.Y - Second.Y;
		return sqrtf(DistanceVector.X * DistanceVector.X + DistanceVector.Y * DistanceVector.Y);
	}

	//Get world comapss direction between two positions
	static string GetVec2DDirection(Vec2D First, Vec2D Second)
	{
		//Get directional vector by subtracting two vectors
		Vec2D DirectionVector;
		DirectionVector.X = First.X - Second.X;
		DirectionVector.Y = First.Y - Second.Y;
		//Get length of the vector for normalization
		float length = float(abs(DirectionVector.X + DirectionVector.Y));
		//Normalize directional vector
		float normalX = float(DirectionVector.X) / length;
		float normalY = float(DirectionVector.Y) / length;

		//Calculate angle of directional vector and convert radians to degrees
		float angle = atan2(normalY, normalX);
		angle = angle * 57.295f;

		//return world direction based on the angle
		if (-22.5f  < angle && angle <= 22.5f)
		{
			return "W";
		}	
		else if (22.5f < angle && angle <= 67.5f)
		{
			return "NW";
		}
		else if (67.5f < angle && angle <= 112.5f)
		{
			return "N";
		}
		else if (112.5f < angle && angle <= 157.5f)
		{
			return "NE";
		}
		else if (-22.5f > angle && angle >= -67.5f)
		{
			return "SW";
		}
		else if (-67.5f > angle && angle >= -112.5f)
		{
			return "S";
		}
		else if (-112.5f > angle && angle >= -157.5f)
		{
			return "SE";
		}
		else
		{
			return "E";
		}
	}

	//Pick an item from weight table in order to distribute drop by rarity
	static int PickWeighted(vector<float> Table)
	{
		//Get sum of all type weights
		float WeightSum = 0.f;
		for (float Weight : Table)
		{
			WeightSum += Weight;
		}
		//Get a random float from 0 to sum of weights
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_real_distribution<float> dis(0.0, WeightSum);
		float RandomWeight = dis(gen);

		//Loop from lowest possible rarity to highest
		//If random float is lower than the rarity limit return that rarity
		//If random float is higher than the rarity subtract the rarity limit from the weight and proceed
		for (uint i = 0; i < Table.size(); i++)
		{
			if (RandomWeight < Table[i])
			{
				return i;
			}
			RandomWeight -= Table[i];
		}
		return Table.size() - 1;
	}

	//Get rarity index based on name
	static uint GetRarity(string inRarity)
	{
		if (inRarity == "common") return 4;
		else if (inRarity == "uncommon") return 3;
		else if (inRarity == "rare") return 2;
		else if (inRarity == "epic") return 1;
		else return 0;
	}
	
	//Get type index based on name
	static uint GetType(string inType)
	{
		if (inType == "armor") return 0;
		else if (inType == "weapon") return 1;
		else if (inType == "helmet") return 2;
		else if (inType == "boots") return 3;
		else if (inType == "shield") return 4;
		else return 0;
	}
	//Generate an item to be dropped by enemy with stats based on enemy level, item rarity and type - Rarity/Type = 5 for random
	static ItemData GenerateItemDrop(uint EnemyLevel, uint inRarity, uint inType)
	{
		ItemWeightTable WeightTable = ItemWeightTable();
		//Item rarity
		if (inRarity == 5)
		{
			inRarity = PickWeighted(WeightTable.weighttable);
		}
		float multiplier = 1;

		//Item level
		ItemData NewItem = ItemData();
		NewItem.ReqLevel = EnemyLevel;
		NewItem.rarity = WeightTable.types.at(inRarity);

		switch (inRarity)
		{
		case 0:
			multiplier = 10;
			break;
		case 1:
			multiplier = 7;
			break;
		case 2:
			multiplier = 4;
			break;
		case 3:
			multiplier = 2;
			break;
		case 4:
			multiplier = 1;
			break;
		}

		
		
		if (inType == 5)
		{
			std::random_device rd;
			std::mt19937 gen(rd());
			std::uniform_int_distribution<int> dis(0, 4);
			inType = dis(gen);
		}
		//Random item type and multipliers based on type
		int StaminaMult, StrenghtMult, DexterityMult;
		StaminaMult = StrenghtMult = DexterityMult = 1;

		switch (inType)
		{
		case 0:
			NewItem.type = "armor";
			StaminaMult = 1;
			StrenghtMult = 0;
			DexterityMult = 2;
			break;
		case 1:
			NewItem.type = "weapon";
			StaminaMult = 0;
			StrenghtMult = 2;
			DexterityMult = 0;
			break;
		case 2:
			NewItem.type = "helmet";
			StaminaMult = 1;
			StrenghtMult = 0;
			DexterityMult = 1;
			break;
		case 3:
			NewItem.type = "boots";
			StaminaMult = 1;
			StrenghtMult = 0;
			DexterityMult = 1;
			break;
		case 4:
			NewItem.type = "shield";
			StaminaMult = 0;
			StrenghtMult = 1;
			DexterityMult = 1;
			break;
		}

		//Calculate item stats
		NewItem.ReqStrength = EnemyLevel * StrenghtMult;
		NewItem.ReqDexterity = EnemyLevel * DexterityMult;
		NewItem.ReqStamina = EnemyLevel * StaminaMult;

		NewItem.AddArmor = int(ceilf((EnemyLevel + DexterityMult) * (1 + multiplier/10)));
		NewItem.AddDamage = int(ceilf((EnemyLevel + StrenghtMult) * (1 + multiplier / 10)));
		NewItem.AddHealth = int(ceilf((EnemyLevel + StaminaMult) * (1 + multiplier / 10)));

		return NewItem;
	}

	//Function to increase ID number without affecting the type index
	static int SplitIDIncrease(int ID)
	{
		//Get individual digits of the ID
		vector<int> digits;
		if (0 == ID) {
			digits.push_back(0);
		}
		else {
			while (ID != 0) {
				int last = ID % 10;
				digits.push_back(last);
				ID = (ID - last) / 10;
			}
		}
		//Get the ID number without type
		int pureID = digits[0];
		for (uint i = 1; i < digits.size() - 2; i++)
		{
			pureID += digits.at(i) * int(powf(10.f, i));
		}
		//Increase pure ID number without type
		pureID++;

		//Check if the number of digits was increased
		int checkDigits = pureID;
		int numExtended = 0;
		vector<int> numDigits;
		while (checkDigits != 0) {
			int last = checkDigits % 10;
			numDigits.push_back(last);
			checkDigits = (checkDigits - last) / 10;
		}
		numExtended = numDigits.size() + 2 - digits.size();
		//Return type index to ID
		for (uint i = digits.size() - 2; i < digits.size(); i++)
		{
			pureID += digits.at(i) * int(powf(10.f, i + numExtended));
		}

		return pureID;

	}

	//Get random bool with possible weighting - Distribution = 0.5 for 50/50
	static bool RandomBool(float Distribution)
	{
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_real_distribution<float> dis(0.0, 1.0);
		float result = dis(gen);

		if (result < Distribution)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	static CharacterData RandomEnemyStats(uint EnemyLevel)
	{
		CharacterData newData;
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<uint> dis(0, 2);
		uint i = 0;
		newData.Strength = newData.Dexterity = newData.Stamina = 1;
		newData.SkillPoints = EnemyLevel * 5;
		while (newData.SkillPoints > 0)
		{
			i = dis(gen);
			switch (i)
			{
			case 0:
				newData.Strength += 3;
				break;
			case 1:
				newData.Dexterity += 3;
				break;
			case 2:
				newData.Stamina += 3;
				break;
			}
			newData.SkillPoints--;
		}
		return newData;
	}
};


