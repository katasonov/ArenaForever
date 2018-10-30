#pragma once


class ILogger
{
public:
	virtual void PrintLine(const wchar_t *fmt, ...) = 0;
};