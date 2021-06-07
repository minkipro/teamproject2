#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <WinSock2.h>
#include <atomic>
#include <thread>
#include "HCCommunicationProtocol.h"
class SocketCommunication
{
public:
	SocketCommunication();
	~SocketCommunication();
	static SocketCommunication* Get() 
	{
		if (nullptr == instance)
		{
			instance = new SocketCommunication;
		}
		return instance;
	}
	void Update();
	void GetIp(std::vector<unsigned long>& out);
	
private:
	static SocketCommunication* instance;

	void ConnectStart();
	void ListenStart();
	void SendData(char* data, int dataSize);
private:
	std::thread* m_pthread;
	SOCKET m_socket;
	std::atomic<char> m_buffer[MAX_BUFFER] = { 0, };
	bool m_exit;
	std::shared_ptr<IHCTextData> m_textRender;
};


