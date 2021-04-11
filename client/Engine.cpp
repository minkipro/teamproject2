#include "stdafx.h"
#include "Engine.h"
#include "Device/HCKeyboard.h"
#include "Device/HCMouse.h"
#include "Device/HCKoreanInput.h"
#include "Window/HCWindow.h"
#include "Graphics/HCGraphicDX11.h"

Engine* Engine::m_Engine = nullptr;

void Engine::Init(HINSTANCE hInstance)
{
	m_Engine = this;
	m_Window = std::make_unique<HCWindow>();

	m_Window->Init(hInstance);

	CreateDevice<HCMouse>(typeid(HCMouse).name());
	CreateDevice<HCKeyboard>(typeid(HCKeyboard).name());
	CreateDevice<HCKoreanInput>(typeid(HCKoreanInput).name());
	m_Graphic = CreateDevice<HCGraphicDX11>(typeid(HCGraphic).name(),m_Window->GetHandle());
	for (auto& it : m_Devices)
	{
		it->Init();
		m_Window->RegisterProc(it.get());
	}
	
	m_Scene.Init();
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
			m_Scene.Update();

			for (auto& it : m_Devices)
			{
				it->Update();
			}

			m_Graphic->Render();
		}
	}

	return (int)msg.wParam;
}