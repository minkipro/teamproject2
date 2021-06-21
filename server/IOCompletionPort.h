#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <WinSock2.h>

#define	MAX_BUFFER		1024
#define SERVER_PORT		8000

#include <vector>

struct SocketInfo
{
	WSAOVERLAPPED	overlapped;
	WSABUF			dataBuf;
	SOCKET			socket;
	char			messageBuffer[MAX_BUFFER];
	//int				recvBytes;
	//int				sendBytes;
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
	std::vector<SocketInfo*> m_pSocketInfo;		// 소켓 정보
	SOCKET			m_listenSocket;
	HANDLE			m_hIOCP;
	bool			m_bAccept;
	bool			m_bWorkerThread;
	HANDLE* m_pWorkerHandle;
};
