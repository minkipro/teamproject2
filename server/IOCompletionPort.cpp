#include "IOCompletionPort.h"
#include <process.h>
#include <iostream>
#include "TCPSocket/HCCommunicationProtocol.h"
#include "COMException.h"
#include <thread>

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
	m_hIOCP = INVALID_HANDLE_VALUE;
	m_listenSocket = INVALID_SOCKET;
	m_pWorkerHandle = nullptr;
}


IOCompletionPort::~IOCompletionPort()
{
	WSACleanup();
	size_t socketNum = m_pSocketInfo.size();
	for (size_t i = 0; i < socketNum; i++)
	{
		if (nullptr != m_pSocketInfo[i])
		{
			closesocket(m_pSocketInfo[i]->socket);
			delete m_pSocketInfo[i];
		}
		m_pSocketInfo[i] = nullptr;
	}
	m_pSocketInfo.clear();
	m_bWorkerThread = false;
	if (m_pWorkerHandle)
	{
		delete[] m_pWorkerHandle;
		m_pWorkerHandle = NULL;
	}
}

bool IOCompletionPort::Initialize()
{
	WSADATA wsaData;
	int nResult;
	nResult = WSAStartup(MAKEWORD(2, 2), &wsaData);

	COM_THROW_IF_FAILED(nResult == 0, "[ERROR] winsock 초기화 실패");

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

	nResult = listen(m_listenSocket, SOMAXCONN);
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

		sockaddr_in sockAddr;
		int sockAddrSize = sizeof(sockAddr);
		memset(&sockAddr, 0x00, sizeof(sockAddr));
		if (0 != getpeername(clientSocket, (struct sockaddr*)&sockAddr, &sockAddrSize))
		{
			COM_THROW_IF_FAILED(false, "getpeername fail");
		}

		char ipbuffer[16];
		strcpy_s(ipbuffer, inet_ntoa(sockAddr.sin_addr));


		if (clientSocket == INVALID_SOCKET)
		{
			printf_s("[ERROR] Accept 실패\n");
			return;
		}
		printf_s("[INFO] socket(%d) 접속\n", reinterpret_cast<int>((void*)clientSocket));
		m_pSocketInfo.push_back(new SocketInfo());
		SocketInfo* curSocketInfo = m_pSocketInfo.back();
		curSocketInfo->socket = clientSocket;
		//curSocketInfo->recvBytes = 0;
		//curSocketInfo->sendBytes = 0;
		curSocketInfo->dataBuf.len = MAX_BUFFER;
		curSocketInfo->dataBuf.buf = curSocketInfo->messageBuffer;
		flags = 0;

		m_hIOCP = CreateIoCompletionPort(
			(HANDLE)clientSocket, m_hIOCP, (DWORD)curSocketInfo, 0
		);

		nResult = WSARecv(
			curSocketInfo->socket,
			&curSocketInfo->dataBuf,
			1,
			&recvBytes,
			&flags,
			&(curSocketInfo->overlapped),
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
	DWORD	recvBytes;
	DWORD	sendBytes;
	SocketInfo* pCompletionKey;
	SocketInfo* pSocketInfo;
	DWORD	dwFlags = 0;

	while (m_bWorkerThread)
	{
		bResult = GetQueuedCompletionStatus(m_hIOCP,
			&recvBytes,				// 실제로 전송된 바이트
			(PULONG_PTR)&pCompletionKey,	// completion key
			(LPOVERLAPPED*)&pSocketInfo,			// overlapped I/O 객체
			INFINITE				// 대기할 시간
		);

		if (!bResult && recvBytes == 0)
		{
			printf_s("[INFO] socket(%d) 접속 끊김\n", (int)(pSocketInfo->socket));
			
			for (std::vector<SocketInfo*>::iterator it = m_pSocketInfo.begin(); it != m_pSocketInfo.end(); it++)
			{
				if (it[0]->socket == pSocketInfo->socket)
				{
					m_pSocketInfo.erase(it);
				}
			}

			closesocket(pSocketInfo->socket);
			free(pSocketInfo);
			continue;
		}

		pSocketInfo->dataBuf.len = recvBytes;

		if (recvBytes == 0)
		{
			for (std::vector<SocketInfo*>::iterator it = m_pSocketInfo.begin(); it != m_pSocketInfo.end(); it++)
			{
				if (it[0]->socket == pSocketInfo->socket)
				{
					m_pSocketInfo.erase(it);
				}
			}
			closesocket(pSocketInfo->socket);
			free(pSocketInfo);
			continue;
		}
		else
		{
			printf_s("[INFO] 메시지 수신- Bytes : [%d], Msg : [%s]\n",
				pSocketInfo->dataBuf.len, pSocketInfo->dataBuf.buf);

			// 클라이언트의 응답을 그대로 송신			
			nResult = WSASend(
				pSocketInfo->socket,
				&(pSocketInfo->dataBuf),
				1,
				&sendBytes,
				dwFlags,
				NULL,
				NULL
			);

			char* curBuffer = pSocketInfo->dataBuf.buf;
			unsigned int bufferOffset = 0;
			HCTypeEnum dataType;
			size_t arrNum;
			HCBufferToData(curBuffer, bufferOffset, dataType);
			HCBufferToData(curBuffer, bufferOffset, arrNum);
			switch (dataType)
			{
			case HCTypeEnum::HCchar:
				for (size_t i = 0; i < arrNum; i++)
				{
				}
				break;
			case HCTypeEnum::HCint:
				break;
			case HCTypeEnum::HCfloat:
				break;
			case HCTypeEnum::HCdouble:
				break;
			case HCTypeEnum::HCSizeT:
				break;
			case HCTypeEnum::HCSTRUCT1:
				break;
			default:
				break;
			}

			if (nResult == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING)
			{
				printf_s("[ERROR] WSASend 실패 : ", WSAGetLastError());
			}

			printf_s("[INFO] 메시지 송신 - Bytes : [%d], Msg : [%s]\n",
				pSocketInfo->dataBuf.len, pSocketInfo->dataBuf.buf);

			// stSOCKETINFO 데이터 초기화
			ZeroMemory(&(pSocketInfo->overlapped), sizeof(OVERLAPPED));
			pSocketInfo->dataBuf.len = MAX_BUFFER;
			pSocketInfo->dataBuf.buf = pSocketInfo->messageBuffer;
			ZeroMemory(pSocketInfo->messageBuffer, MAX_BUFFER);
			//pSocketInfo->recvBytes = 0;
			//pSocketInfo->sendBytes = 0;

			dwFlags = 0;

			// 클라이언트로부터 다시 응답을 받기 위해 WSARecv 를 호출해줌
			nResult = WSARecv(
				pSocketInfo->socket,
				&(pSocketInfo->dataBuf),
				1,
				&recvBytes,
				&dwFlags,
				(LPWSAOVERLAPPED) & (pSocketInfo->overlapped),
				NULL
			);

			if (nResult == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING)
			{
				printf_s("[ERROR] WSARecv 실패 : ", WSAGetLastError());
			}
		}
	}
}