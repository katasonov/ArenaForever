#pragma once

#include <string>
#include <thread>

using namespace std;

typedef void(*GameArenaResponseClbk)(int errCode, wstring srvIp, int port, wstring accessCode, wstring playerDataEnc);

class AFServerAppClient
{
public:

	static AFServerAppClient& Instance();

	AFServerAppClient();

	void RequestGameArenaAsync(GameArenaResponseClbk &&responseClbk);
	
};
