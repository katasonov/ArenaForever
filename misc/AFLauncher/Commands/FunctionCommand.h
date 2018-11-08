#pragma once

#include "ICommand.h"
#include <functional>

class FunctionCommand : public ICommand
{
	std::function<void()> fn;

public:

	FunctionCommand(std::function<void()> &&_fn)
	{
		this->fn = move(_fn);
	}

	void Execute() override
	{
		fn();
	}

};
