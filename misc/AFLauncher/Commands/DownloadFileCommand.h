#pragma once

#include "ICommand.h"
#include "../FilesMap.h"


#include <string>

using namespace std;

class DownloadFileCommand : public ICommand
{
public:

	DownloadFileCommand(const FileInfo &fi, const wstring &fpath)
	{

	}

	void Execute() override
	{
		//TODO:
	}
};
