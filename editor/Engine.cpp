#include "stdafx.h"
#include "Engine.h"
#include "Window/HCWindow.h"
#include "Graphics/HCGraphicDX11.h"

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
	
	m_scene.Init();
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

			m_graphic->Render();
		}
	}

	return (int)msg.wParam;
}