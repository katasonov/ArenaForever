#include "System.h"

#include <Windows.h>
#include <iostream>
#include <functional>
#include "../AFCommon/wstrf.h"

#include "DownloadUpdatesCallback.h"

using namespace std;

std::string GenRandomAnsiString(const int len)
{
	srand(GetTickCount() ^ (unsigned long)GetDesktopWindow());

	static const char alphanum[] =
		"0123456789"
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz";

	std::string s;
	s.reserve(len);
	for (int i = 0; i < len; ++i)
	{
		s += alphanum[rand() % (sizeof(alphanum) - 1)];
	}

	return s;
}


std::wstring System::GetRealCurrentDirectory()
{
	TCHAR dir[4096];
	::GetModuleFileName(NULL, dir, 4096);
	std::wstring strdir = dir;
	size_t pos = strdir.find_last_of(L'\\');
	strdir = strdir.substr(0, pos);
	return strdir;
}

void System::MoveFileFromTo(const wstring &from, const wstring &to)
{
	if (!MoveFile(
		from.c_str(),
		to.c_str()))
	{
		throw exception("System::MoveFileFromTo");
	}
}

void System::RemoveFile(const wstring &path)
{
	if (!DeleteFile(path.c_str())) {    // delete the file
		//TODO:
	}
}

wstring System::JoinPath(const wstring &path1, const  wstring &path2)
{
	return WStrF(L"%s\\%s", path1.c_str(), path2.c_str());
}

wstring System::JoinURI(const wstring &path1, const  wstring &path2)
{
	return WStrF(L"%s/%s", path1.c_str(), path2.c_str());
}

bool System::DownloadFile(const std::wstring &fileUri, const std::wstring &toFile,
	std::function<bool(unsigned int downloaded, unsigned int total)> &&clbk)
{
	HANDLE downloadFinishedEvent = CreateEvent(
		NULL,         // default security attributes
		TRUE,         // manual-reset event
		FALSE,         // initial state is not signaled
		NULL  // object name
	);


	DownloadUpdatesCallback callback(downloadFinishedEvent, [&clbk](unsigned int downloaded, unsigned int total)->bool 
	{
		return clbk(downloaded, total);
	});

	int err = 1;

	try
	{
		if (S_OK != URLDownloadToFile(NULL, fileUri.c_str(), toFile.c_str(), 0, &callback))
		{
			DWORD err = GetLastError();
			throw std::exception("System::DownloadFile: failed to download");
		}

		WaitForSingleObject(downloadFinishedEvent, INFINITE);

		if (callback.IsDownloadSuccess() == FALSE)
		{
			throw std::exception("System::DownloadFile: not downloaded successfully");
		}
		err = 0;
	}
	catch (...)
	{
	}

	if (downloadFinishedEvent != NULL)
	{
		CloseHandle(downloadFinishedEvent);
	}

	return err == 0;
}

std::wstring System::GetTempFilePath()
{
	wchar_t buff[4096];
	auto len = GetTempPath(4096, buff);
	if (len == 0)
		throw std::exception("System::GetTempFilePath");


	return wstring(buff) + UTF8ToW(GenRandomAnsiString(16));
}

