//#include "stdafx.h"


//#include <afx.h>

#include <stdlib.h>
#include <fstream>
#include <locale>
#include <stdio.h>
#include <stdarg.h>
#include <vector>
#include <tchar.h>

#include "utils.h"

#include <fstream>
#include <map>

#include <Shlobj.h>
//#include <shlobj_core.h>
#include <shellapi.h>

#include <chrono>

#if _MSC_VER == 1900

FILE _iob[] = { *stdin, *stdout, *stderr };

extern "C" FILE * __cdecl __iob_func(void)
{
	return _iob;
}

#endif

namespace utils {

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



    std::string GetMultibyteCharacterString(const std::wstring& in_str)
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
        
        delete [] out_str_buf;
        return str;

    }

    void GetMultibyteCharacterString(std::string& out_str, const std::wstring& in_str)
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
        
        delete [] out_str_buf;
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
        
        delete [] out_str_buf;
    }

    std::string GetAnsiMultibyteCharacterString(const std::wstring& in_str)
    {
        std::string ostr;
        GetAnsiMultibyteCharacterString(ostr, in_str);
        return ostr;
    }

    void CopyBytes (std::string &out_str,  const std::wstring& in_str)
    {
        std::size_t size = in_str.size()*sizeof(TCHAR);
        //out_str.assign((const char*)(&in_str[0]), size);
        char* buff = new char[size + 1];
        memcpy(buff, &in_str[0], size);
        buff[size - 1] = '\0';
        out_str = std::string(buff);
        delete [] buff;
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
        out_str_buf[size - 1] = _T('\0');

        if (size <= 0)
        {
            return;
        }
        out_str.assign(out_str_buf);
        
        delete [] out_str_buf;
    }

    void GetUnicodeFromUTF8 (const unsigned char *in_str, std::wstring &out_str)
    {
        std::size_t size = ::MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)in_str, -1, NULL, 0);
        wchar_t *out_str_buf = new wchar_t[size];
        ::MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)in_str, -1, out_str_buf, size);
        out_str_buf[size - 1] = _T('\0');

        if (size <= 0)
        {
            return;
        }
        out_str.assign(out_str_buf);
        
        delete [] out_str_buf;
    }

    void GetUnicodeFromUTF8(const std::vector<unsigned char>& in_str, std::wstring &out_str)
    {
        std::string str(in_str.begin(), in_str.end());
        GetUnicodeFromUTF8(str, out_str);
    }

    std::wstring GetUnicodeFromUTF8(std::string &utf8)
    {
        std::wstring outstr;
        GetUnicodeFromUTF8(utf8, outstr);
        return outstr;
    }

    std::wstring GetBinaryDataUnicodeString(const char* buff, long size)
    {
        std::wstring str;
        //XXX: possible we can lost some bytes by such division
        str.assign((TCHAR*)buff, size/sizeof(TCHAR));

        return str;
    }

    bool ReadFileToMemory(const char* file_name, char** buff, long& file_size)
    {
        std::ifstream ifs(file_name, std::ios_base::binary | std::ios_base::in);
        if (!ifs.good() || ifs.eof() || !ifs.is_open()) { return false; }
        ifs.seekg(0, std::ios_base::beg);
        std::ifstream::pos_type begin_pos = ifs.tellg();
        ifs.seekg(0, std::ios_base::end);
        file_size = static_cast<int>(ifs.tellg() - begin_pos);
        //back to begining
        ifs.seekg(0, std::ios_base::beg);
        
        *buff = new char[file_size];
        ifs.read(*buff, file_size);
        ifs.close();
        return true;
    }

    //bool SetClipboardText(const std::wstring& text)
    //{
    //    std::wstring str = text;//GetMultibyteCharacterString(text);
    //    if (AfxGetMainWnd()->OpenClipboard())// если буфер обмена удалось открыть то
    //    {
    //        if (EmptyClipboard())	// Очистить буфер
    //        {
    //            HGLOBAL clipbuffer;
    //            wchar_t * buffer;	// указатель на данные
    //            clipbuffer = GlobalAlloc(GMEM_DDESHARE, 2*str.length() + 2);// выделить память
    //            buffer = (wchar_t*)GlobalLock(clipbuffer);	// блокировать буфер
    //            wcscpy(buffer, LPCTSTR(str.c_str()));	// копировать в него данные
    //            GlobalUnlock(clipbuffer);	// разблокировать
    //            SetClipboardData(CF_UNICODETEXT,clipbuffer);	// установить данные и тип
    //            if (!CloseClipboard()) return false;// закрыть
    //        } else return false;
    //    } else return true;
    //        
    //    return true;
    //}

    bool InsertAfter(const std::vector<unsigned char>& after_what, const std::vector<unsigned char>& what_insert, std::vector<unsigned char>& src)
    {
        int size = after_what.size();
        std::vector<unsigned char>	buf_vec(src.size() + what_insert.size());
        
        if (size > src.size())
            return false;

        if (size == 0)
            return false;

        int i = 0;
        for (i = 0; i < src.size() - size + 1; i++)
        {
            if (0 == memcmp(&src[i], &after_what[0], size))
                break;
        }

        if (i == src.size() - size + 1)
            return false; //not found
        i += size;

        memcpy(&buf_vec[0], &src[0], i);
        memcpy(&buf_vec[i], &what_insert[0], what_insert.size());
        if (i < src.size())
            memcpy(&buf_vec[i + what_insert.size()], &src[i], src.size() - i);
        src.resize(buf_vec.size());
        memcpy(&src[0], &buf_vec[0], buf_vec.size());

        return true;
    }

    bool ReplaceSubstring(const std::vector<unsigned char>& substring, const std::vector<unsigned char>& what_insert, std::vector<unsigned char>& src)
    {
        int size = substring.size();
        std::vector<unsigned char>	buf_vec(src.size() + what_insert.size() - substring.size());
        
        if (size > src.size())
            return false;

        if (size == 0)
            return false;

        int i = 0;
        for (i = 0; i < src.size() - size + 1; i++)
        {
            if (0 == memcmp(&src[i], &substring[0], size))
                break;
        }

        if (i == src.size() - size + 1)
            return false; //not found
        //i += size;

        memcpy(&buf_vec[0], &src[0], i);
        memcpy(&buf_vec[i], &what_insert[0], what_insert.size());
        if (i < src.size())
        {
            i += size;
            memcpy(&buf_vec[i - size + what_insert.size()], &src[i], src.size() - i);
        }
        src.resize(buf_vec.size());
        memcpy(&src[0], &buf_vec[0], buf_vec.size());

        return true;
    }

    bool ReplaceSubstrings(const std::vector<unsigned char>& substring, const std::vector<unsigned char>& what_insert, std::vector<unsigned char>& src)
    {
        int size = substring.size();
        std::vector<unsigned char>	buf_vec;

        if (size > src.size())
            return false;

        if (size == 0)
            return false;

        if (what_insert.size() == 0)
            return false;

        int i	= 0;
        int j	= 0;
        bool replaced = false;
        for (i = 0; i < src.size() - substring.size() + 1; i++)
        {
            if (0 == memcmp(&src[i], &substring[0], size))
            {
                int old_buf_size = buf_vec.size();
                buf_vec.resize(old_buf_size + i - j + what_insert.size());
                memcpy(&buf_vec[old_buf_size], &src[j], i - j);
                memcpy(&buf_vec[old_buf_size + i - j], &what_insert[0], what_insert.size());
                i += size - 1;
                j = i + 1;
                replaced = true;
            }
        }

        if (replaced)
        {
            if (j < src.size())
            {
                int old_buf_size = buf_vec.size();
                buf_vec.resize(old_buf_size + src.size() - j);
                memcpy(&buf_vec[old_buf_size], &src[j], src.size() - j);
            }

            src.clear();
            src.resize(buf_vec.size());
            memcpy(&src[0], &buf_vec[0], buf_vec.size());
        }

        return replaced;
    }

    std::string GenRandomAnsiString(const int len)
    {
        srand(GetTickCount()^(unsigned long)GetDesktopWindow());

        static const char alphanum[] =
            "0123456789"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz";

        std::string s;
        s.reserve(len);
        for (int i = 0; i < len; ++i) 
        {
            s += alphanum[rand() % (sizeof(alphanum) - 1)];
        }

        return s;
    }

    std::string GetFileExtension(std::string &path)
    {
        std::string ext;

        int i = path.size() - 1;
        for (; i >= 0; i--)
        {
            if (path[i] != '.')
                ext += path[i];
            else
                break;
        }

        if (i <= 0)
            ext = "";
        else
            for (i = 0; i < ext.size(); i++)
            {
                char ch = ext[i];
                ext[i] = ext[ext.size()-1-i];
                ext[ext.size()-1-i] = ch;
            }
        return ext;
    }

    std::wstring GetRealCurrentDirectory()
    {
	    TCHAR dir[4096];
	    ::GetModuleFileName(NULL, dir, 4096);
	    std::wstring strdir = dir;
	    size_t pos = strdir.find_last_of(_T('\\'));
	    strdir = strdir.substr(0, pos);	
	    return strdir;
    }

    bool IsWindowsVistaOrHigher() {
       OSVERSIONINFO osvi;
       ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
       osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
       GetVersionEx(&osvi);
       return osvi.dwMajorVersion >= 6;
    }

    std::wstring GenRandomString(const int len, const wchar_t *alphabet, const int alphabetSize) 
    {
        static const wchar_t alphanum[] =
            L"0123456789"
            L"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            L"abcdefghijklmnopqrstuvwxyz";

        std::wstring s;
		int randVal = 0;
        for (int i = 0; i < len; ++i) {
            randVal = rand();
            if (!alphabet)
                s += alphanum[randVal % (sizeof(alphanum)/sizeof(wchar_t) - 1)];
            else
                s += alphabet[randVal % (alphabetSize - 1)];
        }

        return s;
    }


    std::wstring GenerateGUID()
    {
        HRESULT hr;
	    GUID  guid;
	    hr=CoCreateGuid(&guid);
        std::wstring guidStr;
	    LPOLESTR szGUID = new WCHAR [39];

        if (!FAILED(hr) && StringFromGUID2(guid,szGUID,39) > 0)
	    {
            guidStr = szGUID;
	    }
        else {
            guidStr = L"{";
            guidStr += GenRandomString(8, L"0123456789ABCDEF", 16) + L"-";
            guidStr += GenRandomString(4, L"0123456789ABCDEF", 16) + L"-";
            guidStr += GenRandomString(4, L"0123456789ABCDEF", 16) + L"-";
            guidStr += GenRandomString(4, L"0123456789ABCDEF", 16) + L"-";
            guidStr += GenRandomString(12, L"0123456789ABCDEF", 16) + L"}";
        }            

        return guidStr;
    }

	bool DirExists(const std::wstring& dirName_in)
	{
		DWORD ftyp = GetFileAttributes(dirName_in.c_str());
		if (ftyp == INVALID_FILE_ATTRIBUTES)
			return false;  //something is wrong with your path!

		if (ftyp & FILE_ATTRIBUTE_DIRECTORY)
			return true;   // this is a directory!

		return false;    // this is not a directory!
	}

	int CreateDirectoryRecursively(const wchar_t *path, bool useCommonAccess)
	{
		
		if (useCommonAccess)
		{

			SECURITY_ATTRIBUTES sa;
			SECURITY_DESCRIPTOR sd;

			InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION);
			SetSecurityDescriptorDacl(&sd, TRUE, (PACL)NULL, FALSE);

			sa.nLength = sizeof(SECURITY_ATTRIBUTES);
			sa.bInheritHandle = TRUE;
			sa.lpSecurityDescriptor = &sd;			
			return SHCreateDirectoryEx(NULL, path, &sa);
		}
		return SHCreateDirectoryEx(NULL, path, NULL);
	}

    void SilentlyRemoveDirectory(LPCTSTR dir) 
    {
        SHFILEOPSTRUCT file_op = {
            NULL,
            FO_DELETE,
            dir,
            L"",
            FOF_NOCONFIRMATION |
            FOF_NOERRORUI |
            FOF_SILENT,
            false,
            0,
            L"" };
        SHFileOperation(&file_op);
    }


    bool ReadNextUnsignedIntegerFromBuffer(const char *str, int size, int &pos, unsigned long &num)
    {
        int i = pos;
		if (i > size - 1)
			return false;
        std::string val;
        while ((str[i] < '0' || str[i] > '9') && str[i] != '\0' && (i < size))
            ++i;
		if (i > size - 1)
			return false;
		while (str[i] >= '0' && str[i] <= '9' && (i < size))
        {
            val += str[i];
            ++i;
        }

        if (val.size() > 0)
            num = atoi(val.c_str());
        else
            return false;
        pos = i;
        return true;
    }


    int RunWindowsProcessAndGetResult(const std::wstring &procName, 
        const std::wstring &cmdLine, std::string &procResult)
    {
        int err = 1;

        do {
	        STARTUPINFO si;
	        PROCESS_INFORMATION pi;

	        HANDLE g_hChildStd_OUT_Rd = NULL;
	        HANDLE g_hChildStd_OUT_Wr = NULL;

	        SECURITY_ATTRIBUTES saAttr; 

	        saAttr.nLength = sizeof(SECURITY_ATTRIBUTES); 
	        saAttr.bInheritHandle = TRUE; 
	        saAttr.lpSecurityDescriptor = NULL; 

	        if (!CreatePipe(&g_hChildStd_OUT_Rd, &g_hChildStd_OUT_Wr, &saAttr, 0)) 
	        {
		        err = 2;
		        break;
	        }

	        if (!SetHandleInformation(g_hChildStd_OUT_Rd, HANDLE_FLAG_INHERIT, 0))
	        {
		        err = 3;
		        break;
	        }                  

	        ZeroMemory( &si, sizeof(si) );
	        si.cb = sizeof(si);
	        ZeroMemory( &pi, sizeof(pi) );

	        si.cb = sizeof(STARTUPINFO); 
	        si.hStdError = g_hChildStd_OUT_Wr;
	        si.hStdOutput = g_hChildStd_OUT_Wr;
	        si.dwFlags |= STARTF_USESTDHANDLES;

	        WCHAR buff[4096];
            wcscpy( buff, cmdLine.c_str());

            BOOL ret = CreateProcess(procName.c_str(), buff,
		        NULL,
		        NULL,
		        TRUE,
		        CREATE_NO_WINDOW,
		        NULL,
		        NULL,
		        &si,
		        &pi);

	        if (!ret)
	        {
		        err = 4;
		        break;
	        }


	        WaitForSingleObject( pi.hProcess, INFINITE );

	        DWORD exitCode;
	        ret = GetExitCodeProcess(pi.hProcess, &exitCode);

	        CloseHandle( pi.hProcess );
	        CloseHandle( pi.hThread );

	        if (!ret)
	        {
		        err = 5;
		        break;
	        }

	        CHAR chBuf[0x4000];
	        memset(chBuf, '\0', sizeof(chBuf));

	        DWORD dwRead = 0; 
	        BOOL bSuccess = FALSE;

	        bSuccess = ReadFile( g_hChildStd_OUT_Rd, chBuf, 0x4000, &dwRead, NULL);

	        CloseHandle(g_hChildStd_OUT_Rd);
	        CloseHandle(g_hChildStd_OUT_Wr);	

			//if( ! bSuccess || dwRead == 0 ) break;
			if (!(bSuccess && dwRead > 0))
			{
                err = 6;
                break;
			}

    		procResult = chBuf;
            err = 0;

        } while (0);


        return err; 
    }

    //srand must be called before in program.
    int GetRandomNumberInRange(int from, int to)
    {
        return rand() % (to - from) + from;    
    }

    bool ReadNextQuotedStringValueFromBuffer(const std::string& src, int &pos, std::string &value)
    {
        char ch = ' ';
        int len = src.size() - pos;

        while (ch != '"' && ch != '\0' && len > 0)
        {
            ch = src.c_str()[pos++];
            len--;
        }

        if (ch != '"' || len == 0)
            return false;

        ch = ' ';
        while (ch != '"' && ch != '\0' && len > 0)
        {
            ch = src.c_str()[pos++];
            if (ch != '"' && ch != '\0')
                value += ch;
            len--;
        }
        return (ch == '"');
    }

    std::vector<unsigned long> 
        VectorOfUIntegersFromString(const std::string &str)
    {
        int pos = 0;
        unsigned long num = 0;
        std::vector<unsigned long> ints;

        while (ReadNextUnsignedIntegerFromBuffer(str.c_str(), str.size(), pos, num))
        {
            ints.push_back(num);
        }

        return ints;
    }

    std::string 
        VectorOfUIntegersToString(const std::vector<unsigned long> ints)
    {
        std::string intsStr;

        for (int i = 0; i < ints.size(); i++)
        {
            if (i > 0)
                intsStr += ",";
            char buf[32];
            _ultoa(ints[i], buf, 10);
            intsStr += buf;
        }

        return intsStr;
    }

	int char2int(char input)
	{
		if (input >= '0' && input <= '9')
			return input - '0';
		if (input >= 'A' && input <= 'F')
			return input - 'A' + 10;
		if (input >= 'a' && input <= 'f')
			return input - 'a' + 10;
		return -1;
	}

	// This function assumes src to be a zero terminated sanitized string with
	// an even number of [0-9a-f] characters, and target to be sufficiently large
	bool hex2bin(const char* src, unsigned char* target)
	{
		while (*src && src[1])
		{
			int val1 = char2int(*src);
			int val2 = char2int(src[1]);
			if (val1 < 0 || val2 < 0)
				return false;
			*(target++) = val1 * 16 + val2;
			src += 2;
		}
	}

	std::string HexStringDummyEncode(const std::string &encStr)
	{
		std::string encoded = encStr;
		std::map<char, char> encMap = { 
			{ 'A', '0' },{ '0', 'A' },
			{ 'B', '1' },{ '1', 'B' },
			{ 'C', '2' },{ '2', 'C' },
			{ 'D', '3' },{ '3', 'D' },
			{ 'E', '4' },{ '4', 'E' },
			{ 'F', '5' },{ '5', 'F' },
			{ 'G', '6' },{ '6', 'G' },
			{ 'H', '7' },{ '7', 'H' },
			{ 'I', '8' },{ 'I', '8' },
			{ 'J', '9' },{ '9', 'J' }
		};

		for (size_t i = 0; i < encStr.size(); i++)
		{
			encoded.at(i) = encMap[encoded.at(i)];
		}

		return encoded;
	}

	std::wstring GetUserLocalAppFolder()
	{
		std::wstring path;

		wchar_t szPath[MAX_PATH];
		if (FAILED(SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, 0, szPath)))
		{
			return path;
		}

		path = szPath;
		return path;
	}

	void HBITMAP_to_Pixels(HBITMAP hbmp, int &width, int &height, unsigned char **pixels, BITMAPINFO &bmpInfo)
	{
		BITMAP bmp = { 0 };

		HDC dc = CreateCompatibleDC(NULL);
		std::memset(&bmpInfo, 0, sizeof(BITMAPINFO)); //not necessary really..
		HBITMAP oldBitmap = (HBITMAP)SelectObject(dc, hbmp);
		GetObject(hbmp, sizeof(bmp), &bmp);

		bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bmpInfo.bmiHeader.biWidth = width = bmp.bmWidth;
		bmpInfo.bmiHeader.biHeight = height = bmp.bmHeight;
		bmpInfo.bmiHeader.biPlanes = 1;
		bmpInfo.bmiHeader.biBitCount = 32;
		bmpInfo.bmiHeader.biCompression = BI_RGB;
		bmpInfo.bmiHeader.biSizeImage = ((width * 32 + 31) / 32) * 4 * height; //WTF??? 

		*pixels = new unsigned char[bmpInfo.bmiHeader.biSizeImage];
		GetDIBits(dc, hbmp, 0, height, *pixels, &bmpInfo, DIB_RGB_COLORS);
		SelectObject(dc, oldBitmap);

		height = std::abs(height);
		DeleteDC(dc);
	}

	bool Pixels_To_HBITMAP(HBITMAP hbmp, unsigned char *pixels, const BITMAPINFO &bmpInfo)
	{
		bool ok = true;
		HDC DC = CreateCompatibleDC(NULL);
		HBITMAP oldBitmap = (HBITMAP)SelectObject(DC, hbmp);

		if (SetDIBits(DC, hbmp, 0, bmpInfo.bmiHeader.biHeight, pixels, &bmpInfo, DIB_RGB_COLORS) < 1)
		{
			ok = false;
		}

		SelectObject(DC, oldBitmap);
		DeleteDC(DC);

		return ok;
	}

	BOOL IsDots(const TCHAR* str) {
		if (_tcscmp(str, L".") && _tcscmp(str, L"..")) return FALSE;
		return TRUE;
	}

	BOOL CleanDirectory(const TCHAR* sPath) {
		HANDLE hFind;    // file handle
		WIN32_FIND_DATA FindFileData;

		TCHAR DirPath[MAX_PATH];
		TCHAR FileName[MAX_PATH];

		_tcscpy(DirPath, sPath);
		_tcscat(DirPath, L"\\*");    // searching all files
		_tcscpy(FileName, sPath);
		_tcscat(FileName, L"\\");

		// find the first file
		hFind = FindFirstFile(DirPath, &FindFileData);
		if (hFind == INVALID_HANDLE_VALUE) return FALSE;
		_tcscpy(DirPath, FileName);

		bool bSearch = true;
		while (bSearch) {    // until we find an entry
			if (FindNextFile(hFind, &FindFileData)) {
				if (IsDots(FindFileData.cFileName)) continue;
				_tcscat(FileName, FindFileData.cFileName);
				if ((FindFileData.dwFileAttributes &
					FILE_ATTRIBUTE_DIRECTORY)) {

					// we have found a directory, recurse
					if (!CleanDirectory(FileName)) {
						FindClose(hFind);
						return FALSE;    // directory couldn't be deleted
					}
					// remove the empty directory
					RemoveDirectory(FileName);
					_tcscpy(FileName, DirPath);
				}
				else {
					if (FindFileData.dwFileAttributes &
						FILE_ATTRIBUTE_READONLY)
						// change read-only file mode
						_wchmod(FileName, _S_IWRITE);
					if (!DeleteFile(FileName)) {    // delete the file
						FindClose(hFind);
						return FALSE;
					}
					_tcscpy(FileName, DirPath);
				}
			}
			else {
				// no more files there
				if (GetLastError() == ERROR_NO_MORE_FILES)
					bSearch = false;
				else {
					// some error occurred; close the handle and return FALSE
					FindClose(hFind);
					return FALSE;
				}

			}

		}
		FindClose(hFind);                  // close the file handle

		//return RemoveDirectory(sPath);     // remove the empty directory
		return TRUE;

	}

	bool FileExists(std::wstring filePath)
	{
		std::ifstream f(filePath.c_str());
		return f.good();
	}

	time_t GetTimeStamp()
	{
		return (std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()));
	}


}