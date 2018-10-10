#pragma once

#include <string>
#include <vector>
#include "..\AFCommon\json.h"

#include "Commands\ICommand.h"
#include "..\AFCommon\utils.h"
#include "..\AFCommon\hash.h"
#include "..\AFCommon\wstrf.h"

#include <filesystem>

namespace fs = std::experimental::filesystem;
using namespace std;

class FileInfo
{
public:
	wstring FileName;
	size_t FileSize;
	uint32_t FileHash;
};

class FilesMap
{
	wstring path;

public:

	vector<FileInfo> MapItems;

	static void FromJson(const string &jsonMap, FilesMap& fm)
	{
		
		vector<string> objects;
		if (0 != JsonObjectsArray(jsonMap, objects))
			throw exception("FilesMap invalid");

		for (auto &obj : objects)
		{
			string name;
			intmax_t hash;
			intmax_t size;
			name = JsonValueString("Name", obj);
			hash = JsonValueInt("Hash", obj);
			size = JsonValueInt("Size", obj);

			fm.MapItems.push_back(FileInfo{ UTF8ToW(name), (size_t)size, (uint32_t)hash });
		}

		
	}

	static void FromFolder(const wstring &path, FilesMap &fmp)
	{		
		for (auto& p : fs::directory_iterator(path))
		{
			if (fs::is_directory(p.status()))
				continue;
			auto size = fs::file_size(p);
			string name = p.path().filename().string();
			auto hash = hashfile(p.path().c_str());
			fmp.MapItems.push_back(FileInfo{ UTF8ToW(name), (size_t)size, (uint32_t)hash });
		}
	}



	bool HasFile(const wstring &fn) const
	{
		for (auto it : MapItems)
		{
			if (it.FileName == fn)
				return true;
		}
		return false;
	}

	FileInfo GetFileInfo(const wstring &fn) const
	{
		for (auto it : MapItems)
		{
			if (it.FileName == fn)
				return it;
		}
		throw std::exception("FilesMap::GetFileInfo: failed");
	}


};

bool operator != (const FileInfo &lfi, const FileInfo &rfi);
