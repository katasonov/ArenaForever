#pragma once

#include <string>

#include "System.h"
#include "..\AFCommon\wstrf.h"

using namespace std;

class AppState
{
public:
	static wstring GetResourcesPath()
	{
		auto dir = System::GetRealCurrentDirectory();
		dir = WStrF(L"%s\\baseaf", dir.c_str());
		return dir;
	}
};
