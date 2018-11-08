#pragma once

#include "../../AFCommon/ILogger.h"

class NoLogger : public ILogger
{
	void PrintLine(const wchar_t *fmt, ...) override
	{

	}
};


class ICommand
{
	NoLogger noLogger;
	ILogger *logger = nullptr;
public:

	ICommand()
	{
		logger = &noLogger;
	}

	virtual void Execute() = 0;
	void SetLogger(ILogger *logger) 
	{
		this->logger = logger;
	}

	ILogger* GetLogger() const
	{
		return logger;
	}
};
