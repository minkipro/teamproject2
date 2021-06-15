#pragma once

#include <thread>
#include "HCCommunicationProtocol.h"
#include <queue>

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

private:
	struct RecvDataStruct
	{
		char buffer[MAX_BUFFER];
	};
	std::thread* m_pthread;
	SOCKET m_socket;
	std::queue<RecvDataStruct> m_buffer;
	bool m_exit;
	std::shared_ptr<IHCTextData> m_textRender;
	std::mutex m_mutex;
};



