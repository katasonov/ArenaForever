#include "wstrf.h"
#include <Windows.h>
#include <vector>

#include <string>
#include <stdarg.h>



std::wstring WStrF(const wchar_t* format, ...)
{
	va_list arg_list;
	va_start(arg_list, format);

	// SUSv2 version doesn't work for buf NULL/size 0, so try printing
	// into a small buffer that avoids the double-rendering and alloca path too...
	wchar_t short_buf[4096];
	const size_t needed = _vsnwprintf(short_buf, sizeof short_buf,
		format, arg_list) + 1;
	if (needed <= sizeof short_buf)
		return short_buf;

	// need more space...
	wchar_t* p = new wchar_t[needed];
	_vsnwprintf(p, needed, format, arg_list);
	std::wstring retStr(p);
	delete p;
	return retStr;
}

std::string StrF(const char* format, ...)
{
	va_list arg_list;
	va_start(arg_list, format);

	// SUSv2 version doesn't work for buf NULL/size 0, so try printing
	// into a small buffer that avoids the double-rendering and alloca path too...
	char short_buf[4096];
	const size_t needed = _vsnprintf(short_buf, sizeof short_buf,
		format, arg_list) + 1;
	if (needed <= sizeof short_buf)
		return short_buf;

	// need more space...
	char* p = new char[needed];
	_vsnprintf(p, needed, format, arg_list);
	std::string retStr(p);
	delete p;
	return retStr;
}

std::string WtoUtf8(const std::wstring& in_str)
{
	//char out_str_buf[MAX_MT_STRING_BYTES];
	//size_t size = wcstombs(out_str_buf, in_str.c_str(), MAX_MT_STRING_BYTES);
	std::size_t size = ::WideCharToMultiByte(CP_UTF8, 0, in_str.c_str(), -1, NULL, 0, NULL, NULL);
	char *out_str_buf = new char[size];
	::WideCharToMultiByte(CP_UTF8, 0, in_str.c_str(), in_str.length(), out_str_buf, size, NULL, NULL);
	out_str_buf[size - 1] = '\0';

	if (size <= 0)
	{
		return "";
	}
	std::string str(out_str_buf);

	delete[] out_str_buf;
	return str;

}

void GetUtf8String(std::string& out_str, const std::wstring& in_str)
{
	//char out_str_buf[MAX_MT_STRING_BYTES];
	//size_t size = wcstombs(out_str_buf, in_str.c_str(), MAX_MT_STRING_BYTES);
	std::size_t size = ::WideCharToMultiByte(CP_UTF8, 0, in_str.c_str(), -1, NULL, 0, NULL, NULL);
	char *out_str_buf = new char[size];
	::WideCharToMultiByte(CP_UTF8, 0, in_str.c_str(), in_str.length(), out_str_buf, size, NULL, NULL);
	out_str_buf[size - 1] = '\0';

	if (size <= 0)
	{
		return;
	}
	out_str.assign(out_str_buf);

	delete[] out_str_buf;
}

void GetAnsiMultibyteCharacterString(std::string& out_str, const std::wstring& in_str)
{
	//char out_str_buf[MAX_MT_STRING_BYTES];
	//size_t size = wcstombs(out_str_buf, in_str.c_str(), MAX_MT_STRING_BYTES);
	std::size_t size = ::WideCharToMultiByte(CP_ACP, 0, in_str.c_str(), -1, NULL, 0, NULL, NULL);
	char *out_str_buf = new char[size];
	::WideCharToMultiByte(CP_ACP, 0, in_str.c_str(), in_str.length(), out_str_buf, size, NULL, NULL);
	out_str_buf[size - 1] = '\0';

	if (size <= 0)
	{
		return;
	}
	out_str.assign(out_str_buf);

	delete[] out_str_buf;
}

std::string GetAnsiMultibyteCharacterString(const std::wstring& in_str)
{
	std::string ostr;
	GetAnsiMultibyteCharacterString(ostr, in_str);
	return ostr;
}

void CopyBytes(std::string &out_str, const std::wstring& in_str)
{
	std::size_t size = in_str.size() * sizeof(TCHAR);
	//out_str.assign((const char*)(&in_str[0]), size);
	char* buff = new char[size + 1];
	memcpy(buff, &in_str[0], size);
	buff[size - 1] = '\0';
	out_str = std::string(buff);
	delete[] buff;
}

std::wstring GetUnicodeCharacterString(const std::string& in_str)
{
	return std::wstring(in_str.begin(), in_str.end());
}

void GetUnicodeCharacterString(const std::string& in_str, std::wstring &out_str)
{
	out_str.assign(in_str.begin(), in_str.end());
}

void GetUnicodeFromUTF8(const std::string& in_str, std::wstring &out_str)
{
	std::size_t size = ::MultiByteToWideChar(CP_UTF8, 0, in_str.c_str(), -1, NULL, 0);
	wchar_t *out_str_buf = new wchar_t[size];
	::MultiByteToWideChar(CP_UTF8, 0, in_str.c_str(), -1, out_str_buf, size);
	out_str_buf[size - 1] = L'\0';

	if (size <= 0)
	{
		return;
	}
	out_str.assign(out_str_buf);

	delete[] out_str_buf;
}

void GetUnicodeFromUTF8(const unsigned char *in_str, std::wstring &out_str)
{
	std::size_t size = ::MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)in_str, -1, NULL, 0);
	wchar_t *out_str_buf = new wchar_t[size];
	::MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)in_str, -1, out_str_buf, size);
	out_str_buf[size - 1] = L'\0';

	if (size <= 0)
	{
		return;
	}
	out_str.assign(out_str_buf);

	delete[] out_str_buf;
}

void GetUnicodeFromUTF8(const std::vector<unsigned char>& in_str, std::wstring &out_str)
{
	std::string str(in_str.begin(), in_str.end());
	GetUnicodeFromUTF8(str, out_str);
}

std::wstring UTF8ToW(const std::string &utf8)
{
	std::wstring outstr;
	GetUnicodeFromUTF8(utf8, outstr);
	return outstr;
}

std::wstring GetBinaryDataUnicodeString(const char* buff, long size)
{
	std::wstring str;
	//XXX: possible we can lost some bytes by such division
	str.assign((TCHAR*)buff, size / sizeof(TCHAR));

	return str;
}