#pragma once
#include "..\..\..\Public\Characters\Base\Controller.h"



class APlayerController : public AController
{
public:
	//Receive input from the game process
	void ReceiveInput() override;
	

private:
	//Current location of cursor within menu screens
	int MenuX = 1;
	int MenuY = 1;
};

