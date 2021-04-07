#include "stdafx.h"
#include "HCKeyboard.h"

void HCKeyboard::Init()
{

}

void HCKeyboard::Update()
{
	DirectX::Keyboard::KeyboardStateTracker::Update(DirectX::Keyboard::Get().GetState());
}

LRESULT HCKeyboard::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	DirectX::Keyboard::ProcessMessage(uMsg, wParam, lParam);

	switch (uMsg)
	{
	case WM_KEYDOWN:
	{
		unsigned char keycode = static_cast<unsigned char>(wParam);
		if (autoRepeat)
		{
			DirectX::Keyboard::KeyboardStateTracker::IsKeyPressed(DirectX::Keyboard::Keys::A);
			keyStates[keycode] = true;
			keyBuffer.push(KeyboardEvent(KeyboardEvent::EventType::Press, keycode));
		}
		else
		{
			const bool wasPressed = lParam & 0x40000000;
			if (!wasPressed)
			{
				keyStates[keycode] = true;
				keyBuffer.push(KeyboardEvent(KeyboardEvent::EventType::Press, keycode));
			}
		}
		return 0;
	}
	case WM_KEYUP:
	{
		unsigned char keycode = static_cast<unsigned char>(wParam);
		keyStates[keycode] = false;
		keyBuffer.push(KeyboardEvent(KeyboardEvent::EventType::Release, keycode));
		return 0;
	}
	case WM_CHAR:
	{
		unsigned char ch = static_cast<unsigned char>(wParam);
		if (autoRepeat)
		{
			charBuffer.push(ch);
		}
		else
		{
			const bool wasPressed = lParam & 0x40000000;
			if (!wasPressed)
			{
				charBuffer.push(ch);
			}
		}
		return 0;
	}
	return 0;
	}

	return 0;
}