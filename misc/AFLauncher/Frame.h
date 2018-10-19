#pragma once

#include "sciter-x-window.hpp"

#include "Switcher.h"

#include "Commands\UpdateResourcesCommand.h"
#include "Commands\CheckAndDownloadNewAppFilesCommand.h"
#include "Commands\LoginPlayerCommand.h"
#include "Commands\RegisterPlayerCommand.h"
#include "Commands\LaunchGameCommand.h"
#include "Commands\CheckVKOAuthStatusCommand.h"
#include "Commands\LoadPlayerInfoCommand.h"
#include "Commands\SetPlayerNickNameCommand.h"

#include <map>

static sciter::value native_api() 
{

	sciter::value api_map;

	std::function<wstring()> vk_oauth_uri = []() { 
		return AppState::Instance().GetVKOAuthUri();
	};

	api_map.set_item(sciter::value("vk_oauth_uri"), sciter::vfunc(vk_oauth_uri));

	return api_map;
}

// ala Presenter
class Frame : public sciter::window {

	static Frame *frame;
	Frame() : window(SW_TITLEBAR | SW_RESIZEABLE | SW_CONTROLS | SW_MAIN | SW_ENABLE_DEBUG) {}

public:
	
	static Frame& Instance()
	{
		return *frame;
	}

	BEGIN_FUNCTION_MAP
		//FUNCTION_0("helloWorld", helloWorld);
		FUNCTION_0("nativeApi", native_api);
		FUNCTION_0("ntvExit", NtvExit);
		FUNCTION_0("ntvNeedCheckUpdates", NtvNeedCheckUpdates);
		FUNCTION_1("ntvRunAFMvBins", NtvRunAFMvBins);
		FUNCTION_1("ntvResourceUpdate", NtvResourceUpdate);
		FUNCTION_1("ntvCheckAndDownloadAppFiles", NtvCheckAndDownloadAppFiles);
		
		FUNCTION_3("ntvLoginPlayer", NtvLoginPlayer);
		FUNCTION_4("ntvRegisterPlayer", NtvRegisterPlayer);

		FUNCTION_1("ntvLaunchGame", NtvLaunchGame);

		FUNCTION_1("ntvCheckVKOAuthStatus", NtvCheckVKOAuthStatus);



		FUNCTION_1("ntvLoadPlayerInfo", NtvLoadPlayerInfo);

		FUNCTION_2("ntvSetNickName", NtvSetNickName);

		//FUNCTION_1("getResourceEtalonMap", GetResourceEtalonMap);
		//FUNCTION_1("getResourceDirectoryFilesMap", GetResourceDirectoryFilesMap);
	END_FUNCTION_MAP

	//sciter::string  helloWorld() {
	//	return WSTR("Hello u-minimal World");
	//}

	int NtvCheckAndDownloadAppFiles(sciter::value clbk)
	{
#ifndef TURN_OFF_UPDATES
		Switcher::Instance().AddCommand(new CheckAndDownloadNewAppFilesCommand([=](int err, int progress, const wstring &folder)
		{ 
			clbk.call(sciter::value(err), sciter::value(progress), sciter::value(folder));
		}));
#else
		Switcher::Instance().AddCommand([=]()
		{
			clbk.call(sciter::value(0), sciter::value(100), sciter::value(L""));
		});
		
#endif
		return 0;
	}


	int NtvRunAFMvBins(sciter::value folder)
	{
		System::RunProcess(System::JoinPath(AppState::GetAppPath(), L"afmvbins.exe"), folder.to_string());
		return 0;
	}

	sciter::value  NtvNeedCheckUpdates()
	{
		wstring cmdline = GetCommandLine();
		
		return !(cmdline.find(L"-s") != wstring::npos);
		//sciter::value(&etalonMap[0], etalonMap.size())
	}

	int NtvResourceUpdate(sciter::value clbk)
	{
#ifndef TURN_OFF_UPDATES
		Switcher::Instance().AddCommand(new UpdateResourcesCommand([=](int err, int progress)
		{
			clbk.call(sciter::value(err), sciter::value(progress));
		}));
#else
		Switcher::Instance().AddCommand([=]()
		{
			clbk.call(sciter::value(0), sciter::value(100));
		});
#endif
		return 0;
	};

	int NtvExit()
	{
		ExitProcess(0);
		return 0;
	};

	int NtvLoginPlayer(sciter::value email, sciter::value pass, sciter::value clbk)
	{
		Switcher::Instance().AddCommand(new LoginPlayerCommand(
			email.to_string(), pass.to_string(),
			[=](int err)
			{
				clbk.call(sciter::value(err));
			}));

		return 0;
	}

	int NtvRegisterPlayer(sciter::value nick, sciter::value email, sciter::value pass, sciter::value clbk)
	{
		Switcher::Instance().AddCommand(new RegisterPlayerCommand(
			nick.to_string(), email.to_string(), pass.to_string(),
			[=](int err)
		{
			clbk.call(sciter::value(err));
		}));

		return 0;
	}

	int NtvLaunchGame(sciter::value clbk)
	{
		Switcher::Instance().AddCommand(new LaunchGameCommand(get_hwnd(),
			AppState::Instance().GetAuthCode(),
			AppState::Instance().GetPlayerModelName(), 
			AppState::Instance().GetPlayerSex(), 			
			[=](int err)
		{
			clbk.call(sciter::value(err));
		}));

		return 0;
	}

	int NtvCheckVKOAuthStatus(sciter::value clbk)
	{
		Switcher::Instance().AddCommand(new CheckVKOAuthStatusCommand([=](bool logged, int err)
		{
			clbk.call(sciter::value(logged), sciter::value(err));
		}));

		return 0;
	}


	int NtvLoadPlayerInfo(sciter::value clbk)
	{
		Switcher::Instance().AddCommand(new LoadPlayerInfoCommand(
			[=](int err, LoadPlayerInfoCommand::PlayerInfo &playerInfo)
		{
			sciter::value playerMap;

			
			playerMap.set_item("Nick", playerInfo.nickName);
			playerMap.set_item("Score", playerInfo.score);


			clbk.call(sciter::value(err), playerMap);
		}));

		return 0;
	}

	int NtvSetNickName(sciter::value nick, sciter::value clbk)
	{
		Switcher::Instance().AddCommand(new SetPlayerNickNameCommand(
			nick.to_string(),
			[=](int err, const wstring &msg)
		{
			clbk.call(sciter::value(err), sciter::value(msg));
		}));

		return 0;
	}

	//sciter::string ntvGetVKOAuthURI()
	//{
	//	Switcher::Instance().AddCommand(new LaunchGameCommand(get_hwnd(),
	//		AppState::Instance().GetAuthCode(),
	//		AppState::Instance().GetPlayerModelName(),
	//		AppState::Instance().GetPlayerSex(),
	//		[=](int err)
	//	{
	//		clbk.call(sciter::value(err));
	//	}));

	//	return 0;
	//}


};