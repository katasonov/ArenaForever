
#include "sciter-x-window.hpp"

#include <functional>
#include <thread>
#include "Switcher.h"
#include "Frame.h"

// native API demo

// native functions exposed to script
//static int native_sum(int a, int b) { return a + b; }
//static int native_sub(int a, int b) { return a - b; }

using namespace std;


#include "resources.cpp"

int uimain(std::function<int()> run) 
{
	srand((unsigned int)time(NULL));
	sciter::debug_output_console console;	

	sciter::archive::instance().open(aux::elements_of(resources)); // bind resources[] (defined in "resources.cpp") with the archive

	//Frame *pwin = new Frame();

	// note: this:://app URL is dedicated to the sciter::archive content associated with the application
	Frame::Instance().load(L"this://app/main.htm");
		//pwin->load(WSTR("this://app/index.htm"));
//		thread([=] {
//			Sleep(5000);
////			pwin->call_function("say", sciter::value(L"HeBEBEBEBEBllo"));
//			Frame::Instance().call_function("ConsolePrintLine", sciter::value(L"HeBEBEBEBEBllo"));
//		
//		}).detach();

		//pwin->load( WSTR("file:///home/andrew/Desktop/test.htm") );
		//pwin->expand();
	Frame::Instance().expand();

	Switcher::Instance().Run();

	//UpdatesManager::Check();
	//UpdatesManager::PerformUpdates();

	//GetResourcesUpdatesMapCommand()


	return run();

}
