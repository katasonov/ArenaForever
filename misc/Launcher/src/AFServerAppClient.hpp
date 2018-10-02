#pragma once

#include <string>
#include <vector>
#include <thread>

#include "UpdatesManager.h"


using namespace std;

class AFServerAppClient
{
public:

	static AFServerAppClient& Instance();

	AFServerAppClient();

	void ConnectToArenaAsync(
		wstring authCode,
		wstring model, 
		wstring sex, 
		function<void(int errCode, wstring srvIp, int port, wstring accessCode, wstring playerDataEnc)> clbk);

	void RegisterNewPlayerAsync(wstring nick, wstring email, wstring pass, std::function<void(int, wstring)> clbk);
	void LoginPlayerAsync(wstring email, wstring pass, function<void(int, wstring)> clbk);

	void GetCurrentResourcesTableAsync(function<void(int, vector<FileTableItem>&&)> clbk);

};
