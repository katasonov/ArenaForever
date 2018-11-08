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

		ILogger &logger = *GetLogger();

		logger.PrintLine(L"CheckAndDownloadNewAppFilesCommand::Execute() Begin");

		int err = 0;
		int errCounter = 3;

		auto now = std::chrono::system_clock::now();
		wstring uptFolder = L"";


		while (errCounter > 0)
		{
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
						logger.PrintLine(L"New remote file: %s", it.FileName.c_str());
						foundDiff = true;
						break;
					}
					if (remote.GetFileInfo(it.FileName) != local.GetFileInfo(it.FileName))
					{
						logger.PrintLine(L"Diff remote file: %s", it.FileName.c_str());
						foundDiff = true;
						break;
					}
				}

				if (!foundDiff)
				{
					logger.PrintLine(L"No Diff");
					break;
				}

				uptFolder = WStrF(L"up-%d", std::chrono::system_clock::to_time_t(now));
				auto toPath = System::JoinPath(AppState::GetAppPath(), uptFolder);

				//auto tmpFilePath = System::GetTempFilePath();
				auto tmpArchFileName = toPath + L".zip";
				logger.PrintLine(L"Downloading to %s", tmpArchFileName.c_str());

				unsigned int arcBytes = 0;
				ServerAPI::DownloadHttpFile(logger, 
					ServerAPI::AppWin32BinsArchiveURI(), tmpArchFileName,
					[this, &arcBytes](unsigned int downloadedBytes, unsigned int totalBytes)->bool
				{
					if (totalBytes == 0)
						totalBytes = 1;
					arcBytes = totalBytes;
					unsigned int progress = (int)floor(((float)downloadedBytes / totalBytes) * 100);
					if (progress > 99)
						progress = 99;
					this->clbk(0, progress, L"");
					return true;
				});

				logger.PrintLine(L"Checking file size %s", tmpArchFileName.c_str());
				//IE downloads files first to the temp folder then copies it to destination.
				//But we can get to here while file is not copied yet, so check and wait some time
				int waitCounter = 0;
				while (fs::file_size(tmpArchFileName) != arcBytes)
				{
					/*logger.PrintLine(L"Checking retry %d", waitCounter);
					Sleep(1000);
					if (waitCounter++ < 10)
						continue;*/
					throw exception("Invalid file size. Download is corrupted.");
				}

				logger.PrintLine(L"Unzipping to %s", toPath.c_str());
				System::UnZipContentFolder(tmpArchFileName, toPath, L"arenaf");

				logger.PrintLine(L"Deleting to %s", tmpArchFileName.c_str());
				DeleteFile(tmpArchFileName.c_str());

				logger.PrintLine(L"OK");
				break;
			}
			catch (std::exception &ex)
			{
				//TODO: RETRY
				logger.PrintLine(L"Failed: %s", UTF8ToW(ex.what()).c_str());
				
			}
			errCounter--;
		}

		if (errCounter == 0)
		{
			clbk(1, 0, L"");
		}		
		else
		{
			clbk(0, 100, uptFolder);
		}

		logger.PrintLine(L"CheckAndDownloadNewAppFilesCommand::Execute() End");
	}

};
