#pragma once

#include "BaseHost.hpp"
#include "Consts.hpp"
#include "sciter-x.h"
#include "HostEvh.h"

class Host : public BaseHost
{
public:
	HostEvh _evh;
	
	Host(SciterWindow& wnd) : BaseHost(wnd)
	{
		AttachEvh(_evh);
		SetupPage(WSTR("index.html"));
	}
};
