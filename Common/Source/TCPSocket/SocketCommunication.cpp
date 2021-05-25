#include "stdafx.h"
#include "SocketCommunication.h"


SocketCommunication* SocketCommunication::instance = nullptr;
SocketCommunication::SocketCommunication()
{
	m_exit = false;
	
	WSADATA wsaData;
	COM_THROW_IF_FAILED(WSAStartup(MAKEWORD(2, 2), &wsaData) == 0, "윈속 초기화 실패");
	m_socket = socket(PF_INET, SOCK_STREAM, 0);
	COM_THROW_IF_FAILED(m_socket != INVALID_SOCKET, "소켓 생성 실패");

	auto graphic = HCDEVICE(HCGraphic);
	auto font = graphic->GetFont();
	m_textIndex = font->AddText();

	m_pthread = nullptr;
}

SocketCommunication::~SocketCommunication()
{
	m_exit = true;
	if (m_pthread != nullptr)
	{
		m_pthread->join();
		delete m_pthread;
		m_pthread = nullptr;
	}
	closesocket(m_socket);
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


	auto graphic = HCDEVICE(HCGraphic);
	auto font = graphic->GetFont();
	std::wstring imtrue = L"true";
	std::wstring imfalse = L"false";
	std::wstring outText =
		L" is m_pthread exist? : " + (m_pthread != nullptr ? imtrue : imfalse);
	font->SetText(m_textIndex, outText.c_str());
}

void SocketCommunication::GetIp(std::vector<unsigned long>& out)
{
	char hostname[1024] = { 0, };
	struct hostent* host_info;
	DWORD dw;
	gethostname(hostname, 1024);
	host_info = gethostbyname(hostname); // gethostbyname function retrieves host information.
  // gethostbyname returns a pointer of type struct hostent.
  //A null pointer is returned if an error occurs. The specific error number can be known by calling WSAGetLastError.
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
	const char* ip = "211.192.102.158";		//제 공인 ip
	const char* fortNum = "24642";			//스타듀밸리 fortNum으로 변경 필요.

	SOCKADDR_IN servAddr = { 0, };
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = inet_addr(ip);
	servAddr.sin_port = htons(atoi(fortNum));
	COM_THROW_IF_FAILED(connect(m_socket, (SOCKADDR*)&servAddr, sizeof(servAddr)) != SOCKET_ERROR, "connect 실패");
}

void SocketCommunication::ListenStart()
{
	if (m_pthread != nullptr)
	{
		//COM_THROW_IF_FAILED(false, "ListenStart 함수는 한번만 호출되어야 합니다.");
		return;
	}
	auto receiveFunction = [](bool* exit, SOCKET receiveSocket, std::atomic<char>* pm_buffer)
	{
		char buffer[1024] = { 0, };
		while (*exit == false)
		{
			recv(receiveSocket, buffer, 1024, 0);
			for (int i = 0; i < 1024; i++)
			{
				pm_buffer[i].store(buffer[i]);
			}
		}
	};
	m_pthread = new std::thread(receiveFunction, &m_exit, m_socket, m_buffer);
}


