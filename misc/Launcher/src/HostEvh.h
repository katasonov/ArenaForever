#ifndef _HOSTEVH_H_300920181544_
#define _HOSTEVH_H_300920181544_


#include <Windows.h>

#include "BaseHost.hpp"

#include <string>

#include "AFServerAppClient.hpp"

#include "Player.h"

using namespace std;


class HostEvh : public SciterEventHandler
{
public:

	virtual bool on_script_call(HELEMENT he, LPCSTR name, UINT argc, const SCITER_VALUE* argv, SCITER_VALUE& retval) override;
};


#endif
