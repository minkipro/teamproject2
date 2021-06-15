#include "stdafx.h"
#include "SocketCommunication.h"


SocketCommunication* SocketCommunication::instance = nullptr;
SocketCommunication::SocketCommunication()
{
	m_exit = false;
	m_textRender = nullptr;

	WSADATA wsaData;
	COM_THROW_IF_FAILED(WSAStartup(MAKEWORD(2, 2), &wsaData) == 0, "윈속 초기화 실패");
	m_socket = socket(PF_INET, SOCK_STREAM, 0);
	COM_THROW_IF_FAILED(m_socket != INVALID_SOCKET, "소켓 생성 실패");

	auto graphic = HCDEVICE(HCGraphic);
	graphic->CreateTextData(m_textRender);

	std::vector<std::wstring> fontNames;
	m_textRender->GetFontNames(fontNames);
	m_textRender->SetFont(fontNames.front());

	m_pthread = nullptr;
}

SocketCommunication::~SocketCommunication()
{
	m_exit = true;
	
	closesocket(m_socket);
	if (m_pthread != nullptr)
	{
		m_pthread->join();
		delete m_pthread;
		m_pthread = nullptr;
	}
	WSACleanup();
}

void SocketCommunication::Update()
{
	auto keyboard = HCDEVICE(HCKeyboard);
	if (keyboard->IsKeyPressed(DirectX::Keyboard::Keys::F1))
	{
		ConnectStart();
	}

	if (keyboard->IsKeyPressed(DirectX::Keyboard::Keys::F2))
	{
		ListenStart();
	}
	m_mutex.lock();
	while (m_buffer.empty())
	{
		RecvDataStruct& curData = m_buffer.front();
		
		m_buffer.pop();
	}
	m_mutex.unlock();
	
	if (keyboard->IsKeyPressed(DirectX::Keyboard::Keys::F3))
	{
		char dataBuffer[MAX_BUFFER] = { 0, };
		int offset = 0;
		const char* sourceText = "working test";
		size_t textLen = strlen(sourceText);
		memcpy_s(dataBuffer, MAX_BUFFER, sourceText, textLen);
		send(m_socket, dataBuffer, textLen, 0);
	}
	std::wstring imtrue = L"true";
	std::wstring imfalse = L"false";

	m_textRender->m_text= L" is m_pthread exist? : " + (m_pthread != nullptr ? imtrue : imfalse);
}

void SocketCommunication::GetIp(std::vector<unsigned long>& out)
{
	//GetPeerName 예제
	/*sockaddr_in sockAddr;
	int size = sizeof(sockAddr);
	memset(&sockAddr, 0x00, sizeof(sockAddr));
	int ret2 = getpeername(m_socket, (struct sockaddr*)&sockAddr, &size);
	char mybuffer[16];
	strcpy_s(mybuffer, inet_ntoa(sockAddr.sin_addr));*/

	char hostname[1024] = { 0, };
	struct hostent* host_info;
	gethostname(hostname, 1024);
	host_info = gethostbyname(hostname);
	std::string name = host_info->h_name;
	
	int i = 0;
	while (host_info->h_addr_list[i] != 0)
	{
		out.push_back(*(u_long*)host_info->h_addr_list[i++]);
	}
	in_addr addr;
	for (size_t i = 0; i < out.size(); i++)
	{
		addr.s_addr = out[i];
		std::string str = inet_ntoa(addr);
		int a = 1;
	}
}

void SocketCommunication::ConnectStart()
{
	SOCKADDR_IN servAddr = { 0, };
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = inet_addr(SERVER_IP);
	servAddr.sin_port = htons(SERVER_PORT);
	COM_THROW_IF_FAILED(connect(m_socket, (SOCKADDR*)&servAddr, sizeof(servAddr)) != SOCKET_ERROR, "connect 실패");
}

void SocketCommunication::ListenStart()
{
	if (m_pthread != nullptr)
	{
		//COM_THROW_IF_FAILED(false, "ListenStart 함수는 한번만 호출되어야 합니다.");
		return;
	}
	auto receiveFunction = [](bool* exit, SOCKET receiveSocket, std::queue<RecvDataStruct>* pm_buffer, std::mutex* ourMutex)
	{
		while (*exit == false)
		{
			RecvDataStruct dataStruct;
			recv(receiveSocket, dataStruct.buffer, MAX_BUFFER, 0);
			ourMutex->lock();
			pm_buffer->push(dataStruct);
			ourMutex->unlock();
		}
	};
	m_pthread = new std::thread(receiveFunction, &m_exit, m_socket, m_buffer, &m_mutex);
}



