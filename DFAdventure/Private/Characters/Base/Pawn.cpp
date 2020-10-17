#include "..\..\..\Public\Characters\Base\Pawn.h"
#include "..\..\..\Public\GameMode.h"
#include <iostream>
#include <conio.h>
#include <Windows.h>
using namespace std;

APawn::APawn()
{
}

bool APawn::Move(MovementDirection Direction)
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), PawnColor);
	if (Direction == MovementDirection::UP)
	{
		if (PawnChar == PlayerChar && !CheckPickup(0, -1))
		{
			return false;
		}
		if (GetConsoleRender().UpdateMap(CurrentX, CurrentY, 0, -1, PawnChar))
		{
			CurrentY--;
			if (CanAttack())
			{
				SetAttackMode(true);
			}
			ShouldHeal = false;
			return true;
		}
	}
	if (Direction == MovementDirection::DOWN)
	{
		if (PawnChar == PlayerChar && !CheckPickup(0, 1))
		{
			return false;
		}
		if (GetConsoleRender().UpdateMap(CurrentX, CurrentY, 0, 1, PawnChar))
		{
			CurrentY++;
			if (CanAttack())
			{
				SetAttackMode(true);
			}
			ShouldHeal = false;
			return true;
		}
	}
	if (Direction == MovementDirection::RIGHT)
	{
		if (PawnChar == PlayerChar && !CheckPickup(1, 0))
		{
			return false;
		}
		if (GetConsoleRender().UpdateMap(CurrentX, CurrentY, 1, 0, PawnChar))
		{
			CurrentX++;
			if (CanAttack())
			{
				SetAttackMode(true);
			}
			ShouldHeal = false;
			return true;
		}
	}
	if (Direction == MovementDirection::LEFT)
	{
		if (PawnChar == PlayerChar && !CheckPickup(-1, 0))
		{
			return false;
		}
		if (GetConsoleRender().UpdateMap(CurrentX, CurrentY, -1, 0, PawnChar))
		{
			CurrentX--;
			if (CanAttack())
			{
				SetAttackMode(true);
			}
			ShouldHeal = false;
			return true;
		}
	}
	return false;
}

IConsoleRender& APawn::GetConsoleRender() const
{
	return *ConsoleRender;
}

void APawn::EndTurn()
{
	AttackMode = false;
	if (ShouldHeal)
	{
		float multiplier = 1;
		if (GetFortify())
		{
			multiplier = 1;
		}
		if (GetPawnData().CurrentHealth < GetPawnData().MaxHealth)
		{
			GetPawnData().CurrentHealth += int(ceilf(float(GetPawnData().MaxHealth) * 0.05) * multiplier);
			if (GetPawnData().CurrentHealth > GetPawnData().MaxHealth)
			{
				GetPawnData().CurrentHealth = GetPawnData().MaxHealth;
			}
		}	
	}
	ShouldHeal = true;
	if (PawnChar == PlayerChar)
	{
		GetGameMode().EndTurnProcess(true);
	}
	else
	{
		GetGameMode().EndTurnProcess(false);
	}
}

void APawn::SetConsoleRender(IConsoleRender* NewConsoleRender)
{
	ConsoleRender = NewConsoleRender;
}

void APawn::SetPosition(int X, int Y)
{
	CurrentX = X;
	CurrentY = Y;
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), PawnColor);
	GetConsoleRender().UpdateMap(X, Y, 0, 0, PawnChar);
}

Vec2D APawn::GetPawnPosition() const
{
	return Vec2D(CurrentX, CurrentY);
}

void APawn::SetGameMode(AGameMode* NewGM)
{
	DFGameMode = NewGM;
}

AGameMode& APawn::GetGameMode() const
{
	return *DFGameMode;
}

void APawn::SetCharacterData(CharacterData NewData)
{
	PawnData = NewData;
	PawnData.MaxHealth = 100 + PawnData.level * 10 + PawnData.Stamina * 5;
	PawnData.Damage = PawnData.level * 5 + PawnData.Strength * 3;
	PawnData.Armor = PawnData.level * 2 + PawnData.Dexterity;
	PawnData.CurrentHealth = int(ceilf(float(PawnData.MaxHealth) * float(PawnData.HealthPercentage) / 100.f));
}

CharacterData& APawn::GetPawnData()
{
	return PawnData;
}

bool APawn::TakeDamage(int DamageAmount) 
{
	PawnData.CurrentHealth -= DamageAmount;
	if (PawnData.CurrentHealth < 0)
	{
		Death();
		return true;
	}
	return false;
}

void APawn::SetFortify(bool Set)
{
	IsFortified = Set;
	if (Set)
	{
		GetConsoleRender().ChangePawnColor(GetPawnPosition(), PawnFortifyColor, PawnChar);
	}
	else
	{
		GetConsoleRender().ChangePawnColor(GetPawnPosition(), PawnColor, PawnChar);
	}
}

bool APawn::GetFortify()
{
	return IsFortified;
}

void APawn::IncreaseEXP(int Amount)
{
	GetPawnData().ExperiencePoints += Amount;
	if (GetPawnData().ExperiencePoints > (GetPawnData().level + 1) * (100 + (GetPawnData().level + 1) * 2))
	{
		GetPawnData().SkillPoints += 5;
		GetPawnData().level++;
	}
}

bool APawn::CheckAttackDirection(int X, int Y, char CharToCheck)
{
	return GetConsoleRender().CheckForCharacter(X, Y, CharToCheck);
}

bool APawn::CanAttack()
{
	char checkChar = EnemyChar;
	if (PawnChar == EnemyChar)
	{
		checkChar = PlayerChar;
	}
	if (CheckAttackDirection(CurrentX + 1, CurrentY, checkChar))
	{
		return true;
	}
	else if (CheckAttackDirection(CurrentX - 1, CurrentY, checkChar))
	{
		return true;
	}
	else if (CheckAttackDirection(CurrentX, CurrentY + 1, checkChar))
	{
		return true;
	}
	else if (CheckAttackDirection(CurrentX, CurrentY - 1, checkChar))
	{
		return true;
	}
	else return false;
}

void APawn::SetAttackMode(bool Switch)
{
	AttackMode = Switch;
}
