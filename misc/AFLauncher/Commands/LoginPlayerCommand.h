#pragma once

#include "ICommand.h"
#include <functional>
#include <exception>

#include "../System.h"
#include "../AppState.h"

#include "../ServerAPI.h"
#include "../FilesMap.h"

#include <utility>

using namespace std;

class LoginPlayerCommand : public ICommand
{
	std::function<void(int errCode)> clbk;
	wstring email, pass;

public:

	LoginPlayerCommand(wstring email, wstring pass, std::function<void(int errCode)> &&_fn)
	{
		this->clbk = move(_fn);
		this->email = email;
		this->pass = pass;
	}

	void Execute() override
	{

		int err = 0;
		try
		{
			string response = ServerAPI::GetLoginPlayer(email, pass);
			string authCode = JsonValueString("AuthCode", response);
			if (authCode.size() == 0) {
				clbk(100);
				return;
			}
			AppState::Instance().SetAuthCode(UTF8ToW(authCode));
			clbk(0);
		}
		catch (...)
		{
			clbk(100);
		}
	}

};
