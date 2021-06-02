#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <WinSock2.h>

#define MAX_BUFFER 1024
#define SERVER_PORT 8000

#include <map>

struct HCSocketInfo
{
	WSAOVERLAPPED overlapped;
	WSABUF dataBuf;
	SOCKET socket;
	char messageBuffer[MAX_BUFFER];
	int recvByteSize;
	int sendByteSize;
};

class IOCompletionPort
{
public:
	IOCompletionPort();
	~IOCompletionPort();

	bool Initialize();
	void StartServer();
	bool CreateWorkerThread();
	void WorkerThread();

private:
	std::map<SOCKET, HCSocketInfo> m_socketInfo;
	SOCKET			m_listenSocket;
	HANDLE			m_hIOCP;
	bool			m_bAccept;
	bool			m_bWorkerThread;
	HANDLE*			m_pWorkerHandle;
};

