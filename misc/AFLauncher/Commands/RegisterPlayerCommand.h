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

class RegisterPlayerCommand : public ICommand
{
	std::function<void(int errCode)> clbk;
	wstring nick, email, pass;

public:

	RegisterPlayerCommand(wstring nick, wstring email, wstring pass, std::function<void(int errCode)> &&_fn)
	{
		this->clbk = move(_fn);
		this->email = email;
		this->pass = pass;
		this->nick = nick;
	}

	void Execute() override
	{

		int err = 0xFFFF;
		string response;
		try
		{
			response = ServerAPI::GeRegisterPlayer(nick, email, pass);
			string authCode = JsonValueString("AuthCode", response);
			if (authCode.size() == 0) {
				clbk(err);
				return;
			}
			AppState::Instance().SetAuthCode(UTF8ToW(authCode));
			clbk(0);
		}
		catch (...)
		{
			try
			{
				err = JsonValueInt("ErrorCode", response);
			}
			catch (...)
			{
				clbk(err);
				return;
			}

			clbk(err);
		}
	}

};
