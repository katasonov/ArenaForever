#include "hash.h"

#include <stdint.h>
#include <stdio.h>
#include <memory>

uint32_t hashfile(const wchar_t *filename)
{

	FILE *f = _wfopen(filename, L"rb");

	unsigned char buff[65538];
	memset(buff, 0, sizeof(buff));

	// Hashing
	uint32_t magic = 5381;
	int n = 0;
	uint32_t i = 0;
	while ((n = fread(buff, 1, 65538, f)) != 0)
	{
		for (int j = 0; j < n; j++)
			magic = ((magic << 5) + magic) + buff[j]; // magic * 33 + c
		i = i + n;
	}

	fclose(f);
	return  magic;
}

uint32_t hashwstr(const wchar_t *filename)
{
	// Hashing
	uint32_t magic = 5381;
	int n = 0;
	uint32_t i = 0;
	while (true)
	{
		if (*filename == '\0')
			break;
		magic = ((magic << 5) + magic) + *filename; // magic * 33 + c
		filename++;
	}

	return  magic;
}

