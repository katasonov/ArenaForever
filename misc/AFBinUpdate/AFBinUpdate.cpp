#include "stdafx.h"
#include <windows.h>
#include <commctrl.h>
#include "Resource.h"

#include <filesystem>
#include <shlwapi.h>
#include <shellapi.h>

#include <process.h>
#include <Tlhelp32.h>
#include <winbase.h>

#include "../AFCommon/wstrf.h"

#include "../AFCommon/Logger.h"

namespace fs = std::experimental::filesystem;
using namespace std;
//---------------------------------------------------------------------------
HWND hWnd;
HINSTANCE hInst;
LRESULT CALLBACK DlgProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);



std::wstring GetRealCurrentDirectory()
{
	TCHAR dir[4096];
	::GetModuleFileName(NULL, dir, 4096);
	std::wstring strdir = dir;
	size_t pos = strdir.find_last_of(L'\\');
	strdir = strdir.substr(0, pos);
	return strdir;
}


void SilentlyRemoveDirectory(LPCTSTR dir)
{
	SHFILEOPSTRUCT file_op = {
		NULL,
		FO_DELETE,
		dir,
		L"",
		FOF_NOCONFIRMATION |
		FOF_NOERRORUI |
		FOF_SILENT,
		false,
		0,
		L"" };
	SHFileOperation(&file_op);
}

BOOL IsDots(const TCHAR* str) {
	if (_tcscmp(str, L".") && _tcscmp(str, L"..")) return FALSE;
	return TRUE;
}


BOOL CleanDirectory(const TCHAR* sPath) {
	HANDLE hFind;    // file handle
	WIN32_FIND_DATA FindFileData;

	TCHAR DirPath[MAX_PATH];
	TCHAR FileName[MAX_PATH];

	_tcscpy(DirPath, sPath);
	_tcscat(DirPath, L"\\*");    // searching all files
	_tcscpy(FileName, sPath);
	_tcscat(FileName, L"\\");

	// find the first file
	hFind = FindFirstFile(DirPath, &FindFileData);
	if (hFind == INVALID_HANDLE_VALUE) return FALSE;
	_tcscpy(DirPath, FileName);

	bool bSearch = true;
	while (bSearch) {    // until we find an entry
		if (FindNextFile(hFind, &FindFileData)) {
			if (IsDots(FindFileData.cFileName)) continue;
			_tcscat(FileName, FindFileData.cFileName);
			if ((FindFileData.dwFileAttributes &
				FILE_ATTRIBUTE_DIRECTORY)) {

				// we have found a directory, recurse
				if (!CleanDirectory(FileName)) {
					FindClose(hFind);
					return FALSE;    // directory couldn't be deleted
				}
				// remove the empty directory
				RemoveDirectory(FileName);
				_tcscpy(FileName, DirPath);
			}
			else {
				if (FindFileData.dwFileAttributes &
					FILE_ATTRIBUTE_READONLY)
					// change read-only file mode
					_wchmod(FileName, _S_IWRITE);
				if (!DeleteFile(FileName)) {    // delete the file
					FindClose(hFind);
					return FALSE;
				}
				_tcscpy(FileName, DirPath);
			}
		}
		else {
			// no more files there
			if (GetLastError() == ERROR_NO_MORE_FILES)
				bSearch = false;
			else {
				// some error occurred; close the handle and return FALSE
				FindClose(hFind);
				return FALSE;
			}

		}

	}
	FindClose(hFind);                  // close the file handle

									   //return RemoveDirectory(sPath);     // remove the empty directory
	return TRUE;

}

void killProcessByName(const wchar_t *filename)
{
	HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPALL, NULL);
	PROCESSENTRY32 pEntry;
	pEntry.dwSize = sizeof(pEntry);
	BOOL hRes = Process32First(hSnapShot, &pEntry);
	while (hRes)
	{
		if (wcscmp(pEntry.szExeFile, filename) == 0)
		{
			HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, 0,
				(DWORD)pEntry.th32ProcessID);
			if (hProcess != NULL)
			{
				TerminateProcess(hProcess, 9);
				CloseHandle(hProcess);
			}
		}
		hRes = Process32Next(hSnapShot, &pEntry);
	}
	CloseHandle(hSnapShot);
}

void WaitForProcessDied(const wchar_t *filename)
{
	HANDLE hSnapShot = NULL;
	while (true)
	{
		bool found = false;
		HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPALL, NULL);
		PROCESSENTRY32 pEntry;
		pEntry.dwSize = sizeof(pEntry);
		BOOL hRes = Process32First(hSnapShot, &pEntry);
		while (hRes)
		{
			if (wcscmp(pEntry.szExeFile, filename) == 0)
			{
				found = true;
				break;
			}
			hRes = Process32Next(hSnapShot, &pEntry);
		}
		CloseHandle(hSnapShot);
		hSnapShot = NULL;

		if (!found)
			break;

		Sleep(500);
	}
	if (hSnapShot != NULL)
		CloseHandle(hSnapShot);
}


void RunProcess(const wstring &path, const wstring &args)
{
	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	HANDLE g_hChildStd_OUT_Rd = NULL;
	HANDLE g_hChildStd_OUT_Wr = NULL;

	SECURITY_ATTRIBUTES saAttr;

	ZeroMemory(&saAttr, sizeof(SECURITY_ATTRIBUTES));
	memset(&saAttr, 0, sizeof(SECURITY_ATTRIBUTES));
	saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);

	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));


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

//---------------------------------------------------------------------------
INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpCmdLine, int nCmdShow)
{
	wstring curPath = GetRealCurrentDirectory();
	wstring path = WStrF(L"%s\\%s", curPath.c_str(), UTF8ToW(lpCmdLine).c_str());
	Logger logger(WStrF(L"%s\\%s", curPath.c_str(), L"logs"), L"afmvbins.log.txt");

	hInst = hInstance;

	//killProcessByName(L"AFLauncher.exe");
	logger.PrintLine(L"WaitForProcessDied quakal");
	WaitForProcessDied(L"quakal.exe");
	if (UTF8ToW(lpCmdLine).find (L"up-") == wstring::npos) {
		logger.PrintLine(L"Invalid arguments");
		return 0;
	}

	//MessageBox(NULL, UTF8ToW(lpCmdLine).c_str(), L"", MB_OK);
	//DialogBox(hInst, MAKEINTRESOURCE(IDD_CONTROLS_DLG),
	//	hWnd, reinterpret_cast<DLGPROC>(DlgProc));

	wchar_t szFileName[MAX_PATH + 1];

	GetModuleFileName(NULL, szFileName, MAX_PATH + 1);
	wstring name = PathFindFileName(szFileName);	

	logger.PrintLine(L"Going through %s", path.c_str());
	for (auto& p : fs::directory_iterator(path))
	{

		if (fs::is_directory(p.status()))
		{
			logger.PrintLine(L"%s is directory", p.path().filename().c_str());
			continue;
		}
		if (p.path().filename() == name)
			continue;
		
		wstring npath = WStrF(L"%s\\%s", curPath.c_str(), p.path().filename().c_str());
		logger.PrintLine(L"moving from %s to %s", p.path().c_str(), npath.c_str());
		MoveFileEx(p.path().c_str(), npath.c_str(), MOVEFILE_REPLACE_EXISTING);
	}

	logger.PrintLine(L"Cleaning %s", path.c_str());
	CleanDirectory(path.c_str());
	logger.PrintLine(L"Removing %s", path.c_str());
	RemoveDirectory(path.c_str());

	logger.PrintLine(L"Starting: %s", WStrF(L"%s\\%s", curPath.c_str(), L"quakal.exe").c_str());
	RunProcess(WStrF(L"%s\\%s", curPath.c_str(), L"quakal.exe"), L"-s");
	logger.PrintLine(L"Buy!");
	return FALSE;
}
////---------------------------------------------------------------------------
//LRESULT CALLBACK DlgProc(HWND hWndDlg, UINT Msg,
//	WPARAM wParam, LPARAM lParam)
//{
//	INITCOMMONCONTROLSEX InitCtrlEx;
//
//	InitCtrlEx.dwSize = sizeof(INITCOMMONCONTROLSEX);
//	InitCtrlEx.dwICC = ICC_PROGRESS_CLASS;
//	InitCommonControlsEx(&InitCtrlEx);
//
//	switch (Msg)
//	{
//	case WM_INITDIALOG:
//	{
//		HWND hProgress = CreateWindowEx(0, PROGRESS_CLASS, NULL,
//			WS_CHILD | WS_VISIBLE | PBS_SMOOTH,
//			5, 30, 270, 17,
//			hWndDlg, NULL, hInst, NULL);
//		SendMessage(hProgress, PBM_SETRANGE, 0, MAKELPARAM(0, 100));
//		SendMessage(hProgress, PBM_SETPOS, 50, 0);
//		return TRUE;
//	}
//	case WM_COMMAND:
//		switch (wParam)
//		{
//		case IDOK:
//			EndDialog(hWndDlg, 0);
//			return TRUE;
//		case IDCANCEL:
//			EndDialog(hWndDlg, 0);
//			return TRUE;
//		}
//		break;
//	}
//
//	return FALSE;
//}
//---------------------------------------------------------------------------