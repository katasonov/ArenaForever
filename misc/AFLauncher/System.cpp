#include "System.h"

#include <Windows.h>
#include <LZExpand.h>
#include <iostream>
#include <functional>
#include "../AFCommon/wstrf.h"

#include "DownloadUpdatesCallback.h"

#include "../AFCommon/unzip.h"

#include <exception>

using namespace std;

bool ExtractZip(const std::wstring &zipFilePath, const std::wstring &wherePath, const string &password)
{
	bool ok = false;

	//char pass[7] = ;
	//memset(pass, 0, password.size());
	////Skrdll
	//pass[0] = 'a';
	//pass[1] = 'r';
	//pass[2] = 'e';
	//pass[3] = 'n';
	//pass[4] = 'a';
	//pass[5] = 'f';
	//pass[6] = '\0';

	HZIP hz = NULL;
	do
	{
		hz = OpenZip(zipFilePath.c_str(), password.c_str());
		ZIPENTRY zeFull;
		ZRESULT zr = GetZipItem(hz, -1, &zeFull);
		if (zr != ZR_OK)
			break;
		for (int i = 0; i < zeFull.index; i++)
		{
			ZIPENTRY ze;
			zr = GetZipItem(hz, i, &ze);
			if (zr != ZR_OK)
				continue;
			if (ze.attr & FILE_ATTRIBUTE_DIRECTORY)
				continue;

			zr = UnzipItem(hz, i, (wherePath + L"\\" + ze.name).c_str());
		}

		ok = true;
	} while (0);

	if (hz != NULL)
		CloseZip(hz);
	return ok;
}


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

void System::RunProcess(const wstring &path, const wstring &args)
{
	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	HANDLE g_hChildStd_OUT_Rd = NULL;
	HANDLE g_hChildStd_OUT_Wr = NULL;

	SECURITY_ATTRIBUTES saAttr;

	saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
	saAttr.bInheritHandle = TRUE;
	saAttr.lpSecurityDescriptor = NULL;



	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	si.cb = sizeof(STARTUPINFO);
	si.hStdError = g_hChildStd_OUT_Wr;
	si.hStdOutput = g_hChildStd_OUT_Wr;
	si.dwFlags |= STARTF_USESTDHANDLES;


	//wstring szCmdline = L"-s";
	//wstring cmdLine = WStrF(L"\"%s\"", args.c_str());
	//WCHAR buff[4096];
	//wcscpy(buff, cmdLine.c_str());
	wstring runPath = WStrF(L"%s %s", path.c_str(), args.c_str());
	_wsystem(runPath.c_str());// "\"d:some path\\program.exe\" \"d:\\other path\\file name.ext\"");
	/*BOOL ret = CreateProcess(path.c_str(), buff,
		NULL,
		NULL,
		TRUE,
		NULL,
		NULL,
		NULL,
		&si,
		&pi);

	if (!ret)
	{
		throw exception("Failed to run process");
	}*/
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

void System::UnZipContentFolder(const wstring &zip, const wstring &to, const string pass)
{
	if (!ExtractZip(zip, to, pass))
	{
		throw std::exception("failed to unzip");
	}
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

