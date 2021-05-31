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

	m_timer = CreateDevice<HCTimer>(typeid(HCTimer).name());
	m_mouse = CreateDevice<HCMouse>(typeid(HCMouse).name());
	m_keyboard = CreateDevice<HCKeyboard>(typeid(HCKeyboard).name());
	m_koreanInput = CreateDevice<HCKoreanInput>(typeid(HCKoreanInput).name());
	m_graphic = CreateDevice<HCGraphicDX11>(typeid(HCGraphic).name(), m_window->GetHandle());

	for (auto& it : m_devices)
	{
		it->Init();
		m_window->RegisterProc(it.get());
	}

	CreateBaseMeshs();

	m_scene.Init();

	std::vector<unsigned long> ips;
	SocketCommunication::Get()->GetIp(ips);

	m_timer->Reset();
	m_timer->Start();
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
			m_timer->Update();
			CalculateFrame();

			m_keyboard->Update();
			m_mouse->Update();
			m_koreanInput->Update();

			m_scene.Update();

			m_graphic->Update();

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
	HCMesh normalRect;
	HC::GRAPHIC_RESOURCE_DESC onePointExtToRectDesc;
	std::vector<HCPositionVertex> temp(6);

	temp[0].Position = { 0.0f,0.0f,0.0f };
	temp[1].Position = { 1.0f,0.0f,0.0f };
	temp[2].Position = { 0.0f,1.0f,0.0f };
	temp[3].Position = { 0.0f,1.0f,0.0f };
	temp[4].Position = { 1.0f,0.0f,0.0f };
	temp[5].Position = { 1.0f,1.0f,0.0f };

	normalRect.VertexCount = temp.size();
	normalRect.Primitive = HC::PRIMITIVE_TOPOLOGY::TRIANGLELIST;

	onePointExtToRectDesc.Type = HC::GRAPHIC_RESOURCE_TYPE::GRAPHIC_RESOURCE_BUFFER;
	onePointExtToRectDesc.UsageType = HC::GRAPHIC_RESOURCE_USAGE_TYPE::GRAPHIC_RESOURCE_USAGE_DEFAULT;
	onePointExtToRectDesc.BindFlags = HC::GRAPHIC_RESOURCE_BIND_VERTEX_BUFFER;
	onePointExtToRectDesc.Flags = HC::GRAPHIC_RESOURCE_FLAG_NONE;
	onePointExtToRectDesc.Stride = sizeof(HCPositionVertex);
	onePointExtToRectDesc.Buffer.StrideNum = temp.size();
	onePointExtToRectDesc.DefaultData = temp.data();

	m_graphic->CreateResource(onePointExtToRectDesc, normalRect.VertexBuffer);

	HCMeshManager::Get()->SetMesh("NormalRect", normalRect);
}

void Engine::CalculateFrame()
{
	static int frameCnt = 0;
	static float timeElapsed = 0.0f;

	frameCnt++;

	if ((m_timer->TotalTime() - timeElapsed) >= 1.0f)
	{
		float fps = (float)frameCnt;
		float mspf = 1000.0f / fps;

		std::wstring fpsStr = std::to_wstring(fps);
		std::wstring mspfStr = std::to_wstring(mspf);
		
		std::wstring windowText = std::wstring(L"HC Project") +
			L"    fps: " + fpsStr +
			L"   mspf: " + mspfStr;

		SetWindowText(m_window->GetHandle(), windowText.c_str());

		frameCnt = 0;
		timeElapsed += 1.0f;
	}
}
