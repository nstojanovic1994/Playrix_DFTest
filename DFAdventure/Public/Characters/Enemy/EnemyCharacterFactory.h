#pragma once
#include "..\Base\AbstractCharacterFactory.h"

//Factory for creation of Enemy Characters with Controller and Pawn
class UEnemyCharacterFactory : public UAbstractCharacterFactory
{
	AController& CreateController() override;
	APawn& CreatePawn() override;
};

