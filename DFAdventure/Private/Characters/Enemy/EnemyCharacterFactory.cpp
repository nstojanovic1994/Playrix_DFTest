#include "..\..\..\Public\Characters\Enemy\EnemyCharacterFactory.h"
#include "..\..\..\Public\Characters\Enemy\EnemyController.h"
#include "..\..\..\Public\Characters\Enemy\EnemyPawn.h"

//Create and return enemy controller class
AController& UEnemyCharacterFactory::CreateController()
{
	return *(new AEnemyController());
}

//Create and return enemy pawn class
APawn& UEnemyCharacterFactory::CreatePawn()
{
	return *(new AEnemyPawn());
}
