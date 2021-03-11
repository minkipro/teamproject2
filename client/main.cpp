#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <thread>
#include "Server/SocketCommunication.h"
#include "engine.h"
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	/*ourServer server;
	server.init();*/
	//std::thread t1(ourServer::listenFunction);
	
	Engine engine;
	DEVMODE dmScreenSettings;;
	ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);
	//t1.join();
	return 0;
}