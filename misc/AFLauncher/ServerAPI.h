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

	static string GetResourceUpdatesMap()
	{
		auto response = HttpGetStringResponse(Host, Port,
			StrF("/api/v1/static/folder?name=baseaf"),
			string("HTTP/1.1\r\nUser-Agent: curl/7.33.0\r\nHost: ") + "api.arenaforever.com" + "\r\nAccept: */*",
			10);
		auto body = HTTPBody(response.c_str());

		return body;
	}

	//static string GetStaticFileInfo(string fileName)
	//{
	//	auto response = HttpGetStringResponse(Host, Port,
	//		StrF("/api/v1/static/file?name=%s", fileName.c_str()),
	//		string("HTTP/1.1\r\nUser-Agent: curl/7.33.0\r\nHost: ") + "api.arenaforever.com" + "\r\nAccept: */*",
	//		10);
	//	auto body = HTTPBody(response.c_str());

	//	return body;
	//}
};
