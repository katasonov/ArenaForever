#pragma once

#include "Commands/ICommand.h"
#include "Commands\FunctionCommand.h"
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

		std::thread(
			[this] 
		{
			while (true)
			{
				ICommand* cmd = _commands.front();
				cmd->Execute();
				_commands.pop_front();
				delete cmd;
				std::this_thread::sleep_for(10ms);

			}
		}).detach();
	}
};