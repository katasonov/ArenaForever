﻿#pragma once

#include "ICommand.h"
#include <functional>
#include <exception>

#include "../System.h"
#include "../AppState.h"

#include "../ServerAPI.h"
#include "../FilesMap.h"

#include <utility>
#include <thread>

using namespace std;

class LaunchGameCommand : public ICommand
{
	std::function<void(int errCode, wstring msg)> clbk;
	wstring authCode, model, sex;
	HWND parentHwnd;

public:

	LaunchGameCommand(HWND parentHwnd, wstring authCode, wstring model, wstring sex, std::function<void(int errCode, wstring msg)> &&_fn)
	{
		this->clbk = move(_fn);
		this->authCode = authCode;
		this->model = model;
		this->sex = sex;
		this->parentHwnd = parentHwnd;
	}

	void Execute() override
	{

		int err = 0xFFFF;
		wstring errMsg = L"Неизвестная ошибка";
		string response;
		try
		{
			response = ServerAPI::GetGoodArena(authCode);
			string arenaID;
			bool needToCreateNewArena = false;

			try
			{ 
				arenaID = JsonValueString("ArenaID", response);
			}
			catch (...)
			{
				needToCreateNewArena = true;
			}
			
			if (needToCreateNewArena)
			{
				//random arena not found
				//Try to create new one
				response = ServerAPI::CreateNewArena(authCode, L"wrackdm17");
				string taskID = JsonValueString("TaskID", response);
				int errCounter = 10;
				while (errCounter > 0)
				{
					Sleep(2000);
					response = ServerAPI::GetCreatedArenaID(taskID);
					try
					{
						arenaID = JsonValueString("ArenaID", response);
						break;
					}
					catch (...)
					{
						errCounter--;
						if (errCounter < 1)
							throw;
						continue;
					}
				}				
			}

			response = ServerAPI::AddPlayerToArena(arenaID, authCode);
			
			wstring IP = UTF8ToW(JsonValueString("IP", response));
			int Port = JsonValueInt("Port", response);
			wstring Payload = UTF8ToW(JsonValueString("Payload", response));
			wstring AccessCode = UTF8ToW(JsonValueString("AccessCode", response));

			wchar_t buffer[4096];
			GetModuleFileName(NULL, buffer, MAX_PATH);
			auto path = wstring(buffer);
			wstring::size_type pos = path.find_last_of(L"\\/");
			path = path.substr(0, pos);
			path = path
				+ L"\\afq3mod.exe";
			wstring args = "+set r_fullscreen 0 +set sv_pure 0 +set r_mode 6 +set cg_drawFPS 1 +set net_remote_ip \"%s\" " \
				L"+set net_remote_port %d +set af_srv_access_code %s +set af_player_data_enc %s";


			std::swprintf(buffer, args.c_str(), IP.c_str(), Port, AccessCode.c_str(), Payload.c_str());
			args = buffer;

			::ShowWindow(parentHwnd, SW_HIDE);
			
			HWND parentHwnd = this->parentHwnd;
			std::thread([path, args, parentHwnd]()
			{
				try
				{
					wstring out;
					System::RunProcessAndGetOutput(path, args, out);
					::ShowWindow(parentHwnd, SW_SHOW);
				}
				catch (...)
				{
					//TODO:
				}
				
			}).detach();
			
			//System::RunProcess(path, args);

			//HWND parentHwnd = this->parentHwnd;

			//Switcher::Instance().RepeatOnTimeout([parentHwnd]()->bool {
			//	//check process q3mod.exe is ended
			//	if (System::CheckProcessExists(L"afq3mod.exe"))
			//	{
			//		return true;
			//	}
			//	//if ended
			//	::ShowWindow(parentHwnd, SW_SHOW);
			//	return false;
			//}, 1000);


			clbk(0, L"");
		}
		catch (...)
		{
			try
			{
				err = JsonValueInt("ErrorCode", response);
				errMsg = UTF8ToW(JsonValueString("Message", response));
			}
			catch (...)
			{
				clbk(err, errMsg);
				return;
			}

			clbk(err, errMsg);
		}
	}

};
