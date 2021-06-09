#include "IOCompletionPort.h"
#include "COMException.h"

void main()
{
	try
	{
		IOCompletionPort iocp_server;
		if (iocp_server.Initialize())
		{
			iocp_server.StartServer();
		}
	}
	catch (COMException& error)
	{
		std::wstring error_message = error.what();
		MessageBoxW(NULL, error_message.c_str(), L"Error", MB_ICONERROR);
	}
}