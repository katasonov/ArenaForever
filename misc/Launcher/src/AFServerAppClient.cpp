#include "AFServerAppClient.hpp"
#include "wstrf.h"
#include "HttpRequest.h"
#include "json.h"

AFServerAppClient afServerClient;

#ifdef LOCAL_SERVER
static string Host = "127.0.0.1";
static int Port = 8889;
#else
static string host = "127.0.0.1";
static int Port = 8889;
#endif

AFServerAppClient& AFServerAppClient::Instance()
{
	return afServerClient;
}

AFServerAppClient::AFServerAppClient()
{

}

//void AFServerAppClient::RegisterPlayer(wstring nickname, wstring email, wstring pass, std::function<void())

void AFServerAppClient::RequestGameArenaAsync(GameArenaResponseClbk &&responseClbk)
{
	thread([responseClbk]
	{
#ifdef LOCAL_SERVER
		_sleep(5000);
		//responseClbk(0, L"212.237.50.11", 28099, L"A", L"A");
		responseClbk(0, L"127.0.0.1", 27001, L"AAA", L"6AE3ADF09C5AC2D6D71C151E08B6410D1FB38E21D6A83A1FE2E974098F935FD36144D5D1334D3E1301FC0EB2EF0FEB1DFA29BDE1B31AF9CEE9DD8FF3F331997AF98B688AD5A9B28A640C2B0FA5AE57135C670A630C61BB09D538D3CCBFE4A3170DACCB694C35E5FFEACC54EF8B37E873F1EE9835B22635ED6D747A56D6AE7985982AB7BAB1D54775E5D19B9B5A3AB0D4053E3F6DE75C1418FFC84E223AE3BBD27D13DC10917798CD4CD039A0CF90C06D666ECF1D72FC4997D0D891FD35443C76193E66C787BA7DCB0C640F580BFC1B66A2B07A8D0F552DD60D4EFD2732E4E2CEDB3CBA33A7A3F5FF16F94E41F61816A55A23E09F57AE83DDDE7A4653A6C96945F63E9E2BCA1A0F8D9D5A2C68D8ECBA3D8D84E589D309863AAF9359ECCF4FF14E");
#endif
		//TODO:
	}).detach();
}

void AFServerAppClient::RegisterNewPlayer(wstring nick, wstring email, wstring pass, function<void(int, wstring)> clbk)
{
	thread([nick, email, pass, clbk]
	{

		auto response = HttpGetStringResponse(Host, Port,
			StrF("/api/v1/player/register?nick=%s&pass=%s&email=%s",
				WtoUtf8(nick).c_str(),
				WtoUtf8(pass).c_str(),
				WtoUtf8(email).c_str()),
			string("HTTP/1.1\r\nUser-Agent: curl/7.33.0\r\nHost: ") + "api.arenaforever.com" + "\r\nAccept: */*",
			10);
		auto body = HTTPBody(response.c_str());
		string val;
		int errCode = 0;
		wstring wval;
		auto err = JsonValueString("AuthCode", body, val);
		if (err == 0)
		{
			wval = UTF8ToW(val);
		}
		else
		{
			err = JsonValueInt("ErrorCode", body, errCode);
			if (err != 0)
				errCode = 0xFFFF;
		}
		clbk(errCode, wval);
	}).detach();
}

void AFServerAppClient::LoginPlayer(wstring email, wstring pass, function<void(int, wstring)> clbk)
{
	thread([email, pass, clbk]
	{

		auto response = HttpGetStringResponse(Host, Port,
			StrF("/api/v1/player/login?email=%s&pass=%s",
				WtoUtf8(email).c_str(),
				WtoUtf8(pass).c_str()),
			string("HTTP/1.1\r\nUser-Agent: curl/7.33.0\r\nHost: ") + "api.arenaforever.com" + "\r\nAccept: */*",
			10);
		auto body = HTTPBody(response.c_str());
		string val;
		int errCode = 0;
		wstring wval;
		auto err = JsonValueString("AuthCode", body, val);
		if (err == 0)
		{
			wval = UTF8ToW(val);
		}
		else
		{
			err = JsonValueInt("ErrorCode", body, errCode);
			if (err != 0)
				errCode = 0xFFFF;
		}
		clbk(errCode, wval);
	}).detach();
}
