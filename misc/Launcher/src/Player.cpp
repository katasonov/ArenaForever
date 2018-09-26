#include "Player.h"

static Player player;

Player& Player::Instance()
{
	return player;
}



