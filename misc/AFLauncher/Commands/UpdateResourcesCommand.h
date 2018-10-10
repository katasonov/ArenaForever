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

class UpdateResourcesCommand : public ICommand
{
	std::function<void(int errCode, int progress)> clbk;


	//ResourceInfo ParseResourceFileInfo(string jsonInfo)
	//{
	//	ResourceInfo ri;
	//	ri.FileSize = JsonValueInt("FileSize", jsonInfo);
	//	ri.FileName = UTF8ToW(JsonValueString("FileName", jsonInfo));
	//	ri.FileURI = UTF8ToW(JsonValueString("FileURI", jsonInfo));

	//	return ri;
	//}
	

public:

	UpdateResourcesCommand(std::function<void(int errCode, int progress)> &&_fn)
	{
		this->clbk = move(_fn);
	}

	void Execute() override
	{	

		int err = 0;
		try
		{
			string response = ServerAPI::GetResourceUpdatesMap();
			FilesMap remote;
			FilesMap::FromJson(response, remote);
			//fn(0, 10);
			FilesMap local;
			FilesMap::FromFolder(AppState::GetResourcesPath(), local);
			if (remote.MapItems.size() < 1)
				throw exception("UpdateResourcesCommand: failed to load resources");

			
			vector<FileInfo> removeFiles;
			vector<FileInfo> updateFiles;
			vector<FileInfo> loadFiles;
			for (auto it : local.MapItems)
			{
				if (!remote.HasFile(it.FileName))
				{
					removeFiles.push_back(it);
					//cmds.push_back(new DeleteFileCommand(local.GetFileFullPath(fn)));
				}
				else if (remote.GetFileInfo(it.FileName) != local.GetFileInfo(it.FileName))
				{
					updateFiles.push_back(it);
				}
			}
			for (auto it : remote.MapItems)
			{
				if (!local.HasFile(it.FileName))
				{
					loadFiles.push_back(it);
				}
			}

			vector<FileInfo> resources;
			for (auto fi : updateFiles) {
				resources.push_back(fi);
			}
			for (auto fi : loadFiles) {
				resources.push_back(fi);
			}

			//Get total size
			unsigned int totalSize = 0;
			for (auto ri : resources) {
				totalSize += ri.FileSize;
			}

			//Download and replace files
			unsigned int totalDownloaded = 0;
			for (auto fi : resources) {
				auto tmpFilePath = System::GetTempFilePath();
				if (!System::DownloadFile(
					System::JoinURI(ServerAPI::ResourceFilesURI(), fi.FileName), tmpFilePath,
					[&totalDownloaded, &totalSize, this](unsigned int downloadedBytes, unsigned int totalBytes)->bool
				{
					totalDownloaded += downloadedBytes;
					unsigned int progress = (int)floor(((float)totalDownloaded / totalSize) * 100);
					if (progress > 99)
						progress = 99;
					this->clbk(0, progress);
					return true;
				}))
				{
					throw exception("Failed to download file");
				}
				auto toPath = System::JoinPath(AppState::GetResourcesPath(), fi.FileName);
				System::MoveFileFromTo(tmpFilePath, toPath);
			}

			for (auto fi : removeFiles) {
				System::RemoveFile(System::JoinPath(AppState::GetResourcesPath(), fi.FileName));
			}

			clbk(0, 100);
		}
		catch (...)
		{
			clbk(1, 0);
		}

	}

};
