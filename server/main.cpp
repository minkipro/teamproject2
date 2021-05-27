#include <thread>
#include <WinSock2.h>
#include <atomic>
#include <vector>
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

	struct ClientData
	{
		SOCKET clientSocket;
		SOCKADDR_IN clientAddr;
		std::atomic<char> dataBuffer[1024] = { 0, };
	};
	std::vector<ClientData> clientSockets;

	int clientAddrSize = sizeof(SOCKADDR_IN);


	auto receiveFunction = [](ClientData& clientData)
	{
		char buffer[1024];
		while (true)
		{
			recv(clientData.clientSocket, buffer, 1024, 0);
			clientData
		}
	}
	auto acceptFunction = [serverSocket, &clientSockets, &clientAddrSize]()
	{
		while (acceptEnable)
		{
			clientSockets.push_back(ClientData());
			ClientData curData = clientSockets.back();
			curData.clientSocket = accept(serverSocket, (SOCKADDR*)&curData.clientAddr, &clientAddrSize);
			if (curData.clientSocket == INVALID_SOCKET)
			{
				clientSockets.pop_back();
			}
			else
			{
				
			}
		}
	};

	std::thread acceptThread(acceptFunction);

	

	szClntAddr = sizeof(SOCKADDR);
	clientSocket = accept(serverSocket, (SOCKADDR*)&clntAddr, &szClntAddr);  
	if (clientSocket == INVALID_SOCKET)
		ErrorHandling("accept() error!");

	//send(clientSocket, message, sizeof(message), 0);

	closesocket(clientSocket);
	closesocket(serverSocket);
	WSACleanup();
}