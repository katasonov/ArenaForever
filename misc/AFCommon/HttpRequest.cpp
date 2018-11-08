#include <winsock2.h>

#include "HttpRequest.h"
#include <thread>

#include "utils.h"


#define TCP_IO_BUFFER_SIZE  65535


using namespace std;

static unsigned long GetTotalFileLenFromRangeHeader(const string &range)
{
	auto pos = range.find('/');
	return std::stoul(range.substr(pos + 1));
}


string HTTPBody(const char *httpMsg)
{
	auto where = strstr(httpMsg, "\r\n\r\n");
	if (where == nullptr)
		return "";


	where += 4;
	string body;
	while (*(where) != '\0') {		
		body += *(where++);
	}

	return body;
}

int HTTPBodyLength(char *httpMsg)
{
	auto where = strstr(httpMsg, "\r\n\r\n");
	if (where == nullptr)
		return -1;

	
	where += 4;
	int len = 0;
	while (*(where++) != '\0') {
		len++;	
	}

	return len;
}

int HTTPHeaderIntValue(const char *name, const char *httpMsg)
{
	auto where = strstr(httpMsg, "Content-Length:");
	if (where == nullptr)
		return -1;
	const char *ch = where;
	while (*ch != ':' && *ch != '\n') {
		ch++;
	}
	//
	if (*ch != ':') {
		return -1;
	}
	ch++;
	//trim all spaces
	while (*ch == ' ') {
		ch++;
	}
	int val = atoi(ch);

	return val;
}

std::string HTTPHeaderAsString(const string &header, const string &name)
{
	auto start = header.find(name, 0);
	if (start == string::npos)
		throw std::exception("Header not found");
	auto end = header.find("\r\n", start);
	if (end == string::npos)
		throw std::exception("Header corrupted");

	return header.substr(start + name.size() + 1, end - (start + name.size() + 1));
}

unsigned long HTTPHeaderContentLen(const string &header)
{
	
	auto where = strstr(header.c_str(), "Content-Length:");
	if (where == nullptr)
		return -1;
	const char *ch = where;
	while (*ch != ':' && *ch != '\n') {
		ch++;
	}
	//
	if (*ch != ':') {
		return -1;
	}
	ch++;
	//trim all spaces
	while (*ch == ' ') {
		ch++;
	}
	unsigned long val = atoi(ch);

	return val;
}

std::string HttpGetStringResponse(
	std::string host,
	int port,
	std::string url,
	std::string header,
	int waitTimeoutSec)
{
	std::string srvReturn;
	time_t secs;
	secs = time(NULL);

	do {
		WSADATA wsaData;

		int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
		if (iResult != NO_ERROR) {
			break;
		}
		struct hostent *he;
		struct in_addr **addr_list;
		char ip[100];
		string hostName = host;
		if ((he = gethostbyname(hostName.c_str())) == NULL)
		{
			WSACleanup();
			break;
		}

		addr_list = (struct in_addr **) he->h_addr_list;

		for (int i = 0; addr_list[i] != NULL; i++)
		{
			//Return the first one;
			strcpy(ip, inet_ntoa(*addr_list[i]));
		}

		//----------------------
		// Create a SOCKET for connecting to server
		SOCKET ConnectSocket;
		ConnectSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (ConnectSocket == INVALID_SOCKET) {
			wprintf(L"socket function failed with error: %ld\n", WSAGetLastError());
			WSACleanup();
			break;
		}
		//----------------------
		// The sockaddr_in structure specifies the address family,
		// IP address, and port of the server to be connected to.
		sockaddr_in clientService;
		clientService.sin_family = AF_INET;
		clientService.sin_addr.s_addr = inet_addr(ip);
		clientService.sin_port = htons(port);

		int iTimeOut = waitTimeoutSec*1000;
		setsockopt(ConnectSocket, SOL_SOCKET, SO_SNDTIMEO, (char*)&iTimeOut, sizeof(int));
		setsockopt(ConnectSocket, SOL_SOCKET, SO_RCVTIMEO, (char*)&iTimeOut, sizeof(int));

		//----------------------
		// Connect to server.
		iResult = connect(ConnectSocket, (SOCKADDR *)& clientService, sizeof(clientService));
		if (iResult == SOCKET_ERROR) {
			wprintf(L"connect function failed with error: %ld\n", WSAGetLastError());
			iResult = closesocket(ConnectSocket);
			if (iResult == SOCKET_ERROR)
				wprintf(L"closesocket function failed with error: %ld\n", WSAGetLastError());
			WSACleanup();
			break;
		}
		wprintf(L"Connected to server.\n");
		string getCmd = "GET " + url;
		getCmd += " " + header + "\r\n\r\n";
		char sendbuf[1024];
		strcpy(sendbuf, getCmd.c_str());
		//Send an initial buffer
		iResult = send(ConnectSocket, sendbuf, (int)strlen(sendbuf), 0);
		if (iResult == SOCKET_ERROR) {
			wprintf(L"send failed: %d\n", WSAGetLastError());
			closesocket(ConnectSocket);
			WSACleanup();
			break;
		}

		//shutdown the connection for sending since no more data will be sent
		//the client can still use the ConenctSocket for receiving data					
		char recvbuf[TCP_IO_BUFFER_SIZE];
		char *wrbuf = recvbuf;
		int contentLen = -1;
		iResult = 0;
		do {
			iResult = recv(ConnectSocket, wrbuf, TCP_IO_BUFFER_SIZE, 0);
			if (iResult <= 0)
				break;
			//*(wrbuf + iResult)  = '\0';
			wrbuf += iResult;
			*wrbuf = '\0';
			if (contentLen < 0)
				contentLen = HTTPHeaderIntValue("Content-Length", recvbuf);
			int len = HTTPBodyLength(recvbuf);
			if (len >= contentLen && contentLen > 0)
				break;
		} while (iResult > 0);
		*wrbuf = '\0';


		iResult = closesocket(ConnectSocket);
		if (iResult == SOCKET_ERROR) {
			wprintf(L"closesocket function failed with error: %ld\n", WSAGetLastError());
			WSACleanup();
			break;
		}

		WSACleanup();

		srvReturn = recvbuf;
	} while (0);

	return srvReturn;
}


void HttpGetRequest(
	std::string host,
	int port,
	std::string &url,
	const std::string &reqHeader,
	std::string &respHeader,
	std::string &respBody,
	int waitTimeoutSec)
{
	std::string srvReturn;
	time_t secs;
	secs = time(NULL);


	WSADATA wsaData;

	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != NO_ERROR) {
		throw std::exception("Failed to WSAStartup");
	}
	struct hostent *he;
	struct in_addr **addr_list;
	char ip[100];
	string hostName = host;
	if ((he = gethostbyname(hostName.c_str())) == NULL)
	{
		WSACleanup();
		throw std::exception("Failed to get host by name");
	}

	addr_list = (struct in_addr **) he->h_addr_list;

	for (int i = 0; addr_list[i] != NULL; i++)
	{
		//Return the first one;
		strcpy(ip, inet_ntoa(*addr_list[i]));
	}

	//----------------------
	// Create a SOCKET for connecting to server
	SOCKET ConnectSocket;
	ConnectSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (ConnectSocket == INVALID_SOCKET) {
		wprintf(L"socket function failed with error: %ld\n", WSAGetLastError());
		WSACleanup();
		throw std::exception("Failed to create socket");
	}
	//----------------------
	// The sockaddr_in structure specifies the address family,
	// IP address, and port of the server to be connected to.
	sockaddr_in clientService;
	clientService.sin_family = AF_INET;
	clientService.sin_addr.s_addr = inet_addr(ip);
	clientService.sin_port = htons(port);

	int iTimeOut = waitTimeoutSec * 1000;
	setsockopt(ConnectSocket, SOL_SOCKET, SO_SNDTIMEO, (char*)&iTimeOut, sizeof(int));
	setsockopt(ConnectSocket, SOL_SOCKET, SO_RCVTIMEO, (char*)&iTimeOut, sizeof(int));

	//----------------------
	// Connect to server.
	iResult = connect(ConnectSocket, (SOCKADDR *)& clientService, sizeof(clientService));
	if (iResult == SOCKET_ERROR) {
		wprintf(L"connect function failed with error: %ld\n", WSAGetLastError());
		iResult = closesocket(ConnectSocket);
		if (iResult == SOCKET_ERROR)
			wprintf(L"closesocket function failed with error: %ld\n", WSAGetLastError());
		WSACleanup();
		throw std::exception("Failed to connect");
	}
	wprintf(L"Connected to server.\n");
	string getCmd = "GET " + url;
	getCmd += " " + reqHeader + "\r\n\r\n";
	char sendbuf[1024];
	strcpy(sendbuf, getCmd.c_str());
	//Send an initial buffer
	iResult = send(ConnectSocket, sendbuf, (int)strlen(sendbuf), 0);
	if (iResult == SOCKET_ERROR) {
		wprintf(L"send failed: %d\n", WSAGetLastError());
		closesocket(ConnectSocket);
		WSACleanup();
		throw std::exception("Failed to send");
	}

	//shutdown the connection for sending since no more data will be sent
	//the client can still use the ConenctSocket for receiving data					
	char recvbuf[TCP_IO_BUFFER_SIZE];
	int contentLen = -1;
	iResult = 0;
	respHeader = "";
	respBody = "";
	do {
		iResult = recv(ConnectSocket, recvbuf, TCP_IO_BUFFER_SIZE, 0);
		if (iResult <= 0)
			break;

		//TEST:
		//static int errorCounter = 0;
		//errorCounter++;
		//if (!(errorCounter % 200))
		//	throw std::exception("Failed connection!");
		//:TEST

		respBody.append(recvbuf, iResult);
		size_t pos;
		if (respHeader.size() == 0 && (pos = respBody.find("\r\n\r\n", 0)) != std::string::npos)
		{
			respHeader = respBody.substr(0, pos + 4);
			respBody = respBody.substr(pos + 4);
		}

		if (respHeader.size() == 0)
			continue;

		if (contentLen < 0)
		{
			contentLen = HTTPHeaderIntValue("Content-Length", respHeader.c_str());
			if (contentLen < 0)
			{
				closesocket(ConnectSocket);
				WSACleanup();
				throw std::exception("Failed to get Content-Length from response header");
			}
				
		}
		
		if (respBody.size() >= contentLen)
			break;
	} while (true);


	iResult = closesocket(ConnectSocket);
	WSACleanup();
}

void HttpGetFile(
	std::string host,
	int port,
	std::string &url,
	const std::string &reqHeader,
	unsigned long fromByte,
	std::function<bool(const string &data, unsigned long fileSize)> clbkOnData,
	int waitTimeoutSec)
{

	//unsigned long fileSize = 0;

	//string header, chunk;
	//HttpGetRequest(host, port,
	//	url,
	//	utils::StrF("HTTP/1.1\r\nUser-Agent: curl/7.33.0\r\nHost: %s\r\nAccept: */*\r\nRange: bytes=%u-%u",
	//		host.c_str(), fromByte, fromByte),
	//	header,
	//	chunk,
	//	10);


	//auto rangeVal = HTTPHeaderAsString(header, "Content-Range");
	//if (rangeVal.size() == 0)
	//	throw std::exception("HttpGetFile: Failed to find Range header");
	//fileSize = GetTotalFileLenFromRangeHeader(rangeVal);
	//if (fileSize < 1)
	//	throw std::exception("HttpGetFile: Failed to get content from Range header");



	std::string srvReturn;
	time_t secs;
	secs = time(NULL);


	WSADATA wsaData;

	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != NO_ERROR) {
		throw std::exception("Failed to WSAStartup");
	}
	struct hostent *he;
	struct in_addr **addr_list;
	char ip[100];
	string hostName = host;
	if ((he = gethostbyname(hostName.c_str())) == NULL)
	{
		WSACleanup();
		throw std::exception("Failed to get host by name");
	}

	addr_list = (struct in_addr **) he->h_addr_list;

	for (int i = 0; addr_list[i] != NULL; i++)
	{
		//Return the first one;
		strcpy(ip, inet_ntoa(*addr_list[i]));
	}

	//----------------------
	// Create a SOCKET for connecting to server
	SOCKET ConnectSocket;
	ConnectSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (ConnectSocket == INVALID_SOCKET) {
		wprintf(L"socket function failed with error: %ld\n", WSAGetLastError());
		WSACleanup();
		throw std::exception("Failed to create socket");
	}
	//----------------------
	// The sockaddr_in structure specifies the address family,
	// IP address, and port of the server to be connected to.
	sockaddr_in clientService;
	clientService.sin_family = AF_INET;
	clientService.sin_addr.s_addr = inet_addr(ip);
	clientService.sin_port = htons(port);

	int iTimeOut = waitTimeoutSec * 1000;
	setsockopt(ConnectSocket, SOL_SOCKET, SO_SNDTIMEO, (char*)&iTimeOut, sizeof(int));
	setsockopt(ConnectSocket, SOL_SOCKET, SO_RCVTIMEO, (char*)&iTimeOut, sizeof(int));

	//----------------------
	// Connect to server.
	iResult = connect(ConnectSocket, (SOCKADDR *)& clientService, sizeof(clientService));
	if (iResult == SOCKET_ERROR) {
		wprintf(L"connect function failed with error: %ld\n", WSAGetLastError());
		iResult = closesocket(ConnectSocket);
		if (iResult == SOCKET_ERROR)
			wprintf(L"closesocket function failed with error: %ld\n", WSAGetLastError());
		WSACleanup();
		throw std::exception("Failed to connect");
	}
	wprintf(L"Connected to server.\n");
	string getCmd = utils::StrF("GET %s %s\r\nRange: bytes=%u-\r\n\r\n", url.c_str(), reqHeader.c_str(), fromByte);
	//string getCmd = "GET " + url;
	//getCmd += " " + reqHeader + "\r\n\r\n";
	char sendbuf[1024];
	strcpy(sendbuf, getCmd.c_str());
	//Send an initial buffer
	iResult = send(ConnectSocket, sendbuf, (int)strlen(sendbuf), 0);
	if (iResult == SOCKET_ERROR) {
		wprintf(L"send failed: %d\n", WSAGetLastError());
		closesocket(ConnectSocket);
		WSACleanup();
		throw std::exception("Failed to send");
	}

	//shutdown the connection for sending since no more data will be sent
	//the client can still use the ConenctSocket for receiving data					
	char recvbuf[TCP_IO_BUFFER_SIZE];
	int contentLen = -1;
	int totalLen = 0;
	iResult = 0;
	unsigned long totalBodyDownloaded = 0;
	string respHeader = "";
	string respBody = "";
	unsigned long fileSize = 0;
	do {
		iResult = recv(ConnectSocket, recvbuf, TCP_IO_BUFFER_SIZE, 0);
		if (iResult <= 0)
			break;

		//TEST:
		//static int errorCounter = 0;
		//errorCounter++;
		//if (!(errorCounter % 200))
		//	throw std::exception("Failed connection!");
		//:TEST

		respBody.append(recvbuf, iResult);
		size_t pos;
		if (respHeader.size() == 0 && (pos = respBody.find("\r\n\r\n", 0)) != std::string::npos)
		{
			respHeader = respBody.substr(0, pos + 4);
			respBody = respBody.substr(pos + 4);
		}

		if (respHeader.size() == 0)
			continue;

		totalBodyDownloaded += iResult;
		
		if (contentLen < 0)
		{
			contentLen = HTTPHeaderIntValue("Content-Length", respHeader.c_str());
			if (contentLen < 0)
			{
				closesocket(ConnectSocket);
				WSACleanup();
				throw std::exception("Failed to get Content-Length from response header");
			}
		}

		if (fileSize == 0)
		{
			auto rangeVal = HTTPHeaderAsString(respHeader, "Content-Range");
			if (rangeVal.size() == 0)
				throw std::exception("HttpGetFile: Failed to find Range header");
			fileSize = GetTotalFileLenFromRangeHeader(rangeVal);
			if (fileSize < 1)
				throw std::exception("HttpGetFile: Failed to get content from Range header");
		}


		bool needStop = false;
		if (respBody.size() > 65000)
		{
			try {
				needStop = !clbkOnData(respBody, fileSize);
			}
			catch (...)
			{
				closesocket(ConnectSocket);
				WSACleanup();
				throw;
			}
			
			respBody.clear();
		}

		if (totalBodyDownloaded >= contentLen)
			break;

		if (needStop)
			break;

	} while (true);

	if (respBody.size() > 0)
		clbkOnData(respBody, fileSize);


	iResult = closesocket(ConnectSocket);
	WSACleanup();
}


void HttpGetStringResponseAsync(
	std::string host,
	int port,
	std::string url,
	std::string header,
	int waitTimeoutSec,
	std::function<void(std::string response)> callback)
{
	using namespace std;

	thread t(
		[host, port, url, header, waitTimeoutSec, callback]
	{

		auto response = HttpGetStringResponse(host, port, url, header, waitTimeoutSec);

		callback(response);
	});

	t.detach();
}
