#include "stdafx.h"
#include "Engine.h"
#include "Device/HCKeyboard.h"
#include "Device/HCMouse.h"
#include "Device/HCKoreanInput.h"
#include "Window/HCWindow.h"
#include "Graphics/HCGraphicDX11.h"

void Engine::Init(HINSTANCE hInstance)
{
	m_Window = std::make_unique<HCWindow>();

	m_Window->Init(hInstance);

	m_Devices.emplace_back(std::make_unique<HCMouse>());
	m_Devices.emplace_back(std::make_unique<HCKeyboard>());
	m_Devices.emplace_back(std::make_unique<HCKoreanInput>());
	m_Devices.emplace_back(std::make_unique<HCGraphicDX11>(m_Window->GetHandle()));
	m_Graphic = static_cast<HCGraphic*>(m_Devices.back().get());

	for (auto& it : m_Devices)
	{
		it->Init();
		m_Window->RegisterProc(it.get());
	}
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
			for (auto& it : m_Devices)
			{
				it->Update();
			}

			m_Graphic->Render();
		}
	}

	return (int)msg.wParam;
}