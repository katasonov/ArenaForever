#include <winsock2.h>

#include "HttpRequest.h"
#include <thread>

using namespace std;


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

int HTTPHeaderIntValue(const char *name, char *httpMsg)
{
	auto where = strstr(httpMsg, "Content-Length:");
	if (where == nullptr)
		return -1;
	char *ch = where;
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
		char recvbuf[4096];
		char *wrbuf = recvbuf;
		int contentLen = -1;
		iResult = 0;
		do {
			iResult = recv(ConnectSocket, wrbuf, 4096, 0);
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
