#pragma once
#include "..\..\..\Public\Characters\Base\AbstractCharacterFactory.h"

//Factory for creation of Player Characters with Controller and Pawn
class UPlayerCharacterFactory : public UAbstractCharacterFactory
{
	AController& CreateController() override;
	APawn& CreatePawn() override;
};

