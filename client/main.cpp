#include <thread>
#include "Server/SocketCommunication.h"
#include "engine.h"
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	/*SocketCommunication server;
	server.Init();
	std::thread t1(SocketCommunication::ListenFunction);*/
	
	Engine engine;
	if (engine.Initialize(hInstance, 800, 600))
	{
		while (engine.ProcessMessages() == true)
		{
			engine.Update();
			engine.RenderFrame();
		}
	}
	//t1.join();
	return 0;
}