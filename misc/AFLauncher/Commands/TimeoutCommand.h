#pragma once

#include "ICommand.h"

#include <functional>
#include <chrono>
class TimeoutCommand : public ICommand
{
	std::function<bool()> func;

	int timeout = 0;
	int nextTime = 0;

public:
	bool Done = false;

	TimeoutCommand(std::function<bool()> &&func, int timeout)
	{
		this->func = move(func);
		this->timeout = timeout;
		auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
		this->nextTime = now*1000 + this->timeout;
	}

	void Execute() override 
	{
		auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
		if (this->nextTime - now*1000 > 0)
			return;

		//Time's up
		this->Done = !func();
		this->nextTime = now + this->timeout;
	}

};
