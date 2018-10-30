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
	std::function<void(wstring &fileName, unsigned int totalSize, unsigned int gotSize)> clbkProgress;
	std::function<void()> clbkDone;
	std::function<void()> clbkFailed;

	//ResourceInfo ParseResourceFileInfo(string jsonInfo)
	//{
	//	ResourceInfo ri;
	//	ri.FileSize = JsonValueInt("FileSize", jsonInfo);
	//	ri.FileName = UTF8ToW(JsonValueString("FileName", jsonInfo));
	//	ri.FileURI = UTF8ToW(JsonValueString("FileURI", jsonInfo));

	//	return ri;
	//}
	

public:

	UpdateResourcesCommand(
		std::function<void(wstring &fileName, unsigned int totalSize, unsigned int gotSize)> &&clbkProgress,
		std::function<void()> &&clbkDone,
		std::function<void()> &&clbkFailed)
	{
		this->clbkProgress = move(clbkProgress);
		this->clbkDone = move(clbkDone);
		this->clbkFailed = move(clbkFailed);
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

				int test = 0;

				//Download and replace files
				unsigned int totalDownloaded = 0;
				for (auto fi : resources)
				{
					int errRetries = 0;
					while (errRetries++ < 3)
					{
						try
						{
							auto toPath = System::JoinPath(AppState::GetResourcesPath(), fi.FileName);
							if (utils::FileExists(toPath))
								DeleteFile(toPath.c_str());

							//auto tmpFilePath = System::GetTempFilePath();
							wstring uri = System::JoinURI(ServerAPI::ResourceFilesURI(), fi.FileName);
							logger.PrintLine(L"Download: %s", uri.c_str());
							unsigned int fileDownloadedBytes = 0;
							if (!System::DownloadFile(
								uri, /*tmpFilePath*/toPath,
								[&fi, &totalDownloaded, &fileDownloadedBytes, &totalSize, this](unsigned int downloadedBytes, unsigned int totalBytes)->bool
							{
								fileDownloadedBytes = totalBytes;
								unsigned int progress = (int)floor(((float)(totalDownloaded + downloadedBytes) / totalSize) * 100);
								if (progress > 100)
									progress = 100;
								this->clbkProgress(fi.FileName, totalBytes, downloadedBytes);
								return true;
							}))
							{
								throw exception("Failed to download file");
							}

							/*logger.PrintLine(L"Move file from %s to %s", tmpFilePath.c_str(), toPath.c_str());
							System::MoveFileFromTo(tmpFilePath, toPath);*/

							if (test++ == 3)
								throw exception("Failed to download file");

							//CHECK hash and size of moved file

							//IE downloads files first to the temp folder then copies it to destination.
							//But we can get to here while file is not copied yet, so check and wait some time
							int waitCounter = 0;
							while (fs::file_size(toPath) != fi.FileSize)
							{
								Sleep(1000);
								if (waitCounter++ < 10)
									continue;
								throw exception("Invalid file size. Download is corrupted.");
							}

							if (hashfile(toPath.c_str()) != fi.FileHash)
							{
								throw exception("Invalid file hash. Download is corrupted.");
							}
							totalDownloaded += fileDownloadedBytes;
						}
						catch (std::exception &ex)
						{
							logger.PrintLine(L"Failed to download:\r\n %s retry %d", UTF8ToW(ex.what()).c_str(), errRetries);
							continue;
						}

						errRetries = 0;
						break; //Stop retries loop
					}

					if (errRetries != 0)
					{
						throw std::exception(StrF("Failed to download %s", WtoUtf8(fi.FileName).c_str()).c_str());
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
			this->clbkFailed();
		}
		else
		{
			clbkDone();
		}

		logger.PrintLine(L"UpdateResourcesCommand End");

	}

};
