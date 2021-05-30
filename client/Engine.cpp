#include "stdafx.h"
#include "Engine.h"
#include "Window/HCWindow.h"
#include "Graphics/HCGraphicDX11.h"
#include "TCPSocket/SocketCommunication.h"
#include "Graphics\HCMeshManager.h"

Engine* Engine::m_engine = nullptr;

void Engine::Init(HINSTANCE hInstance)
{
	m_engine = this;
	m_window = std::make_unique<HCWindow>();

	m_window->Init(hInstance);

	CreateDevice<HCMouse>(typeid(HCMouse).name());
	CreateDevice<HCKeyboard>(typeid(HCKeyboard).name());
	CreateDevice<HCKoreanInput>(typeid(HCKoreanInput).name());
	CreateDevice<HCTimer>(typeid(HCTimer).name());
	m_graphic = CreateDevice<HCGraphicDX11>(typeid(HCGraphic).name(),m_window->GetHandle());

	for (auto& it : m_devices)
	{
		it->Init();
		m_window->RegisterProc(it.get());
	}
	
	CreateBaseMeshs();

	m_scene.Init();

	std::vector<unsigned long> ips;
	SocketCommunication::Get()->GetIp(ips);
}

int Engine::Run()
{
	MSG msg = {};

	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			m_scene.Update();

			for (auto& it : m_devices)
			{
				it->Update();
			}

			SocketCommunication::Get()->Update();

			m_graphic->RenderBegin();
			m_scene.Render();
			m_graphic->DrawFont();
			m_graphic->RenderEnd();
		}
	}
	delete SocketCommunication::Get();
	return (int)msg.wParam;
}

void Engine::CreateBaseMeshs()
{
	HCMesh onePointExtToRect;
	HC::GRAPHIC_RESOURCE_DESC onePointExtToRectDesc;
	HCOnePointExtToRect temp;

	onePointExtToRect.VertexCount = 1;
	onePointExtToRect.Primitive = HC::PRIMITIVE_TOPOLOGY::POINT;
	
	onePointExtToRectDesc.Type = HC::GRAPHIC_RESOURCE_TYPE::GRAPHIC_RESOURCE_BUFFER;
	onePointExtToRectDesc.UsageType = HC::GRAPHIC_RESOURCE_USAGE_TYPE::GRAPHIC_RESOURCE_USAGE_DEFAULT;
	onePointExtToRectDesc.BindFlags = HC::GRAPHIC_RESOURCE_BIND_VERTEX_BUFFER;
	onePointExtToRectDesc.Flags = HC::GRAPHIC_RESOURCE_FLAG_NONE;
	onePointExtToRectDesc.Stride = sizeof(HCOnePointExtToRect);
	onePointExtToRectDesc.Buffer.StrideNum = 1;
	onePointExtToRectDesc.DefaultData = &temp;

	m_graphic->CreateResource(onePointExtToRectDesc, onePointExtToRect.VertexBuffer);

	HCMeshManager::Get()->SetMesh(typeid(HCOnePointExtToRect).name(), onePointExtToRect);
}
