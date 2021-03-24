#include <thread>
#include "TCPSocket/SocketCommunication.h"
#include "COMException.h"
#include "engine.h"
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	try 
	{
		Engine engine;
		if (engine.Initialize(hInstance, 800, 600))
		{
			while (engine.ProcessMessages() == true)
			{
				engine.Update();
				engine.RenderFrame();
			}
		}
	}
	catch (COMException& error)
	{
		std::wstring error_message = error.what();
		MessageBoxW(NULL, error_message.c_str(), L"Error", MB_ICONERROR);
	}
	
	return 0;
}