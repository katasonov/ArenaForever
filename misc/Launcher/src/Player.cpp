#include "Player.h"

static Player player;

static bool initialized = false;

Player& Player::Instance()
{
	if (!initialized)
	{
		//TODO: set player saved config or defaults

		player.Model = L"assassin/default";
		player.Sex = L"male";
		initialized = true;
	}
	return player;
}



