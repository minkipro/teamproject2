#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <WinSock2.h>
#include <atomic>
class SocketCommunication
{
public:
	bool init();
	~SocketCommunication();
public:
	static void listenFunction();
private:
	static SOCKET _socket; 
	static std::atomic<char> _buffer[1024];
	static bool _exit;
};

