#ifndef WSTR_H_2509181648
#define WSTR_H_2509181648

#include <string>

std::wstring WStrF(const wchar_t* format, ...);
std::string StrF(const char* format, ...);
std::string WtoUtf8(const std::wstring& in_str);
std::string GetAnsiMultibyteCharacterString(const std::wstring& in_str);

std::wstring GetUnicodeCharacterString(const std::string& in_str);
std::wstring UTF8ToW(std::string &utf8);
std::wstring GetBinaryDataUnicodeString(const char* buff, long size);


#endif //WSTR_H_1648

