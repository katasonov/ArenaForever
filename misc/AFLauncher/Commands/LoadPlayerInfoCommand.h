#pragma once

#include "ICommand.h"

#include <functional>
#include <map>
#include <string>

#include "../../AFCommon/json.h"

#include "../ServerAPI.h"

#include "../AppState.h"
class LoadPlayerInfoCommand : public ICommand
{
public:
	struct PlayerInfo 
	{
		wstring nickName;
		int score;
	};

private:
	std::function<void(int err, PlayerInfo& plInf)> clbk;

public:
	LoadPlayerInfoCommand(std::function<void(int err, PlayerInfo& plInf)> &&_fn)
	{
		this->clbk = move(_fn);
	}

	void Execute() override
	{
		int err = 0xFFFF;
		string response;
		PlayerInfo plInf;

		try
		{
			response = ServerAPI::LoadPlayerInfo(AppState::Instance().GetAuthCode());			

			
			plInf.nickName = UTF8ToW(JsonValueString("Nick", response));
			plInf.score = JsonValueInt("Score", response);

			clbk(0, plInf);
		}
		catch (...)
		{
			try
			{
				err = JsonValueInt("ErrorCode", response);
			}
			catch (...)
			{
				clbk(err, plInf);
				return;
			}

			clbk(err, plInf);
		}
	}
};
