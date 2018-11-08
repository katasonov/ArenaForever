#pragma once

#include "ICommand.h"

#include <string>

using namespace std;

class DeleteFileCommand : public ICommand
{
public:

	DeleteFileCommand(const wstring &fpath)
	{

	}

	void Execute() override
	{
		//TODO:
	}
};
