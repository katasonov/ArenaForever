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
		ILogger &logger = *GetLogger();
		logger.PrintLine(L"UpdateResourcesCommand Begin");
		int err = 0;
		int errCounter = 0;
		while (errCounter < 3)
		{
			logger.PrintLine(L"Retry %d", errCounter);
			try
			{
				if (!utils::DirExists(AppState::GetResourcesPath()))
				{
					logger.PrintLine(L"Create resource dir");

					utils::CreateDirectoryRecursively(AppState::GetResourcesPath().c_str(), false);
				}
				logger.PrintLine(L"Get Resource Map");
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
					else if (remote.GetFileInfo(it.FileName) != it)
					{
						//We need to have actual remote file info later
						updateFiles.push_back(remote.GetFileInfo(it.FileName));
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
					logger.PrintLine(L"Update: %s", fi.FileName.c_str());
					resources.push_back(fi);
				}
				for (auto fi : loadFiles) {
					logger.PrintLine(L"New: %s", fi.FileName.c_str());
					resources.push_back(fi);
				}

				//Get total size
				unsigned int totalSize = 0;
				for (auto ri : resources) {
					
					totalSize += ri.FileSize;
				}
				logger.PrintLine(L"Total size: %d", totalSize);

				//Download and replace files
				unsigned int totalDownloaded = 0;
				for (auto fi : resources) 
				{
					auto toPath = System::JoinPath(AppState::GetResourcesPath(), fi.FileName);
					if (utils::FileExists(toPath))
						DeleteFile(toPath.c_str());

					//auto tmpFilePath = System::GetTempFilePath();
					wstring uri = System::JoinURI(ServerAPI::ResourceFilesURI(), fi.FileName);
					logger.PrintLine(L"Download: %s", uri.c_str());
					if (!System::DownloadFile(
						uri, /*tmpFilePath*/toPath,
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


					/*logger.PrintLine(L"Move file from %s to %s", tmpFilePath.c_str(), toPath.c_str());
					System::MoveFileFromTo(tmpFilePath, toPath);*/

					//CHECK hash and size of moved file
					if (fs::file_size(toPath) != fi.FileSize)
					{
						throw exception("Invalid file size. Download is corrupted.");
					}

					if (hashfile(toPath.c_str()) != fi.FileHash)
					{
						throw exception("Invalid file hash. Download is corrupted.");
					}

				}

				for (auto fi : removeFiles) {
					auto path = System::JoinPath(AppState::GetResourcesPath(), fi.FileName);
					logger.PrintLine(L"Removing: %s ", path.c_str());
					System::RemoveFile(path);
				}
				logger.PrintLine(L"OK");
				errCounter = 0;
				break;
			}
			catch (exception ex)
			{
				logger.PrintLine(L"Failed: %s", UTF8ToW(ex.what()).c_str());
			}
			errCounter++;
		}

		if (errCounter > 0) {
			clbk(1, 0);
		}
		else
		{
			clbk(0, 100);
		}

		logger.PrintLine(L"UpdateResourcesCommand End");

	}

};
