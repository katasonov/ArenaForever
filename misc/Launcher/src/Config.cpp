#include "Config.h"
#include <memory>
#include <filesystem>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>

#include <Windows.h>
#include "../../AFCommon/wstrf.h"

static auto_ptr<Config> cfg;
static wstring homePath;

namespace fs = std::experimental::filesystem;


Config& Config::Instance()
{
	if (cfg.get() == nullptr)
	{
		cfg.reset(new Config());
	}

	return *cfg.get();
}

wstring Config::HomePath() 
{
	if (homePath.size() > 0)
		return homePath;
	
	wchar_t dir[4096];
	::GetModuleFileName(NULL, dir, 4096);
	std::wstring strdir = dir;
	size_t pos = strdir.find_last_of(L'\\');
	homePath = strdir.substr(0, pos);
	return homePath;
}

wstring Config::ResourcesPath()
{
	return this->HomePath() + L"\\baseaf";
}

namespace {

	uint32_t hashfile(const wchar_t *filename)
	{

		FILE *f = _wfopen(filename, L"rb");

		unsigned char buff[65538];
		memset(buff, 0, sizeof(buff));

		// Hashing
		uint32_t magic = 5381;
		int n = 0;
		uint32_t i = 0;
		while ((n = fread(buff, 1, 65538, f)) != 0)
		{
			for (int j = 0; j < n; j++)
				magic = ((magic << 5) + magic) + buff[j]; // magic * 33 + c
			i = i + n;
		}

		fclose(f);		
		return  magic;
	}
}

namespace {
	bool sort_by_name(FileTableItem &v1, FileTableItem &v2)
	{
		return (v1.name < v2.name);
	}

}

void Config::GetFolderContentState(wstring path, vector<FileTableItem> &table)
{
	for (auto& p : fs::directory_iterator(ResourcesPath()))
	{
		if (fs::is_directory(p.status()))
			continue;
		auto sz = fs::file_size(p);
		wstring name = p.path().filename().wstring();
		auto hash = hashfile(p.path().c_str());
		table.push_back(FileTableItem { name, hash, sz });
	}

	//sort by file name
	sort(table.begin(), table.end(), sort_by_name);
}


