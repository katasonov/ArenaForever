#pragma once

#include "ILogger.h"

#include <string>
#include <time.h>
#include <chrono>

#include "utils.h"

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
			: m_sTime(utils::GetTimeStamp())
			, m_logger(logger)
		{
			m_logger.PrintLine(L"Begin Timer (%s)", id);
		}
	
		~Timer()
		{
			m_logger.PrintLine(L"End Timer: %lu",
				utils::GetTimeStamp() - m_sTime);
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

public:

	static ILogger* CreateLogger(const std::wstring &folder, const std::wstring &prefix)
	{
		DWORD tmsmp = utils::GetTimeStamp();
		return new Logger(folder, utils::WStrF(L"%s%lu", prefix.c_str(), tmsmp));
	}


private:
	time_t m_initTime;
	FILE *m_f;
public:
	Logger(const std::wstring &folder, const std::wstring &logFileName);
	~Logger();


	void PrintLine(const wchar_t *fmt, ...) override;
};