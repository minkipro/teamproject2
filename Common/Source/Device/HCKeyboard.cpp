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
	switch (uMsg)
	{
	case WM_ACTIVATEAPP:
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
	case WM_KEYUP:
	case WM_SYSKEYUP:
		DirectX::Keyboard::ProcessMessage(uMsg, wParam, lParam);
		break;
	}
	return 0;
}

bool HCKeyboard::IsKeyCurrState(HCKEYBOARD_KEY_TYPE key, HCKEYBOARD_KEY_STATE state)
{
	bool result = false;

	switch (state)
	{
	case HCKEYBOARD_KEY_STATE::UP:
		break;
	case HCKEYBOARD_KEY_STATE::HELD:
	{
		result = lastState.IsKeyDown(key);
	}
		break;
	case HCKEYBOARD_KEY_STATE::RELEASED:
	{
		result = released.IsKeyDown(key);
	}
		break;
	case HCKEYBOARD_KEY_STATE::PRESSED:
	{
		result = pressed.IsKeyDown(key);
	}
		break;
	default:
		break;
	}
	
	return result;
}
