#include "..\..\..\Public\Characters\Base\Controller.h"

//Return currently controlled pawn
APawn& AController::GetPawn() const
{
	return *ControlledPawn;
}

//Assign new controlled pawn
void AController::SetPawn(APawn& NewPawn)
{
	ControlledPawn = &NewPawn;
}

//Set current state between UI types and in game
void AController::SetCurrentState(MenuOpened NewState)
{
	CurrentState = NewState;
}

//Returns current state of operating
MenuOpened AController::GetCurrentState()
{
	return CurrentState;
}
