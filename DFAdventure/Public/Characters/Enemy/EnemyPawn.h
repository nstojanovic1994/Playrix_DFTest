#pragma once
#include "..\..\..\Public\Characters\Base\Pawn.h"
#include "..\..\..\Public\View\DataStruct.h"

class AEnemyPawn : public APawn
{
public:
	AEnemyPawn();

	//Start the turn process for current pawn
	void StartTurn() override;

	//Horizontal and vertical tracing functions in order to determine if player is in line of sight 
	bool TraceUp(bool Up, int traceX, int& traceY);
	bool TraceRight(bool Right, int& traceX, int traceY);
	//Use tracing functions in order to return the visibility of the player
	bool CanSeePlayer(Vec2D PlayerInfo);

	//Get needed player pawn position needed for other functions
	Vec2D GetPlayer() const;
	
	//Determine direction of the movement. If pawn is within range and visible move towards the pawn, if not perform random movement
	bool DetermineDirection(Vec2D PlayerInfo, bool CanSeePlayer);

	//Death process of enemy pawn
	void Death() override;
	//Attack process of enemy pawn
	void Attack(int X, int Y) override;
	
private:
	//Vision cooldown in order to make enemy pawn "remember" the direction of the player for n turns, even if it looses sight.
	mutable unsigned int VisionCooldown = 0;
};

