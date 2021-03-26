#include "Engine.h"
#define WIN32_LEAN_AND_MEAN      
#include <Windows.h>
#include "Device/HCKeyboard.h"
#include "Device/HCMouse.h"
#include "Window/HCWindow.h"

void Engine::Init(HINSTANCE hInstance)
{
	m_Window = std::make_unique<HCWindow>();

	m_Devices.emplace_back(std::make_unique<HCMouse>());
	m_Devices.emplace_back(std::make_unique<HCKeyboard>());

	m_Window->Init(hInstance);

	for (auto& it : m_Devices)
	{
		it->Init();
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
		}
	}

	return (int)msg.wParam;
}