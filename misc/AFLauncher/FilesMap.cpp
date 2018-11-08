#include "FilesMap.h"

bool operator != (const FileInfo &lfi, const FileInfo &rfi)
{
	return !(lfi.FileHash == rfi.FileHash && lfi.FileSize == rfi.FileSize); 
}