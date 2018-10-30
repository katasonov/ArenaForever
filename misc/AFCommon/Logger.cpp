#include "Logger.h"
#include <map>
#include <string>
#include <assert.h>
#include <stdio.h>
#include <stdarg.h> 

using namespace std;


//Logger& Logger::GetInstance()
//{
//	return gLogger;
//}

Logger::Logger(const std::wstring &folder, const std::wstring &logFileName)
{
#ifdef _LOGGING

	m_f = _wfopen((folder + L"\\" + logFileName).c_str(), L"w");
	if (m_f == NULL)
		throw std::exception("Failed to open log file");
	PrintLine(L"=======================BEGIN LOG==============================");
#endif
}

Logger::~Logger()
{
#ifdef _LOGGING
	PrintLine(L"=======================END LOG==============================");
	if (m_f != NULL)
		fclose(m_f);
	
#endif
}


void Logger::PrintLine(const wchar_t *fmt, ...)
{
#ifdef _LOGGING
    va_list ap;
    va_start(ap, fmt);
    vfwprintf(m_f, fmt, ap);
    fwprintf(m_f, L"\n");
    va_end(ap);
	fflush(m_f);
#endif
}
