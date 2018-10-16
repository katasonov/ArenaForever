#pragma once

#include <string>

#include "System.h"
#include "..\AFCommon\wstrf.h"

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
		return WStrF(L"%s\\baseaf", GetAppPath().c_str());
	}

	static wstring GetAppPath()
	{
		return System::GetRealCurrentDirectory();
	}

	void SetAuthCode(const wstring &appCode);
	wstring GetAuthCode();
	void SetPlayerModelName(const wstring &model);
	wstring GetPlayerModelName();
	void SetPlayerSex(const wstring &sex);
	wstring GetPlayerSex();
};
