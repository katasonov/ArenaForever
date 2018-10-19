#pragma once

#include "ICommand.h"

#include <functional>

#include "../../AFCommon/json.h"

#include "../ServerAPI.h"

#include "../AppState.h"

using namespace std;

class CheckVKOAuthStatusCommand : public ICommand
{
	std::function<void(bool logged, int errCode)> clbk;

public:
	CheckVKOAuthStatusCommand(std::function<void(bool logged, int errCode)> &&_fn)
	{
		this->clbk = move(_fn);
	}

	void Execute() override
	{
		int err = 0xFFFF;
		string response;
		try
		{
			response = ServerAPI::CheckVKOAuthStatus(AppState::Instance().GetVKOAuthCheckStatusCode());
			
			string authCode = JsonValueString("AuthCode", response);
			if (authCode.size() == 0) { 
				clbk(false, 0);
				return;
			}
			AppState::Instance().SetAuthCode(UTF8ToW(authCode));

			clbk(true, 0);
		}
		catch (...)
		{
			try
			{
				err = JsonValueInt("ErrorCode", response);
			}
			catch (...)
			{
				clbk(false, err);
				return;
			}

			clbk(false, err);
		}
	}
};
