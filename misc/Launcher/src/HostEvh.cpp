#include "HostEvh.h"

#include "UpdatesManager.h"

#include "Config.h"

bool HostEvh::on_script_call(HELEMENT he, LPCSTR name, UINT argc, const SCITER_VALUE* argv, SCITER_VALUE& retval)
{

	if (std::string(name) == "CheckResourceUpdateStatus")
	{
		//App::AppWindow
		auto func = argv[0];
		
		AFServerAppClient::Instance().GetCurrentResourcesTableAsync(
			[=](int errCode, vector<FileTableItem> &&remoteTable) {
				if (errCode == 0)
				{
					//compare current folder with remote resource table
					vector<FileTableItem> curTable;
					Config::Instance().GetFolderContentState(Config::Instance().ResourcesPath(), curTable);
					vector<UpdateCommand> updateCommands;
					UpdatesManager::GetUpdateCommands(UpdateCommandType_Resource, curTable, remoteTable, updateCommands);

					if (updateCommands.size() > 0)
					{
						UpdatesManager::Instance().UpdateResourcesCommands = std::move(updateCommands);
						func.call(sciter::value(0), sciter::value(true));
						return;
					}
					func.call(sciter::value(0), sciter::value(false));
				}
				else {
					func.call(sciter::value(errCode), sciter::value(false));
				}
				
		});
		//thread([=]
		//{
		//	Sleep(2000);
		//	auto val = sciter::value(WSTR("Hello World! (from native side)"));
		//	printFn.call(L"asd");
		//	//App::AppHost->call_function("ConsolePrintLine", L"hello");

		//}).detach();

		//UpdatesManager::Instance().RunAsync(printFn);
		//host->call_function("ConsolePrintLine", val);
		//json::value r = sciter::call(hwnd, "namespace.name", p0, p1, ...);
		//Updater updater(func);
		//updater.RunAsync();
		//func.call(L"Loading resources ....");
	}
	else if (std::string(name) == "LoginPlayer")
	{
		auto email = argv[0];
		auto pass = argv[1];
		auto func = argv[2];

		AFServerAppClient::Instance().LoginPlayerAsync(
			email.to_string(),
			pass.to_string(),
			[=](int errCode, wstring authCode) {
			if (errCode == 0)
			{
				Player::Instance().AuthCode = authCode;
			}
			func.call(errCode);
		});
	}
	else if (std::string(name) == "RegisterPlayer")
	{
		auto nick = argv[0];
		auto email = argv[1];
		auto pass = argv[2];
		auto func = argv[3];

		AFServerAppClient::Instance().RegisterNewPlayerAsync(
			nick.to_string(),
			email.to_string(),
			pass.to_string(),
			[=](int errCode, wstring authCode) {
			if (errCode == 0)
			{
				Player::Instance().AuthCode = authCode;
				Player::Instance().Nick = nick.to_string();
				Player::Instance().Email = email.to_string();
			}
			func.call(errCode);
		});
	}
	else if (std::string(name) == "LaunchGame")
	{
		//retval = sciter::value(WSTR("Hello World! (from native side)"));
		auto func = argv[0];
		AFServerAppClient::Instance().ConnectToArenaAsync(Player::Instance().AuthCode,
			Player::Instance().Model,
			Player::Instance().Sex,
			[=](int errCode, wstring srvIp, int port, wstring accessCode, wstring playerDataEnc) {
			if (errCode != 0)
			{
				func.call(errCode);
				return;
			}

			wchar_t buffer[4096];
			GetModuleFileName(NULL, buffer, MAX_PATH);
			auto path = wstring(buffer);
			wstring::size_type pos = path.find_last_of(L"\\/");
			path = path.substr(0, pos);
			path = path
				+ L"\\af.exe +set r_fullscreen 0 +set sv_pure 0 +set r_mode 6 +set cg_drawFPS 1 +set net_remote_ip \"%s\" " \
				L"+set net_remote_port %d +set af_srv_access_code %s +set af_player_data_enc %s";


			std::swprintf(buffer, path.c_str(), srvIp.c_str(), port, accessCode.c_str(), playerDataEnc.c_str());
			path = buffer;

			int result = _wsystem(path.c_str());
			func.call(0);
		});

		return true;
	}
	return false;
}