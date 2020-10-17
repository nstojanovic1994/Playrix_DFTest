#include "..\..\..\Public\Characters\Enemy\EnemyPawn.h"
#include "..\..\..\Public\GameMode.h"
#include <iostream>
#include <random>

//Constructor for setting rendering options
AEnemyPawn::AEnemyPawn()
{
	PawnChar = EnemyChar;
	PawnColor = EnemyColor;
	PawnFortifyColor = EnemyFortifyColor;
	VisionCooldown = 0;
}

//Begin automated turn process for enemy pawns
void AEnemyPawn::StartTurn()
{
	//If pawn is low on HP fortify for healing
	if (GetPawnData().CurrentHealth < float(GetPawnData().MaxHealth) * 0.3)
	{
		SetFortify(true);
	}
	//If pawn is in fortify mode and healed over 50% of Max HP leave fortify mode
	else if (GetPawnData().CurrentHealth > float(GetPawnData().MaxHealth) * 0.5 && GetFortify())
	{
		SetFortify(false);
	}
	//If not in fortify mode process moving/attacking
	if (!GetFortify())
	{
		//Get player position and visibility
		Vec2D PlayerPos = GetPlayer();
		//Check vision of the player with 3 turn remembering period
		bool SeePlayer = false;
		if (CanSeePlayer(PlayerPos))
		{
			SeePlayer = true;
			VisionCooldown = 3;
		}
		else if (VisionCooldown > 0)
		{
			SeePlayer = true;
			VisionCooldown--;
		}
		else
		{
			SeePlayer = false;
		}
		//Calculate direction of movement and perform move if possible
		if (!DetermineDirection(PlayerPos, SeePlayer))
		{
			//If pawn didn't move check if it is next to the player and can attack
			int X = PlayerPos.X - GetPawnPosition().X;
			int Y = PlayerPos.Y - GetPawnPosition().Y;
			if (((X == -1 || X == 1) && Y == 0) || ((Y == - 1 || Y == 1) && X == 0))
			{
				//If pawn can attack player perform attack
				if(CheckAttackDirection(GetPawnPosition().X + X, GetPawnPosition().Y + Y, PlayerChar))
				{
					Attack(0, 0);
				}
			}			
		}
		//If pawn moved and entered attack mode automatically attack
		if (AttackMode)
		{
			Attack(0, 0);
		}
	}
	//If pawn is alive send end turn information
	//if (GetPawnData().CurrentHealth > 0)
	//{
	EndTurn();
	//}
}

//Access player position from GM
Vec2D AEnemyPawn::GetPlayer() const
{
	return GetGameMode().GetPlayerPos();
}

//Vision trace Up/Down to detect if vision to the player is obstructed or not
bool AEnemyPawn::TraceUp(bool Up, int traceX, int& traceY)
{
	if (Up)
	{
		traceY--;
		if (!GetConsoleRender().CheckTile(traceX, traceY))
		{
			return false;
		}
	}
	else
	{
		traceY++;
		if (!GetConsoleRender().CheckTile(traceX, traceY))
		{
			return false;
		}
	}
	return true;
}

//Vision trace RightLeft to detect if vision to the player is obstructed or not
bool AEnemyPawn::TraceRight(bool Right, int& traceX, int traceY)
{
	if (Right)
	{
		traceX++;
		if (!GetConsoleRender().CheckTile(traceX, traceY))
		{
			return false;
		}
	}
	else
	{
		traceX--;
		if (!GetConsoleRender().CheckTile(traceX, traceY))
		{
			return false;
		}
	}
	return true;
}

//Check if pawn can see the player by finding optimal straight/diagonal vision trace towards the player
bool AEnemyPawn::CanSeePlayer(Vec2D PlayerInfo)
{
	Vec2D CurrentLocation = GetPawnPosition();
	bool Up, Right, GoUp, GoRight;
	Up = Right = GoUp = GoRight = true;

	//Check each axis distance
	int distanceX, distanceY;
	distanceX = PlayerInfo.X - CurrentLocation.X;
	distanceY = PlayerInfo.Y - CurrentLocation.Y;

	//Check eyesight direction towards player
	if (distanceX == 0)
	{
		GoRight = false;
	}
	else if (PlayerInfo.X < CurrentLocation.X)
	{
		Right = false;
	}
	if (distanceY == 0)
	{
		GoUp = false;
	}
	else if (PlayerInfo.Y > CurrentLocation.Y)
	{
		Up = false;
	}

	int traceX, traceY;
	traceX = CurrentLocation.X;
	traceY = CurrentLocation.Y;

	//If player and enemy are equal on one of the axises only trace along that axis
	if (GoUp && !GoRight)
	{
		for (int i = 1; i < distanceY + 1; i++)
		{
			if (!TraceUp(Up, traceX, traceY))
			{
				return false;
			}
		}
	}
	else if (!GoUp && GoRight)
	{
		for (int i = 1; i < distanceX + 1; i++)
		{
			if (!TraceRight(Right, traceX, traceY))
			{
				return false;
			}
		}
	}

	//If both axises are different
	else if (GoUp && GoRight)
	{
		//In which direction to make bigger steps to simulate diagonal trace
		bool UpFirst = true;
		int distanceStep = 0;
		int currentStep = 0;
		if (distanceY > distanceX)
		{
			int distanceStep = int(floorf(distanceY / distanceX));
		}
		else
		{
			int distanceStep = int(floorf(distanceX / distanceY));
			bool UpFirst = false;
		}

		int totalDistance = distanceX + distanceY;

		while (totalDistance != 0)
		{
			if (UpFirst)
			{
				for (int i = 1; i < distanceStep + 1; i++)
				{
					if (!TraceUp(Up, traceX, traceY))
					{
						return false;
					}
					totalDistance--;
				}
				if (!TraceRight(Right, traceX, traceY))
				{
					return false;
				}
				totalDistance--;
			}
			else
			{
				for (int i = 1; i < distanceStep + 1; i++)
				{
					if (!TraceRight(Right, traceX, traceY))
					{
						return false;
					}
					totalDistance--;
				}
				if (!TraceUp(Up, traceX, traceY))
				{
					return false;
				}
				totalDistance--;
			}
		}
		return true;
	}
	else
	{
		return false;
	}
	return true;
}

//Determine direction of the movement for the pawn
bool AEnemyPawn::DetermineDirection(Vec2D PlayerInfo, bool CanSeePlayer)
{
	//Get distance to player
	Vec2D CurrentLocation = GetPawnPosition();
	float distance = FMath::GetVec2DDistance(PlayerInfo, CurrentLocation);
	bool Up, Right, GoUp, GoRight;
	Up = Right = GoUp = GoRight = true;

	//If within distance and can see player chase after the player pawn
	if (distance < 8 && CanSeePlayer)
	{
		if (PlayerInfo.X == CurrentLocation.X)
		{
			GoRight = false;
		}
		else if (PlayerInfo.X < CurrentLocation.X)
		{
			Right = false;
		}
		if (PlayerInfo.Y == CurrentLocation.Y)
		{
			GoUp = false;
		}
		else if (PlayerInfo.Y > CurrentLocation.Y)
		{
			Up = false;
		}
	}
	//Else roam the map
	else
	{
		//Randomize movement direction of the pawn
		GoUp = FMath::RandomBool(0.5f);
		GoRight = !GoUp;
		Right = Up = FMath::RandomBool(0.5f);
	}

	//Move based on randomized bools
	//Bool holder to check if pawn successfully moved or should try other directions
	bool Moved = false;
	//If only upwards movement is true
	if(!GoRight && GoUp)
	{
		//Switch between up and down
		if (Up)
		{
			return Move(MovementDirection::UP);
			{
				Moved = true;
			}
		}
		else if(!Up || !Moved)
		{
			if (Move(MovementDirection::DOWN))
			{
				Moved = true;
			}
		}
	}
	//If only right movement is true
	if ((!GoUp && GoRight) || (!Moved))
	{
		//Switch between left and right
		if (Right)
		{
			if (Move(MovementDirection::RIGHT))
			{
				Moved = true;
			}
		}
		else if(!Right || !Moved)
		{
			if (Move(MovementDirection::LEFT))
			{
				Moved = true;
			}
		}
	}
	//If both movement directions are true
	else if ((GoUp && GoRight) || !Moved)
	{
		
		//Randomize movement
		bool randBool = FMath::RandomBool(0.5);
		if (randBool)
		{
			if (Up)
			{
				if (Move(MovementDirection::UP))
				{
					Moved = true;
				}
			}
			if(!Up || !Moved)
			{
				if (Move(MovementDirection::DOWN))
				{
					Moved = true;
				}
			}
		}
		if(!randBool || !Moved)
		{
			if (Right)
			{
				if (Move(MovementDirection::RIGHT))
				{
					Moved = true;
				}
			}
			if(!Right || !Moved)
			{
				if (Move(MovementDirection::LEFT))
				{
					Moved = true;
				}
			}
		}
		return Moved;
	}
	return false;
}

//End turn and remove pawn from the game mode
void AEnemyPawn::Death()
{
	EndTurn();
	GetGameMode().EnemyDeath(*this);
}

//Perform attack on player pawn
void AEnemyPawn::Attack(int X, int Y)
{
	ShouldHeal = false;
	GetGameMode().PerformAttack(this, &GetGameMode().GetPlayerController().GetPawn());
	AttackMode = false;
}
