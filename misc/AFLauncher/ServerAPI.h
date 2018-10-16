#pragma once

#include <string>
#include "../AFCommon/HttpRequest.h"
#include "../AFCommon/wstrf.h"


using namespace std;

#ifdef LOCAL_SERVER
static string Host = "127.0.0.1";
static int Port = 8889;
#else
static string Host = "api.arenaforever.com";
static int Port = 8889;
#endif

class ServerAPI
{
public:
	static wstring ResourceFilesURI()
	{
		if (Port != 80 || Port != 8080)
		{
			return WStrF(L"http://%s:%d/win32/baseaf", UTF8ToW(Host).c_str(), Port);
		}
		else
		{
			return WStrF(L"http://%s/win32/baseaf", UTF8ToW(Host).c_str(), Port);
		}
		
	}

	static wstring AppWin32BinsArchiveURI()
	{
		if (Port != 80 || Port != 8080)
		{
			return WStrF(L"http://%s:%d/appwin32bins.zip", UTF8ToW(Host).c_str(), Port);
		}
		else
		{
			return WStrF(L"http://%s/appwin32bins.zip", UTF8ToW(Host).c_str(), Port);
		}
	}

	static string GetResourceUpdatesMap()
	{
		auto response = HttpGetStringResponse(Host, Port,
			StrF("/api/v1/static/folder?name=baseaf"),
			string("HTTP/1.1\r\nUser-Agent: curl/7.33.0\r\nHost: ") + "api.arenaforever.com" + "\r\nAccept: */*",
			10);
		auto body = HTTPBody(response.c_str());

		return body;
	}

	static string GetAppBinsUpdatesMap()
	{
		auto response = HttpGetStringResponse(Host, Port,
			StrF("/api/v1/static/folder?name=win32"),
			string("HTTP/1.1\r\nUser-Agent: curl/7.33.0\r\nHost: ") + "api.arenaforever.com" + "\r\nAccept: */*",
			10);
		auto body = HTTPBody(response.c_str());

		return body;
	}

	static string GetLoginPlayer(wstring &email, wstring &pass)
	{
		auto response = HttpGetStringResponse(Host, Port,
			StrF("/api/v1/player/login?email=%s&pass=%s",
				WtoUtf8(email).c_str(),
				WtoUtf8(pass).c_str()),
			string("HTTP/1.1\r\nUser-Agent: curl/7.33.0\r\nHost: ") + "api.arenaforever.com" + "\r\nAccept: */*",
			10);
		auto body = HTTPBody(response.c_str());

		return body;
	}

	static string GeRegisterPlayer(const wstring &nick, const wstring &email, const wstring &pass)
	{
		auto response = HttpGetStringResponse(Host, Port,
			StrF("/api/v1/player/register?email=%s&pass=%s&nick=%s",
				WtoUtf8(email).c_str(),
				WtoUtf8(pass).c_str(),
				WtoUtf8(nick).c_str()),
			string("HTTP/1.1\r\nUser-Agent: curl/7.33.0\r\nHost: ") + "api.arenaforever.com" + "\r\nAccept: */*",
			10);
		auto body = HTTPBody(response.c_str());

		return body;
	}

	static string GeArenaToConnect(const wstring &model, const wstring &sex, const wstring &authCode)
	{
		auto response = HttpGetStringResponse(Host, Port,
			StrF("/api/v1/arena/connect?authCode=%s&model=%s&sex=%s",
				WtoUtf8(authCode).c_str(),
				WtoUtf8(model).c_str(),
				WtoUtf8(sex).c_str()),
			string("HTTP/1.1\r\nUser-Agent: curl/7.33.0\r\nHost: ") + "api.arenaforever.com" + "\r\nAccept: */*",
			10);
		auto body = HTTPBody(response.c_str());

		return body;
	}


};
