#include "stdafx.h"
#include "Engine.h"
#include "Window/HCWindow.h"
#include "Graphics/HCGraphicDX11.h"
#include "Graphics\HCMeshManager.h"
#include "Physics\Physics2D.h"

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
	m_physics = CreateDevice<Physics2D>(typeid(HCPhysics).name());

	for (auto& it : m_devices)
	{
		it->Init();
		m_window->RegisterProc(it.get());
	}

	CreateBaseMeshs();

	m_scene.Init();

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
			m_physics->PickingUpdate();

			m_scene.Update();

			m_physics->Update();
			m_graphic->Update();

			m_graphic->RenderBegin();
			m_scene.Render();
			m_graphic->DrawFont();
			m_graphic->RenderEnd();
		}
	}
	return (int)msg.wParam;
}

void Engine::CreateBaseMeshs()
{
	HCMesh onePointMesh;
	HC::GRAPHIC_RESOURCE_DESC onePointExtToRectDesc;

	HCOnePointExtToRect temp;

	onePointMesh.VertexCount = 1;
	onePointMesh.Primitive = HC::PRIMITIVE_TOPOLOGY::POINT;

	onePointExtToRectDesc.Type = HC::GRAPHIC_RESOURCE_TYPE::GRAPHIC_RESOURCE_BUFFER;
	onePointExtToRectDesc.UsageType = HC::GRAPHIC_RESOURCE_USAGE_TYPE::GRAPHIC_RESOURCE_USAGE_DEFAULT;
	onePointExtToRectDesc.BindFlags = HC::GRAPHIC_RESOURCE_BIND_VERTEX_BUFFER;
	onePointExtToRectDesc.Flags = HC::GRAPHIC_RESOURCE_FLAG_NONE;
	onePointExtToRectDesc.Stride = sizeof(HCOnePointExtToRect);
	onePointExtToRectDesc.Buffer.StrideNum = 1;
	onePointExtToRectDesc.DefaultData = &temp;

	m_graphic->CreateResource(onePointExtToRectDesc, onePointMesh.VertexBuffer);

	HCMeshManager::Get()->SetMesh(typeid(HCOnePointExtToRect).name(), onePointMesh);
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