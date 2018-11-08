#pragma once

#include <string>
#include "../AFCommon/HttpRequest.h"
#include "../AFCommon/wstrf.h"


using namespace std;

#ifdef LOCAL_SERVER
static string Host = "127.0.0.1";
static int Port = 8889;
#else
static string Host = "api.quaka.online";
static int Port = 80;
#endif

class ServerAPI
{
public:
	static wstring ResourceFilesURI()
	{
		if (Port != 80 || Port != 8080)
		{
			return WStrF(L"http://%s:%d/win32/baseaf", UTF8ToW(Host).c_str(), Port);
		}
		else
		{
			return WStrF(L"http://%s/win32/baseaf", UTF8ToW(Host).c_str(), Port);
		}
		
	}

	static wstring AppWin32BinsArchiveURI()
	{
		if (Port != 80 || Port != 8080)
		{
			return WStrF(L"http://%s:%d/appwin32bins.zip", UTF8ToW(Host).c_str(), Port);
		}
		else
		{
			return WStrF(L"http://%s/appwin32bins.zip", UTF8ToW(Host).c_str(), Port);
		}
	}

	static string GetResourceUpdatesMap()
	{
		auto response = HttpGetStringResponse(Host, Port,
			StrF("/api/v1/static/folder?name=baseaf"),
			string("HTTP/1.1\r\nUser-Agent: curl/7.33.0\r\nHost: ") + Host + "\r\nAccept: */*",
			10);
		auto body = HTTPBody(response.c_str());

		return body;
	}

	static string GetAppBinsUpdatesMap()
	{
		auto response = HttpGetStringResponse(Host, Port,
			StrF("/api/v1/static/folder?name=win32"),
			string("HTTP/1.1\r\nUser-Agent: curl/7.33.0\r\nHost: ") + Host + "\r\nAccept: */*",
			10);
		auto body = HTTPBody(response.c_str());

		return body;
	}

	static string GetLoginPlayer(wstring &email, wstring &pass)
	{
		auto response = HttpGetStringResponse(Host, Port,
			StrF("/api/v1/player/login?email=%s&pass=%s",
				WtoUtf8(email).c_str(),
				WtoUtf8(pass).c_str()),
			string("HTTP/1.1\r\nUser-Agent: curl/7.33.0\r\nHost: ") + Host + "\r\nAccept: */*",
			10);
		auto body = HTTPBody(response.c_str());

		return body;
	}

	static string GeRegisterPlayer(const wstring &nick, const wstring &email, const wstring &pass)
	{
		auto response = HttpGetStringResponse(Host, Port,
			StrF("/api/v1/player/register?email=%s&pass=%s&nick=%s",
				WtoUtf8(email).c_str(),
				WtoUtf8(pass).c_str(),
				WtoUtf8(nick).c_str()),
			string("HTTP/1.1\r\nUser-Agent: curl/7.33.0\r\nHost: ") + Host + "\r\nAccept: */*",
			10);
		auto body = HTTPBody(response.c_str());

		return body;
	}

	static string GetArenaToConnect(const wstring &model, const wstring &sex, const wstring &authCode)
	{
		auto response = HttpGetStringResponse(Host, Port,
			StrF("/api/v1/arena/connect?authCode=%s&model=%s&sex=%s",
				WtoUtf8(authCode).c_str(),
				WtoUtf8(model).c_str(),
				WtoUtf8(sex).c_str()),
			string("HTTP/1.1\r\nUser-Agent: curl/7.33.0\r\nHost: ") + Host + "\r\nAccept: */*",
			10);
		auto body = HTTPBody(response.c_str());

		return body;
	}

	static string CheckVKOAuthStatus(const wstring &checkStatusCode)
	{
		auto response = HttpGetStringResponse(Host, Port,
			StrF("/api/v1/vklogin/status?checkStatusCode=%s",
				WtoUtf8(checkStatusCode).c_str()),
			string("HTTP/1.1\r\nUser-Agent: curl/7.33.0\r\nHost: ") + Host + "\r\nAccept: */*",
			10);
		auto body = HTTPBody(response.c_str());

		return body;
	}

	static string LoadPlayerInfo(const wstring &authCode)
	{
		auto response = HttpGetStringResponse(Host, Port,
			StrF("/api/v1/player/get?authCode=%s",
				WtoUtf8(authCode).c_str()),
			string("HTTP/1.1\r\nUser-Agent: curl/7.33.0\r\nHost: ") + Host + "\r\nAccept: */*",
			10);
		auto body = HTTPBody(response.c_str());

		return body;
	}

	static string SetPlayerNickName(const wstring &authCode, const wstring &nickName)
	{
		string url = StrF("/api/v1/player/setNick?authCode=%s&nick=%s",
			WtoUtf8(authCode).c_str(),
			WtoUtf8(nickName).c_str());
		auto response = HttpGetStringResponse(Host, Port,
			url,
			string("HTTP/1.1\r\nUser-Agent: curl/7.33.0\r\nHost: ") + Host + "\r\nAccept: */*",
			10);
		auto body = HTTPBody(response.c_str());

		return body;
	}


	static void GetFileChunk(const wstring &file, unsigned long fromByte, unsigned bytesCount, string &respHeader, string &respContent)
	{
		string url = StrF("%s",
			WtoUtf8(file).c_str());
		HttpGetRequest(Host, Port,
			url,
			StrF("HTTP/1.1\r\nUser-Agent: curl/7.33.0\r\nHost: %s\r\nAccept: */*\r\nRange: bytes=%u-%u",
				Host.c_str(), fromByte, fromByte + bytesCount - 1),
			respHeader,
			respContent,
			10);

	}

	static unsigned long GetTotalFileLenFromRangeHeader(const string &range)
	{
		auto pos = range.find('/');
		return std::stoul(range.substr(pos + 1));
	}

	static void DownloadHttpFile(ILogger &logger, const std::wstring &fileUri, const std::wstring &toFile,
		std::function<bool(unsigned int downloadedBytes, unsigned int total)> &&clbk)
	{
		//string header, chunk;
		//ServerAPI::GetFileChunk(L"win32/baseaf/pak0.pk3", 0, header, chunk);

		unsigned long totalLen = 0;
		unsigned long downloadedBytes = 0;
		FILE *f = _wfopen(toFile.c_str(), L"wb+");
		if (f == NULL)
		{
			throw std::exception("DownloadHttpFile: Failed to open file for write");
		}
			
		bool ok = false;
		bool canceled = false;

		try 
		{
			while (true)
			{
				//if (canceled)
				//{
				//	throw std::exception("DownloadHttpFile: download canceled");
				//}
				/*string header, chunk;
				try
				{
					unsigned long bytesCount = totalLen - downloadedBytes;
					if (bytesCount % 512000 > 0)
						bytesCount = 512000;
					if (bytesCount == 0)
						bytesCount = 1;
					ServerAPI::GetFileChunk(fileUri, downloadedBytes, bytesCount, header, chunk);
				}
				catch (std::exception &ex)
				{
					logger.PrintLine(L"Failed to get file chunk: %s", UTF8ToW(ex.what()).c_str());
					Sleep(2000);
					continue;
				}


				if (totalLen == 0)
				{
					auto rangeVal = HTTPHeaderAsString(header, "Content-Range");
					if (rangeVal.size() == 0)
						throw std::exception("DownloadHttpFile: Failed to find Range header");
					totalLen = GetTotalFileLenFromRangeHeader(rangeVal);
					if (totalLen < 1)
						throw std::exception("DownloadHttpFile: Failed to get content from Range header");
				}

				downloadedBytes += chunk.size();

				if (fwrite(chunk.c_str(), 1, chunk.size(), f) != chunk.size())
				{
					throw std::exception("DownloadHttpFile: Failed to write chunk to file");
				}
				fflush(f);

				logger.PrintLine(L"====================================\n%s", UTF8ToW(header.c_str()).c_str());


				if (!clbk(downloadedBytes, totalLen))
				{
					throw std::exception("DownloadHttpFile: download canceled");
				}



				if (downloadedBytes == totalLen)
				{
					break;
				}*/

				string url = StrF("%s",
					WtoUtf8(fileUri).c_str());

				try
				{
					HttpGetFile(Host, Port,
						url,
						StrF("HTTP/1.1\r\nUser-Agent: curl/7.33.0\r\nHost: %s\r\nAccept: */*",
							Host.c_str()),
						downloadedBytes,
						[&canceled, f, &downloadedBytes, &totalLen, &clbk](const string &data, unsigned long fileSize) ->bool
					{
						unsigned long written = 0;
						if ((written = fwrite(data.c_str(), 1, data.size(), f)) != data.size())
						{
							throw std::exception("DownloadHttpFile: Failed to write chunk to file");
						}
						fflush(f);
						downloadedBytes += data.size();
						totalLen = fileSize;

						canceled = !clbk(downloadedBytes, totalLen);
						return !canceled;
					},
					10);

				}
				catch (std::exception &ex)
				{
					logger.PrintLine(L"Failed HttpGetFile: %s", UTF8ToW(ex.what()).c_str());
					Sleep(2000);
					continue;
				}

				break;
			}

		}
		catch (...)
		{
			fclose(f);
			throw;
		}


		fclose(f);
	}

};
