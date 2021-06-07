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
	int				recvBytes;
	int				sendBytes;
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
	SOCKET			m_listenSocket;		// 서버 리슨 소켓
	HANDLE			m_hIOCP;			// IOCP 객체 핸들
	bool			m_bAccept;			// 요청 동작 플래그
	bool			m_bWorkerThread;	// 작업 스레드 동작 플래그
	HANDLE* m_pWorkerHandle;	// 작업 스레드 핸들
};
