#pragma once
#include <string>
#include <functional>

std::string HTTPBody(const char *httpMsg);

void HttpGetRequest(
	std::string host,
	int port,
	std::string &url,
	const std::string &reqHeader,
	std::string &respHeader,
	std::string &respBody,
	int waitTimeoutSec);

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


unsigned long HTTPHeaderContentLen(const std::string &header);

std::string HTTPHeaderAsString(const std::string &header, const std::string &name);