#pragma once

#include "ICommand.h"
#include <functional>
#include <exception>

#include "../System.h"
#include "../AppState.h"

#include "../ServerAPI.h"
#include "../FilesMap.h"

#include <utility>

using namespace std;

//class ResourceInfo
//{
//public:
//	unsigned int FileSize;
//	wstring FileURI;
//	wstring FileName;
//};

class CheckAndDownloadNewAppFilesCommand : public ICommand
{
	std::function<void(int errCode, int progress, const wstring &folder)> clbk;


	//ResourceInfo ParseResourceFileInfo(string jsonInfo)
	//{
	//	ResourceInfo ri;
	//	ri.FileSize = JsonValueInt("FileSize", jsonInfo);
	//	ri.FileName = UTF8ToW(JsonValueString("FileName", jsonInfo));
	//	ri.FileURI = UTF8ToW(JsonValueString("FileURI", jsonInfo));

	//	return ri;
	//}


public:

	CheckAndDownloadNewAppFilesCommand(std::function<void(int errCode, int progress, const wstring &folder)> &&_fn)
	{
		this->clbk = move(_fn);
	}

	void Execute() override
	{

		int err = 0;
		try
		{
			string response = ServerAPI::GetAppBinsUpdatesMap(); 
			FilesMap remote;
			FilesMap::FromJson(response, remote);
			//fn(0, 10);
			FilesMap local;
			FilesMap::FromFolder(AppState::GetAppPath(), local);
			if (remote.MapItems.size() < 1)
				throw exception("CheckAndDownloadNewAppFilesCommand: failed to load resources");


			vector<FileInfo> updateFiles;
			vector<FileInfo> loadFiles;
			bool foundDiff = false;
			for (auto it : remote.MapItems)
			{
				if (!local.HasFile(it.FileName))
				{
					foundDiff = true;
					break;
				}
				if (remote.GetFileInfo(it.FileName) != local.GetFileInfo(it.FileName))
				{
					foundDiff = true;
					break;
				}
			}

			if (!foundDiff)
			{
				clbk(0, 100, L"");
				return;
			}

			
			auto tmpFilePath = System::GetTempFilePath();
			if (!System::DownloadFile(
				 ServerAPI::AppWin32BinsArchiveURI(), tmpFilePath,
				[this](unsigned int downloadedBytes, unsigned int totalBytes)->bool
			{
				if (totalBytes == 0)
					totalBytes = 1;
				unsigned int progress = (int)floor(((float)downloadedBytes / totalBytes) * 100);
				if (progress > 99)
					progress = 99;
				this->clbk(0, progress, L"");
				return true;
			}))
			{
				throw exception("Failed to download file");
			}
			
			auto now = std::chrono::system_clock::now();
			auto uptFolder = WStrF(L"up-%d", std::chrono::system_clock::to_time_t(now));
			auto toPath = System::JoinPath(AppState::GetAppPath(), uptFolder);
			System::UnZipContentFolder(tmpFilePath, toPath, L"arenaf");

			//TODO: Start Sync in tis to copy app files

			
			clbk(0, 100, uptFolder);
		}
		catch (...)
		{
			clbk(1, 0, L"");
		}

	}

};
