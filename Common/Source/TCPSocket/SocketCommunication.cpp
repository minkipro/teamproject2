#include "stdafx.h"
#include "SocketCommunication.h"


SocketCommunication* SocketCommunication::instance = nullptr;
SocketCommunication::SocketCommunication()
{
	m_exit = false;
	m_textRender = nullptr;

	WSADATA wsaData;
	COM_THROW_IF_FAILED(WSAStartup(MAKEWORD(2, 2), &wsaData) == 0, "���� �ʱ�ȭ ����");
	m_socket = socket(PF_INET, SOCK_STREAM, 0);
	COM_THROW_IF_FAILED(m_socket != INVALID_SOCKET, "���� ���� ����");

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

	std::wstring imtrue = L"true";
	std::wstring imfalse = L"false";

	m_textRender->m_text= L" is m_pthread exist? : " + (m_pthread != nullptr ? imtrue : imfalse);
}

void SocketCommunication::GetIp(std::vector<unsigned long>& out)
{
	int size;
	sockaddr_in sockAddr;

	size = sizeof(sockAddr);
	memset(&sockAddr, 0x00, sizeof(sockAddr));
	int ret2 = getpeername(m_socket, (struct sockaddr*)&sockAddr, &size);
	char mybuffer[16];
	strcpy(mybuffer, inet_ntoa(sockAddr.sin_addr));

	char hostname[1024] = { 0, };
	struct hostent* host_info;
	//gethostname(hostname, 1024);
	strcpy_s(hostname, 1024,"DESKTOP-Fil2GBD");
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

	//
	int nPrivate = -1;
	std::string strIP;
	int ret = 0;
	for (int i = 0; host_info->h_addr_list[i]; i++)
	{
		switch (((PUCHAR)host_info->h_addr_list[i])[0])
		{
			// �ڵ� ���� ip �ּ� ����
		case 169:
			if (((PUCHAR)host_info->h_addr_list[i])[1] == 254)
				continue;
			break;

			// �缳 ip �ּ��� ��� ����
		case 10:
			nPrivate = i;
			break;
		case 172:
			if (((PUCHAR)host_info->h_addr_list[i])[1] > 15 &&
				(host_info->h_addr_list[i])[1] < 32)
				nPrivate = i;
			break;
		case 192:
			if (((PUCHAR)host_info->h_addr_list[i])[1] == 168)
				nPrivate = i;
			break;

			// ���� ip �ּ��� ���
		default:

			for (int j = 0; j < 4; j++)
			{
				strIP += ((PUCHAR)host_info->h_addr_list[i])[j];
				if (j != 3)
					strIP += ".";
			}
			ret = 1;
		}
	}

	if (nPrivate >= 0)
	{
		for (int j = 0; j < 4; j++)
		{
			strIP += ((PUCHAR)host_info->h_addr_list[nPrivate])[j];
			if (j != 3)
				strIP += ".";
		}
		ret = 2;
	}
	else
		ret = 0;
	int a = 1;
}

void SocketCommunication::ConnectStart()
{
	const char* ip = "211.192.102.158";		//�� ���� ip
	const char* fortNum = "24642";			//��Ÿ��븮 fortNum���� ���� �ʿ�.

	SOCKADDR_IN servAddr = { 0, };
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = inet_addr(ip);
	servAddr.sin_port = htons(atoi(fortNum));
	COM_THROW_IF_FAILED(connect(m_socket, (SOCKADDR*)&servAddr, sizeof(servAddr)) != SOCKET_ERROR, "connect ����");
}

void SocketCommunication::ListenStart()
{
	if (m_pthread != nullptr)
	{
		//COM_THROW_IF_FAILED(false, "ListenStart �Լ��� �ѹ��� ȣ��Ǿ�� �մϴ�.");
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


