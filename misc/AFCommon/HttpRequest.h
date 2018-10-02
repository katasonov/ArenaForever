#pragma once
#include <string>
#include <functional>

std::string HTTPBody(const char *httpMsg);

std::string HttpGetStringResponse(
	std::string host,
	int port,
	std::string url,	
	std::string header,
	int waitTimeoutSec);

void HttpGetStringResponseAsync(
	std::string host,
	int port,
	std::string url,
	std::string header,
	int waitTimeoutSec,
	std::function<void(std::string response)> callback);

