#pragma once

#include <string>

class Log
{

private:

	static Log log;

	Log()
	{

	}
public:

	static Log& Instance()
	{
		return log;
	}

	void Println(const std::wstring &line)
	{
		//TODO:
	}

	void Errorln(const std::wstring &line)
	{
		//TODO:
	}
};
