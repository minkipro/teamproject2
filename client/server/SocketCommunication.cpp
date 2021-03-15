#include "SocketCommunication.h"
#include <assert.h>
#include "../ErrorLogger.h"
SOCKET SocketCommunication::_socket;
bool SocketCommunication::_exit = false;
std::atomic<char> SocketCommunication::_buffer[1024] = { 0, };
bool SocketCommunication::Init()
{
	const char* fortNum = "24642";			//��Ÿ��븮 fortNum���� ���� �ʿ�.
	const char* ip = "211.192.102.158";		//�� ���� ip

	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		ErrorLogger::Log("���� �ʱ�ȭ ����");

		return false;
	}

	_socket = socket(PF_INET, SOCK_STREAM, 0);
	
	if (_socket == INVALID_SOCKET)
	{
		ErrorLogger::Log("���� ���� ����");
		return false;
	}
	SOCKADDR_IN servAddr = { 0, };
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = inet_addr(ip);
	servAddr.sin_port = htons(atoi(fortNum));

	if (connect(_socket, (SOCKADDR*)&servAddr, sizeof(servAddr)) == SOCKET_ERROR)
	{
		ErrorLogger::Log("���� ���� ����");
		return false;
	}
	return true;
}

SocketCommunication::~SocketCommunication()
{
	closesocket(_socket);
	WSACleanup();
}

void SocketCommunication::ListenFunction()
{
	char buffer[1024] = { 0, };
	while (_exit == false)
	{
		recv(_socket,buffer, 1024, 0);
		for (int i = 0; i < 1024; i++)
		{
			_buffer[i].store(buffer[i]);
		}
	}
}

