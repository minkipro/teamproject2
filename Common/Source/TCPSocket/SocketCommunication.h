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
	template<typename T> void HCSend(T data);
	template<typename T> void HCSend(T data[], unsigned int arrSize);

private:
	struct DataStruct
	{
		char buffer[MAX_BUFFER];
		unsigned int offset = 0;
	};
	std::thread* m_pthread;
	SOCKET m_socket;
	std::queue<DataStruct> m_recvBuffer;
	std::queue<DataStruct> m_tempBuffer;
	std::queue<DataStruct> m_sendBuffer;
	bool m_exit;
	std::shared_ptr<IHCTextData> m_textRender;
	std::mutex m_mutex;
};

template<typename T>
inline void SocketCommunication::HCSend(T data)
{
	char tempBuffer[MAX_BUFFER];
	unsigned int dataSize = HCDataArangeForSend(data, buffer);
	DataStruct& curData = m_sendBuffer.back();
	if (MAX_BUFFER - curData.offset < dataSize)
	{
		m_sendBuffer.push(DataStruct());
		curData = m_sendBuffer.back();
	}
	memcpy_s(&curData.buffer[curData.offset], MAX_BUFFER - curData.offset, tempBuffer, dataSize);
}

template<typename T>
inline void SocketCommunication::HCSend(T data[], unsigned int arrSize)
{
	char tempBuffer[MAX_BUFFER];
	unsigned int dataSize = HCDataArangeForSend(data, arrSize, buffer);
	DataStruct& curData = m_sendBuffer.back();
	if (MAX_BUFFER - curData.offset < dataSize)
	{
		m_sendBuffer.push(DataStruct());
		curData = m_sendBuffer.back();
	}
	memcpy_s(&curData.buffer[curData.offset], MAX_BUFFER - curData.offset, tempBuffer, dataSize);
}
