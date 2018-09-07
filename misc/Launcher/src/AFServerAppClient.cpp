#include "AFServerAppClient.hpp"

AFServerAppClient afServerClient;

AFServerAppClient& AFServerAppClient::Instance()
{
	return afServerClient;
}

AFServerAppClient::AFServerAppClient()
{

}

void AFServerAppClient::RequestGameArenaAsync(GameArenaResponseClbk &&responseClbk)
{
	thread([responseClbk]
	{
#ifdef LOCAL_SERVER
		_sleep(5000);
		//responseClbk(0, L"212.237.50.11", 28099, L"A", L"A");
		responseClbk(0, L"127.0.0.1", 27001, L"A", L"A");
#endif
		//TODO:
	}).detach();
}
