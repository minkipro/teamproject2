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

public:
	template<typename T> void SendData(T dataArr[], size_t arrSize);

private:
	std::thread* m_pthread;
	SOCKET m_socket;
	std::atomic<char> m_buffer[MAX_BUFFER] = { 0, };
	bool m_exit;
	std::shared_ptr<IHCTextData> m_textRender;
};

template<typename T>
void SocketCommunication::SendData(T dataArr[], size_t arrSize)
{
	HCTypeEnum dataType;
	if (typeid(T) == typeid(char))
	{
		dataType = HCTypeEnum::HCchar;
	}
	else if (typeid(T) == typeid(int))
	{
		dataType = HCTypeEnum::HCint;
	}
	else if (typeid(T) == typeid(float))
	{
		dataType = HCTypeEnum::HCfloat;
	}
	else if (typeid(T) == typeid(double))
	{
		dataType = HCTypeEnum::HCdouble;
	}
	std::vector<char> dataBuffer;
	size_t bufferSize = arrSize * sizeof(T) + sizeof(HCTypeEnum);
	dataBuffer.resize(bufferSize);
	memcpy_s(&dataBuffer[0], dataBuffer.size(), &dataType, sizeof(HCTypeEnum));
	memcpy_s(&dataBuffer[0] + sizeof(HCTypeEnum), dataBuffer.size() - sizeof(HCTypeEnum), dataArr, sizeof(T) * arrSize);
	send(m_socket, &dataBuffer[0], SizeTTransINT(bufferSize), 0);
}


