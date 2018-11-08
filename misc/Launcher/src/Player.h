#ifndef _PLAYER_2609181438_
#define _PLAYER_2609181438_

#include <string>

using namespace std;

class Player 
{

public:
	static Player& Instance();

	wstring AuthCode;
	wstring Nick;
	wstring Email;
	wstring Model;
	wstring Sex;
};

#endif
