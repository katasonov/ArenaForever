#include "System.h"

#include <Windows.h>
#include <LZExpand.h>
#include <iostream>
#include <functional>
#include "../AFCommon/wstrf.h"

#include "DownloadUpdatesCallback.h"

#include <exception>

#include <process.h>
#include <Tlhelp32.h>
#include <winbase.h>


using namespace std;


bool ExtractZip(const std::wstring &zipFilePath, const std::wstring &wherePath, const wstring &password)
{
	bool ok = false;

	wstring out;
	System::RunProcessAndGetOutput(System::JoinPath(System::GetRealCurrentDirectory(), L"7za.exe"),
		WStrF(L"e -o\"%s\" -p%s %s", wherePath.c_str(), password.c_str(), zipFilePath.c_str()), out);

	if (out.find(L"Everything is Ok") == string::npos)
		return false;

	return true;
}

//
//bool ExtractZip(const std::wstring &zipFilePath, const std::wstring &wherePath, const string &password)
//{
//	bool ok = false;
//
//	//char pass[7] = ;
//	//memset(pass, 0, password.size());
//	////Skrdll
//	//pass[0] = 'a';
//	//pass[1] = 'r';
//	//pass[2] = 'e';
//	//pass[3] = 'n';
//	//pass[4] = 'a';
//	//pass[5] = 'f';
//	//pass[6] = '\0';
//
//	HZIP hz = NULL;
//	do
//	{
//		hz = OpenZip(zipFilePath.c_str(), password.c_str());
//		ZIPENTRY zeFull;
//		ZRESULT zr = GetZipItem(hz, -1, &zeFull);
//		if (zr != ZR_OK)
//			break;
//		for (int i = 0; i < zeFull.index; i++)
//		{
//			ZIPENTRY ze;
//			zr = GetZipItem(hz, i, &ze);
//			if (zr != ZR_OK)
//				continue;
//			if (ze.attr & FILE_ATTRIBUTE_DIRECTORY)
//				continue;
//
//			zr = UnzipItem(hz, i, (wherePath + L"\\" + ze.name).c_str());
//		}
//
//		ok = true;
//	} while (0);
//
//	if (hz != NULL)
//		CloseZip(hz);
//	return ok;
//}


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

void System::RunProcessAndGetOutput(const wstring &path, const wstring &args, wstring &out)
{
	bool ok = false;

	do {

		STARTUPINFO si;
		PROCESS_INFORMATION pi;

		HANDLE g_hChildStd_OUT_Rd = NULL;
		HANDLE g_hChildStd_OUT_Wr = NULL;

		SECURITY_ATTRIBUTES saAttr;

		saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
		saAttr.bInheritHandle = TRUE;
		saAttr.lpSecurityDescriptor = NULL;

		if (!CreatePipe(&g_hChildStd_OUT_Rd, &g_hChildStd_OUT_Wr, &saAttr, 0))
		{
			break;
		}

		if (!SetHandleInformation(g_hChildStd_OUT_Rd, HANDLE_FLAG_INHERIT, 0))
		{
			break;
		}

		ZeroMemory(&si, sizeof(si));
		si.cb = sizeof(si);
		ZeroMemory(&pi, sizeof(pi));

		si.cb = sizeof(STARTUPINFO);
		si.hStdError = g_hChildStd_OUT_Wr;
		si.hStdOutput = g_hChildStd_OUT_Wr;
		si.dwFlags |= STARTF_USESTDHANDLES;

		wstring cmdline = WStrF(L"\"%s\" %s", path.c_str(), args.c_str());

		WCHAR buff[1<<12];
		wcscpy(buff, cmdline.c_str());

		BOOL ret = CreateProcess(NULL, buff,
			NULL,
			NULL,
			TRUE,
			CREATE_NO_WINDOW,
			NULL,
			NULL,
			&si,
			&pi);

		if (!ret)
		{
			break;
		}


		WaitForSingleObject(pi.hProcess, INFINITE);

		DWORD exitCode;
		ret = GetExitCodeProcess(pi.hProcess, &exitCode);

		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);

		if (!ret)
		{
			break;
		}

		CHAR chBuf[0x4000];
		memset(chBuf, '\0', sizeof(chBuf));

		DWORD dwRead = 0;
		BOOL bSuccess = FALSE;

		//for (;;) 
		//{
		bSuccess = ReadFile(g_hChildStd_OUT_Rd, chBuf, 0x4000, &dwRead, NULL);

		CloseHandle(g_hChildStd_OUT_Rd);
		CloseHandle(g_hChildStd_OUT_Wr);

		//if( ! bSuccess || dwRead == 0 ) break;
		if (bSuccess && dwRead > 0)
		{
			out = UTF8ToW(chBuf);			
		}
		else
		{
			break;
		}

		ok = true;

	} while (0);

	// Wait until child process exits.
	//WaitForSingleObject( pi.hProcess, INFINITE );

	if (!ok) 
	{
		throw exception("System::RunProcessAndGetOutput failed");
	}
}

void System::RunProcess(const wstring &path, const wstring &args)
{
	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	HANDLE g_hChildStd_OUT_Rd = NULL;
	HANDLE g_hChildStd_OUT_Wr = NULL;

	SECURITY_ATTRIBUTES saAttr;

	ZeroMemory(&saAttr, sizeof(SECURITY_ATTRIBUTES));
	memset(&saAttr, 0, sizeof(SECURITY_ATTRIBUTES));
	saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
	//saAttr.bInheritHandle = TRUE;
	//saAttr.lpSecurityDescriptor = NULL;



	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	//si.hStdError = g_hChildStd_OUT_Wr;
	//si.hStdOutput = g_hChildStd_OUT_Wr;
	//si.dwFlags |= STARTF_USESTDHANDLES;


	
	wstring cmdLine = WStrF(L"\"%s\" %s", path.c_str(), args.c_str());
	WCHAR buff[4096];
	wcscpy(buff, cmdLine.c_str());

	BOOL ret = CreateProcess(NULL, buff,
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

void System::UnZipContentFolder(const wstring &zip, const wstring &to, const wstring pass)
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
		wstring randomizedUrl = (fileUri + L"?" + UTF8ToW(GenRandomAnsiString(16)));
		if (S_OK != URLDownloadToFile(NULL, randomizedUrl.c_str(), toFile.c_str(), 0, &callback))
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

bool System::CheckProcessExists(wstring procName)
{
	bool found = false;
	HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPALL, NULL);
	PROCESSENTRY32 pEntry;
	pEntry.dwSize = sizeof(pEntry);
	BOOL hRes = Process32First(hSnapShot, &pEntry);
	while (hRes)
	{
		if (wcscmp(pEntry.szExeFile, procName.c_str()) == 0)
		{
			found = true;
			break;
		}
		hRes = Process32Next(hSnapShot, &pEntry);
	}



	if (hSnapShot != NULL)
		CloseHandle(hSnapShot);

	return found;
}