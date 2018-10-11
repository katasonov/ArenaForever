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
		return WStrF(L"%s\\baseaf", GetAppPath().c_str());
	}

	static wstring GetAppPath()
	{
		return System::GetRealCurrentDirectory();
	}
	
};
