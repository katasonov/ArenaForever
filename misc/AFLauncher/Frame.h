#pragma once

#include "sciter-x-window.hpp"

#include "Switcher.h"

#include "Commands\UpdateResourcesCommand.h"


static sciter::value native_api() 
{

	sciter::value api_map;
	sciter::value api_math_map;

	std::function<int(int, int)> native_sum = [](int a, int b) { return a + b; };
	std::function<int(int, int)> native_sub = [](int a, int b) { return a - b; };


	api_math_map.set_item(sciter::value("sum"), sciter::vfunc(native_sum));
	api_math_map.set_item(sciter::value("sub"), sciter::vfunc(native_sub));
	

	api_map.set_item(sciter::value("math"), api_math_map);

	/*
	return {
	math: {
	sum: {native_sum},
	sub: {native_sub},
	}
	}
	*/
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
		FUNCTION_1("ntvResourceUpdate", NtvResourceUpdate);
		//FUNCTION_1("getResourceEtalonMap", GetResourceEtalonMap);
		//FUNCTION_1("getResourceDirectoryFilesMap", GetResourceDirectoryFilesMap);
	END_FUNCTION_MAP

	//sciter::string  helloWorld() {
	//	return WSTR("Hello u-minimal World");
	//}

	int NtvResourceUpdate(sciter::value clbk)
	{
		Switcher::Instance().AddCommand(new UpdateResourcesCommand([=](int err, int progress)
		{
			clbk.call(sciter::value(err), sciter::value(progress));
		}));
		return 0;
	};

	int NtvExit()
	{
		ExitProcess(0);
		return 0;
	};

	/*int GetResourceEtalonMap(sciter::value clbk)
	{
		Switcher::Instance().AddCommand([=]() {

			vector<sciter::value> etalonMap;

			int err = 0;
			try
			{
				void *obj = nullptr;
				string response = ServerAPI::GetResourceUpdatesMap();
				FilesMap remote;
				FilesMap::FromJson(response, remote);

				for (auto item : remote.MapItems)
				{
					sciter::value fmItem;
					fmItem.set_item("name", sciter::value(item.filename.c_str()));
					fmItem.set_item("size", sciter::value((int)item.fileSize));
					fmItem.set_item("hash", sciter::value((int)item.filehash));

					etalonMap.push_back(fmItem);
				}
			
			}
			catch (...)
			{
				err = 1;
			}

			if (etalonMap.size() > 0)
			{
				clbk.call(sciter::value(0), sciter::value(&etalonMap[0], etalonMap.size()));
			}				
			else
			{
				clbk.call(sciter::value(0), sciter::value(NULL));
			}
		});

		return 0;
	};
*/
	//int GetResourceDirectoryFilesMap(sciter::value clbk)
	//{
	//	Switcher::Instance().AddCommand([=]() {

	//		vector<sciter::value> map;

	//		int err = 0;
	//		try
	//		{
	//			auto path = AppState::GetResourcesPath();
	//			for (auto& p : fs::directory_iterator(path))
	//			{
	//				if (fs::is_directory(p.status()))
	//					continue;
	//				auto sz = fs::file_size(p);
	//				wstring name = p.path().filename().wstring();
	//				auto hash = hashfile(p.path().c_str());

	//				sciter::value fmItem;
	//				fmItem.set_item("name", sciter::value(name.c_str()));
	//				fmItem.set_item("size", sciter::value((int)sz));
	//				fmItem.set_item("hash", sciter::value((int)hash));

	//				map.push_back(fmItem);
	//			}

	//		}
	//		catch (...)
	//		{
	//			err = 1;
	//		}

	//		if (map.size() > 0)
	//		{
	//			clbk.call(sciter::value(0), sciter::value(&map[0], map.size()));
	//		}
	//		else
	//		{
	//			clbk.call(sciter::value(0), sciter::value(NULL));
	//		}
	//	});

	//	return 0;
	//};

	//int DownloadResourceFile(sciter::value clbk)
	//{
	//	Switcher::Instance().AddCommand([=]() {
	//					
	//	});

	//	return 0;
	//};

	//int DeleteResourceFile(sciter::value clbk)
	//{
	//	Switcher::Instance().AddCommand([=]() {

	//	});

	//	return 0;
	//};

	//int ReplaceResourceFile(sciter::value clbk)
	//{
	//	Switcher::Instance().AddCommand([=]() {

	//	});

	//	return 0;
	//};

};