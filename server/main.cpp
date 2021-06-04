#include <thread>
#include <WinSock2.h>
#include <atomic>
#include <vector>
#include <functional>
void ErrorHandling(const char* message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
bool acceptEnable = true;
void main()
{
	WSADATA     wsaData;
	SOCKET      serverSocket, clientSocket;
	SOCKADDR_IN servAddr, clntAddr;

	short   port = 5001;
	int     szClntAddr;

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		ErrorHandling("WSAStartup() error!");

	// TCP 家南积己
	serverSocket = socket(PF_INET, SOCK_STREAM, 0);

	if (serverSocket == INVALID_SOCKET)
		ErrorHandling("socket() error!");

	// 家南狼 林家沥焊
	memset(&servAddr, 0, sizeof(servAddr));

	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY);  // 泅犁 PC IP林家 荤侩
	servAddr.sin_port = htons(port);        // 器飘锅龋

	// 林家 瘤沥
	if (bind(serverSocket, (SOCKADDR*)&servAddr, sizeof(servAddr)) == SOCKET_ERROR)
		ErrorHandling("bind() error!");

	if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR)    // connect
		ErrorHandling("listen() error!");
	setsockopt
	struct ClientInfo
	{
		SOCKET clientSocket;
		SOCKADDR_IN clientAddr;
		std::atomic<char> dataBuffer[1024] = { 0, };
		std::thread* recvThread = nullptr;
		bool recvEnable = false;
	};
	SOCKET testSocket;
	ioctlsocket(m_socket, FIONBIO, &iMode);
	std::vector<ClientInfo> clientInfos;

	int clientAddrSize = sizeof(SOCKADDR_IN);

	std::function<void(ClientInfo&)> receiveFunction = [](ClientInfo& clientInfo)
	{
		char buffer[1024];
		while (clientInfo.recvEnable)
		{
			int ret = recv(clientInfo.clientSocket, buffer, 1024, 0);
			if (ret != 0)
			{
				ErrorHandling("recv error!");
			}

			for (int i = 0; i < 1024; i++)
			{
				clientInfo.dataBuffer[i].store(buffer[i]);
			}
		}
	};

	
	std::function<void()> acceptFunction = [serverSocket, &clientInfos, &clientAddrSize, &receiveFunction]()
	{
		while (acceptEnable)
		{
			clientInfos.push_back(ClientInfo());
			ClientInfo& curData = clientInfos.back();
			curData.clientSocket = accept(serverSocket, (SOCKADDR*)&curData.clientAddr, &clientAddrSize);
			if (curData.clientSocket == INVALID_SOCKET)
			{
				clientInfos.pop_back();
			}
			else
			{
				curData.recvThread = new std::thread(receiveFunction, curData);
				curData.recvEnable = true;
			}
		}
	};

	std::thread acceptThread(acceptFunction);

	//send, recv

	acceptEnable = false;
	acceptThread.join();
	size_t clientNum = clientInfos.size() - 1;
	for (size_t i = 0; i < clientNum; i++)
	{
		clientInfos[i].recvEnable = false;
		clientInfos[i].recvThread->join();
		delete clientInfos[i].recvThread;
	}
	//send(clientSocket, message, sizeof(message), 0);

	closesocket(clientSocket);
	closesocket(serverSocket);
	WSACleanup();
}