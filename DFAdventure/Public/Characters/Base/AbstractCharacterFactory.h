#pragma once
#include "Controller.h"
#include "Pawn.h"

//Abstract Factory for creation of Characters with Controller and Pawn
class UAbstractCharacterFactory
{
public:
	//Create Controller based on dervied class of the Abstract Factory
	virtual AController& CreateController();
	//Create Pawn based on dervied class of the Abstract Factory
	virtual APawn& CreatePawn();
};