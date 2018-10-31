#pragma once

#include "ILogger.h"

#include <string>
#include <time.h>
#include <chrono>

class Logger : public ILogger
{
public:



	class Timer
	{
		#include "time.h"

		time_t m_sTime;
		Logger &m_logger;
	public:

		
#ifdef _LOGGING
		Timer(const wchar_t *id, Logger &logger)
			: m_sTime(std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()))
			, m_logger(logger)
		{
			m_logger.PrintLine(L"Begin Timer (%s)", id);
		}
	
		~Timer()
		{
			m_logger.PrintLine(L"End Timer: %lu",
				std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()) - m_sTime);
		}
#else
		Timer(const wchar_t *id, Logger &logger)
			: m_logger(logger)
		{
		}
	
		~Timer()
		{
		}
#endif
	};


private:
	time_t m_initTime;
	FILE *m_f;
public:
	Logger(const std::wstring &folder, const std::wstring &logFileName);
	~Logger();


	void PrintLine(const wchar_t *fmt, ...) override;
};