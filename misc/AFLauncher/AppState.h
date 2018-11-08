#pragma once

#include <string>

#include "System.h"
#include "..\AFCommon\wstrf.h"
#include "..\AFCommon\utils.h"

using namespace std;

class AppState
{

	static AppState appState;

	AppState();

public:

	static AppState & Instance()
	{
		return appState;
	}

	static wstring GetResourcesPath()
	{
		auto path = WStrF(L"%s\\baseaf", GetAppPath().c_str());
		if (!utils::DirExists(path))
			utils::CreateDirectoryRecursively(path.c_str(), false);
		return path;
		//return WStrF(L"%s\\baseaf", GetAppPath().c_str());
	}

	static wstring GetLogsPath()
	{
		auto path = WStrF(L"%s\\logs", GetAppPath().c_str());
		if (!utils::DirExists(path))
			utils::CreateDirectoryRecursively(path.c_str(), false);
		return path;
	}

	static wstring GetAppPath()
	{
		return System::GetRealCurrentDirectory();
	}

	wstring GetVKOAuthUri();

	void SetAuthCode(const wstring &appCode);
	wstring GetAuthCode();
	void SetPlayerModelName(const wstring &model);
	wstring GetPlayerModelName();
	void SetPlayerSex(const wstring &sex);
	wstring GetPlayerSex();

	wstring GetVKOAuthCheckStatusCode();

	void SetPersistentValue(const string &key, const int value);


};
