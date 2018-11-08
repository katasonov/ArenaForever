#include "Host.hpp"
#include "App.hpp"
#include "UpdatesManager.h"

static UpdatesManager updatesManager;

static bool initialized = false;

UpdatesManager& UpdatesManager::Instance()
{
	return updatesManager;
}

void UpdatesManager::GetUpdateCommands(
	UpdateableResourceType type,
	vector<FileTableItem> &curTable,
	vector<FileTableItem> &remoteTable,
	vector<UpdateCommand> &cmds)
{
	//Search for objects that exists in local and changed in remote table
	for (auto &ci : curTable)
	{
		bool found = false;
		for (auto &ri : remoteTable)
		{
			if (ci.name == ri.name)
			{
				found = true;
				if (ci.hash != ri.hash || ci.size != ri.size)
				{
					cmds.push_back(UpdateCommand{ type, UpdateCommand_Delete , ci.name });
					cmds.push_back(UpdateCommand{ type, UpdateCommand_Download , ci.name });
				}
			}
		}

		if (!found)
		{
			cmds.push_back(UpdateCommand{ type, UpdateCommand_Delete , ci.name });
		}
	}
	//search for objects that exists in remote and no in local
	for (auto &ri : remoteTable)
	{
		bool found = false;
		for (auto &ci : curTable)
		{
			if (ci.name == ri.name)
			{
				found = true;
			}
		}

		if (!found)
		{
			cmds.push_back(UpdateCommand{ type, UpdateCommand_Download , ri.name });
		}
	}


}

void UpdatesManager::RunAsync(sciter::value printFn)
{
	thread([=]
	{
		Sleep(2000);
		//printFn.call(L"Проверяем наличие новых ресурсов для загрузки");
		//sciter.call
		Sleep(2000);
		printFn.call(L"Найдены новые ресурсы");
		printFn.call(L"Загружаем: res.pak");
		Sleep(2000);
		printFn.call(L"Проверяем наличие обновлений програмного кода");
		Sleep(2000);
		printFn.call(L"Обновления доступны для скачивания");
		printFn.call(L"Скачиваем");
		//App::AppHost->call_function("ConsolePrintLine", L"hello");
		
	}).detach();
	//Sleep(2000);
	//printFn.call(L"Hello");
}
