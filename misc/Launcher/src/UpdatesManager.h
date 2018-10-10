#ifndef _UPDATESMANAGER_H_300920181538_
#define _UPDATESMANAGER_H_300920181538_

#include "sciter-x.h"

#include <string>

using namespace std;

enum UpdateCommandType
{
	UpdateCommand_Delete = 0,
	UpdateCommand_Download = 1
};
enum UpdateableResourceType
{
	UpdateCommandType_Resource = 0,
	UpdateCommandType_Main = 1
};

struct FileTableItem
{
	wstring name;
	uint32_t hash;
	uintmax_t size;
};

struct UpdateCommand
{
	UpdateableResourceType type;
	UpdateCommandType cmd;
	wstring sarg0;
	wstring sarg1;
};

class UpdatesManager
{
public:

	vector<UpdateCommand> UpdateCommands;

	static UpdatesManager& Instance();

	static void GetUpdateCommands(
		UpdateableResourceType type,
		vector<FileTableItem> &curTable,
		vector<FileTableItem> &remoteTable,
		vector<UpdateCommand> &cmds);


	void RunAsync(sciter::value printFn);
};

#endif
