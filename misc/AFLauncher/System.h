#pragma once

#include <string>
#include <functional>
using namespace std;

class System
{
public:

	static std::wstring GetRealCurrentDirectory();
	static std::wstring GetTempFilePath();
	static bool DownloadFile(const std::wstring &fileUri,
		const std::wstring &toFile, std::function<bool(unsigned int downloaded, unsigned int total)> &&clbk);
	static void MoveFileFromTo(const wstring &from, const wstring &to);
	static wstring JoinPath(const wstring &path1, const wstring &path2);
	static wstring JoinURI(const wstring &path1, const wstring &path2);
	static void RemoveFile(const wstring &path);
};


