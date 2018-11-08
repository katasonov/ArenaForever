#pragma once

#include "ICommand.h"

#include <functional>
#include <map>
#include <string>

#include "../../AFCommon/json.h"

#include "../ServerAPI.h"

#include "../AppState.h"
class SetPlayerNickNameCommand : public ICommand
{
	std::function<void(int err, const wstring& msg)> clbk;
	wstring nick;

public:
	SetPlayerNickNameCommand(wstring nick, std::function<void(int err, const wstring &msg)> &&_fn)
	{
		this->clbk = move(_fn);
		this->nick = nick;
	}

	void Execute() override
	{
		int err = 0xFFFF;
		string response;
		try
		{
			response = ServerAPI::SetPlayerNickName(AppState::Instance().GetAuthCode(), nick);
			err = JsonValueInt("ErrorCode", response);
			wstring message = UTF8ToW(JsonValueString("Message", response));
			clbk(err, message);
		}
		catch (...)
		{
			clbk(err, L"Неизвестная ошибка");
		}
	}
};

