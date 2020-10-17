#include "..\..\..\Public\Characters\Player\PlayerCharacterFactory.h"
#include "..\..\..\Public\Characters\Player\PlayerController.h"
#include "..\..\..\Public\Characters\Player\PlayerPawn.h"

//Create and return player controller class
AController& UPlayerCharacterFactory::CreateController()
{
	return *(new APlayerController());
}

//Create and return player pawn class
APawn& UPlayerCharacterFactory::CreatePawn()
{
	return *(new APlayerPawn());
}
