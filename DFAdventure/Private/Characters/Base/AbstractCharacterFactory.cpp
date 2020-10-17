#include "..\..\..\Public\Characters\Base\AbstractCharacterFactory.h"

//Create and return base controller class
AController& UAbstractCharacterFactory::CreateController()
{
	return *(new AController());
}

//Create and return base pawn class
APawn& UAbstractCharacterFactory::CreatePawn()
{
	return *(new APawn());
}
