#pragma once

#include <string>
#include <vector>

#include "UpdatesManager.h"

using namespace std;

class Config
{
public:
	
	static Config &Instance();

	wstring HomePath();
	wstring ResourcesPath();

	void GetFolderContentState(wstring path, vector<FileTableItem> &table);
};


