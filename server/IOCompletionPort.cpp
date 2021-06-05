#include "IOCompletionPort.h"
#include <process.h>
#include <iostream>
#include "TCPSocket/HCCommunicationProtocol.h"

unsigned int WINAPI CallWorkerThread(LPVOID p)
{
	IOCompletionPort* pOverlappedEvent = (IOCompletionPort*)p;
	pOverlappedEvent->WorkerThread();
	return 0;
}

IOCompletionPort::IOCompletionPort()
{
	m_bWorkerThread = true;
	m_bAccept = true;
	m_listenSocket = INVALID_SOCKET;
	m_hIOCP = INVALID_HANDLE_VALUE;
	m_pWorkerHandle=nullptr;
}


IOCompletionPort::~IOCompletionPort()
{
	WSACleanup();
	for (auto& it : m_socketInfo)
	{
		closesocket(it.first);
	}
	m_socketInfo.clear();

	if (m_pWorkerHandle)
	{
		delete[] m_pWorkerHandle;
		m_pWorkerHandle = NULL;
	}

	closesocket(m_listenSocket);
	WSACleanup();
}

bool IOCompletionPort::Initialize()
{
	WSADATA wsaData;
	int nResult;
	nResult = WSAStartup(MAKEWORD(2, 2), &wsaData);

	if (nResult != 0)
	{
		printf_s("[ERROR] winsock 초기화 실패\n");
		return false;
	}

	m_listenSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (m_listenSocket == INVALID_SOCKET)
	{
		printf_s("[ERROR] 소켓 생성 실패\n");
		return false;
	}

	SOCKADDR_IN serverAddr;
	serverAddr.sin_family = PF_INET;
	serverAddr.sin_port = htons(SERVER_PORT);
	serverAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

	nResult = bind(m_listenSocket, (struct sockaddr*)&serverAddr, sizeof(SOCKADDR_IN));
	if (nResult == SOCKET_ERROR)
	{
		printf_s("[ERROR] bind 실패\n");
		closesocket(m_listenSocket);
		WSACleanup();
		return false;
	}

	nResult = listen(m_listenSocket, 5);
	if (nResult == SOCKET_ERROR)
	{
		printf_s("[ERROR] listen 실패\n");
		closesocket(m_listenSocket);
		WSACleanup();
		return false;
	}

	return true;
}

void IOCompletionPort::StartServer()
{
	int nResult;
	SOCKADDR_IN clientAddr;
	int addrLen = sizeof(SOCKADDR_IN);
	SOCKET clientSocket;
	DWORD recvBytes;
	DWORD flags;

	m_hIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	if (!CreateWorkerThread()) return;
	printf_s("[INFO] 서버 시작...\n");

	while (m_bAccept)
	{
		clientSocket = WSAAccept(
			m_listenSocket, (struct sockaddr*)&clientAddr, &addrLen, NULL, NULL
		);

		if (clientSocket == INVALID_SOCKET)
		{
			printf_s("[ERROR] Accept 실패\n");
			return;
		}
		HCSocketInfo& curSocketInfo = m_socketInfo[clientSocket];
		curSocketInfo.recvByteSize = 0;
		curSocketInfo.sendByteSize = 0;
		curSocketInfo.dataBuf.len = MAX_BUFFER;
		curSocketInfo.dataBuf.buf = curSocketInfo.messageBuffer;
		flags = 0;

		CreateIoCompletionPort(
			(HANDLE)clientSocket, m_hIOCP, (ULONG_PTR)&curSocketInfo, 0
		);

		// 중첩 소켓을 지정하고 완료시 실행될 함수를 넘겨줌
		nResult = WSARecv(
			clientSocket,
			&curSocketInfo.dataBuf,
			1,
			&recvBytes,
			&flags,
			&(curSocketInfo.overlapped),
			NULL
		);

		if (nResult == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING)
		{
			printf_s("[ERROR] IO Pending 실패 : %d", WSAGetLastError());
			return;
		}
	}

}

bool IOCompletionPort::CreateWorkerThread()
{
	unsigned int threadId;
	SYSTEM_INFO sysInfo;
	GetSystemInfo(&sysInfo);
	printf_s("[INFO] CPU 갯수 : %d\n", sysInfo.dwNumberOfProcessors);
	// 적절한 작업 스레드의 갯수는 (CPU * 2) + 1
	int nThreadCnt = sysInfo.dwNumberOfProcessors * 2;

	m_pWorkerHandle = new HANDLE[nThreadCnt];
	for (int i = 0; i < nThreadCnt; i++)
	{
		m_pWorkerHandle[i] = (HANDLE*)_beginthreadex(
			NULL, 0, &CallWorkerThread, this, CREATE_SUSPENDED, &threadId
		);
		if (m_pWorkerHandle[i] == NULL)
		{
			printf_s("[ERROR] Worker Thread 생성 실패\n");
			return false;
		}
		ResumeThread(m_pWorkerHandle[i]);
	}
	printf_s("[INFO] Worker Thread 시작...\n");
	return true;
}

void IOCompletionPort::WorkerThread()
{
	BOOL	bResult;
	int		nResult;
	DWORD	recvByteSize;
	DWORD	sendByteSize;
	// Completion Key를 받을 포인터 변수
	HCSocketInfo* pCompletionKey;
	// I/O 작업을 위해 요청한 Overlapped 구조체를 받을 포인터	
	HCSocketInfo* pSocketInfo;
	DWORD	dwFlags = 0;

	while (m_bWorkerThread)
	{
		pSocketInfo->overlapped.
		bResult = GetQueuedCompletionStatus(m_hIOCP,
			&recvByteSize,
			(PULONG_PTR)&pCompletionKey,			// completion key
			(LPOVERLAPPED*)&pSocketInfo,			// overlapped I/O 객체
			INFINITE				// 대기할 시간
		);

		if (!bResult && recvByteSize == 0)
		{
			printf_s("[INFO] socket(%d) 접속 끊김\n", (int)pSocketInfo->socket);
			closesocket(pSocketInfo->socket);
			m_socketInfo.erase(pSocketInfo->socket);
			continue;
		}

		pSocketInfo->dataBuf.len = recvByteSize;

		if (recvByteSize == 0)
		{
			closesocket(pSocketInfo->socket);
			m_socketInfo.erase(pSocketInfo->socket);
			continue;
		}
		else
		{
			printf_s("[INFO] 메시지 수신- Bytes : [%d], Msg : [%s]\n",
				pSocketInfo->dataBuf.len, pSocketInfo->dataBuf.buf);

			HCDataFormat dataFormat;
			memcpy_s(&dataFormat, sizeof(HCDataFormat), pSocketInfo->dataBuf.buf, sizeof(HCDataFormat));
			if (dataFormat == HCDataFormat::IP)
			{
				int offset = 0;
				char memoryBuffer[MAX_BUFFER] = { 0, };
				WSABUF sendBuf;
				sendBuf.buf = memoryBuffer;

				HCDataFormat sendDataFormat = HCDataFormat::IP;
				memcpy_s(memoryBuffer + offset, sizeof(HCDataFormat), &sendDataFormat, sizeof(HCDataFormat));
				offset += sizeof(HCDataFormat);


			}
			// 클라이언트의 응답을 그대로 송신			
			nResult = WSASend(
				pSocketInfo->socket,
				&(pSocketInfo->dataBuf),
				1,
				&sendByteSize,
				dwFlags,
				NULL,
				NULL
			);

			if (nResult == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING)
			{
				printf_s("[ERROR] WSASend 실패 : %d", WSAGetLastError());
			}

			printf_s("[INFO] 메시지 송신 - Bytes : [%d], Msg : [%s]\n",
				pSocketInfo->dataBuf.len, pSocketInfo->dataBuf.buf);

			// stSOCKETINFO 데이터 초기화
			ZeroMemory(&(pSocketInfo->overlapped), sizeof(OVERLAPPED));
			ZeroMemory(pSocketInfo->messageBuffer, MAX_BUFFER);
			pSocketInfo->recvByteSize = 0;
			pSocketInfo->sendByteSize = 0;

			dwFlags = 0;

			// 클라이언트로부터 다시 응답을 받기 위해 WSARecv 를 호출해줌
			nResult = WSARecv(
				pSocketInfo->socket,
				&(pSocketInfo->dataBuf),
				1,
				&recvByteSize,
				&dwFlags,
				(LPWSAOVERLAPPED) & (pSocketInfo->overlapped),
				NULL
			);

			if (nResult == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING)
			{
				printf_s("[ERROR] WSARecv 실패 : %d", WSAGetLastError());
			}
		}
	}
}