#pragma once

#include "..\AFCommon\Logger.h"


#include "Commands/ICommand.h"
#include "Commands\FunctionCommand.h"
#include "Commands\TimeoutCommand.h"

#include "System.h"
#include "AppState.h"

#include "SharedQueue.h"
#include <utility>
#include <thread>
#include <chrono>


class Switcher
{
private:
	static Switcher switcher;

	Switcher()
	{}

private:
	SharedQueue<ICommand*> _commands;

public:

	static Switcher & Instance()
	{
		return switcher;
	}


	void AddCommand(ICommand *command)
	{
		_commands.push_back(command);
	}
	void AddCommand(std::function<void()> &&func)
	{		
		auto fc = new FunctionCommand(move(func));
		_commands.push_back(fc);
	}

	void Run()
	{
		using namespace std::chrono_literals;

		wstring appPath = AppState::GetLogsPath();

		std::thread(
			[this, appPath]
		{
			DeleteFile(System::JoinPath(appPath, L"commads.log.txt").c_str());
			Logger logger(appPath, L"commads.log.txt");

			while (true)
			{
				ICommand* cmd = _commands.front();
				cmd->SetLogger(&logger);
				cmd->Execute();
				_commands.pop_front();

				bool deleteCmd = true;
				//Check if TimeoutCommand behaviour
				auto tmCmd = dynamic_cast<TimeoutCommand*>(cmd);
				if (tmCmd != NULL)
				{
					if (!tmCmd->Done)
					{
						_commands.push_back(cmd);
						deleteCmd = false;
					}
				}
				if (deleteCmd)
					delete cmd;
				std::this_thread::sleep_for(10ms);

			}
		}).detach();
	}

	void RepeatOnTimeout(std::function<bool()> &&func, int timeout)
	{
		AddCommand(new TimeoutCommand(move(func), timeout));
	}
};