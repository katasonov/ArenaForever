#ifndef _UTILS_H_1402_
#define _UTILS_H_1402_

#define SECONDS_IN_24_HOURS 60*60*24

#include <Windows.h>

#include <iomanip>

#include <vector>

#include <time.h>

//creates vector with name "name" filled by byte array "data", \0 at the end of data is ignored
#define CREATE_VECTOR_FROM_BYTE_ARRAY(name, data) \
    const char name##Str[] = data; \
    std::vector<unsigned char> name(sizeof(name##Str) - 1); \
    memcpy(&name[0], name##Str, sizeof(name##Str) - 1) 

#define FILL_VECTOR_FROM_BYTE_ARRAY(name, data) \
    const char name##Str[] = data; \
    name.resize(sizeof(name##Str) - 1); \
    memcpy(&name[0], name##Str, sizeof(name##Str) - 1) 

#define FILL_VECTOR_FROM_STRING(vec, str) \
  if (str.size() != 0) \
  { \
      vec.resize(str.size()); \
      memcpy(&vec[0], str.c_str(), vec.size()); \
  } 

#define CREATE_VECTOR_FROM_STRING(vec, str) \
  std::vector<unsigned char> vec; \
  if (str.size() != 0) \
  { \
    vec.resize(str.size()); \
    memcpy(&vec[0], str.c_str(), vec.size()); \
  } 

#define CREATE_XSD__BASE64BINARY(name, str) \
    xsd__base64Binary name; \
    std::vector<unsigned char> name##Vec; \
    if (str.size() != 0) \
    { \
        name##Vec.resize(str.size()); \
        memcpy(&(name##Vec[0]), str.c_str(), name##Vec.size()); \
    } \
    name.__ptr = (name##Vec.size() == 0) ? NULL : &(name##Vec[0]); \
    name.__size = name##Vec.size(); 


namespace utils {

//10 kb
#define MAX_MT_STRING_BYTES 10240


	time_t GetTimeStamp();

    std::wstring WStrF(const wchar_t* format, ...);

    std::string StrF(const char* format, ...);

    std::string GetComputerUID();

    //uses UTF8 as output 
    std::string GetMultibyteCharacterString(const std::wstring& in_str);

    //uses UTF8 as output 
    void GetMultibyteCharacterString(std::string& out_str, const std::wstring& in_str);

    //uses ANSI as output 
    void GetAnsiMultibyteCharacterString(std::string& out_str, const std::wstring& in_str);

    //uses ANSI as output 
    std::string GetAnsiMultibyteCharacterString(const std::wstring& in_str);

    void CopyBytes (std::string &out_str,  const std::wstring& in_str);

    //returns unicode (utf16) string
    std::wstring GetUnicodeCharacterString(const std::string& in_str);

    //faster then previous one
    void GetUnicodeCharacterString(const std::string& in_str, std::wstring &out_str);

    //in_str is utf8 converted to wide character string
    void GetUnicodeFromUTF8(const std::string& in_str, std::wstring &out_str);

    //in_str is utf8 converted to wide character string
    void GetUnicodeFromUTF8 (const unsigned char *in_str, std::wstring &out_str);

    //in_str is utf8 converted to wide character string
    void GetUnicodeFromUTF8(const std::vector<unsigned char>& in_str, std::wstring &out_str);

    //in_str is utf8 converted to wide character string
    std::wstring GetUnicodeFromUTF8(std::string &utf8);
        

    //////////////////
    std::wstring GetBinaryDataUnicodeString(const char* buff, long size);

    ///////////////
    bool ReadFileToMemory(const char* file_name, char** buff, long& file_size);

    bool SetClipboardText(const std::wstring& text);

    ///////////////
    //inserts what_insert in src after after_what. It is not usual strings. Thats data arrays
    bool InsertAfter(const std::vector<unsigned char>& after_what, const std::vector<unsigned char>& what_insert, std::vector<unsigned char>& src);

    ///////////////
    //inserts what_insert in src instead substring. It is not usual strings. Thats data arrays
    bool ReplaceSubstring(const std::vector<unsigned char>& substring, const std::vector<unsigned char>& what_insert, std::vector<unsigned char>& src);

    //inserts what_insert in src instead all accurances of substring. It is not usual strings. Thats data arrays	
    bool ReplaceSubstrings(const std::vector<unsigned char>& substring, const std::vector<unsigned char>& what_insert, std::vector<unsigned char>& src);

	bool DirExists(const std::wstring& dirName_in);
	
	int CreateDirectoryRecursively(const wchar_t *path, bool useCommonAccess = true);

	void SilentlyRemoveDirectory(LPCTSTR dir);

    std::string GenRandomAnsiString(const int len);

    std::wstring GenRandomString(const int len, const wchar_t *alphabet = 0, const int alphabetSize = 0);

    std::string GetFileExtension(std::string &path);

    std::wstring GenerateGUID();

	bool hex2bin(const char* src, unsigned char* target);

	std::string HexStringDummyEncode(const std::string &encStr);

	std::wstring GetUserLocalAppFolder();

    template <typename T>
    inline std::string ConvertBytesToNiceString(T bytes)
    {
        double val = bytes;

        std::ostringstream ss;
        ss << std::fixed << std::setprecision(2);

        bool isgb = val/(1024*1024*1024) > 1;
        if (isgb)
            ss << val/(1024*1024*1024);
        else 
            ss << val/(1024*1024);  
        
        return ss.str();     
    }

    template <typename T>
    inline std::string GetNiceMeasureUTF8StringForBytes(T bytes)
    {
        if (!(bytes/(1024*1024*1024) < 1))
        {
            return utils::GetMultibyteCharacterString(ResourceManager::GetResourceManager().GetStringResource(IDS_GIGABYTE));
        }
        return utils::GetMultibyteCharacterString(ResourceManager::GetResourceManager().GetStringResource(IDS_MEGOBYTE));
    }

    std::wstring GetRealCurrentDirectory();

    bool IsWindowsVistaOrHigher();

    //String should be with end of string character '\0'
    //string starting with pos can contain any non number characters in the begining,
    //they will be skiped.
    bool ReadNextUnsignedIntegerFromBuffer(const char *str, int size, int &pos, unsigned long &num);

    //Run in current set directory
    int RunWindowsProcessAndGetResult(const std::wstring &procName, 
        const std::wstring &cmdLine, std::string &procResult);

    int GetRandomNumberInRange(int from, int to);

    bool ReadNextQuotedStringValueFromBuffer(const std::string& src, int &pos, std::string &value);


    std::vector<unsigned long>
        VectorOfUIntegersFromString(const std::string &str);
    std::string 
        VectorOfUIntegersToString(const std::vector<unsigned long> ints);


	void HBITMAP_to_Pixels(HBITMAP hbmp, int &width, int &height, unsigned char **pixels, BITMAPINFO &bmpInfo);
	bool Pixels_To_HBITMAP(HBITMAP hbmp, unsigned char *pixels, const BITMAPINFO &bmpInfo);


	BOOL CleanDirectory(const TCHAR* sPath);

	bool FileExists(std::wstring filePath);

} // namespace utils

#endif //_UTILS_H_1402_