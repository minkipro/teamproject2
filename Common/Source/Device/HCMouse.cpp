#include "stdafx.h"
#include "HCMouse.h"

void HCMouse::Init()
{
}

void HCMouse::Update()
{
	DirectX::Mouse::ButtonStateTracker::Update(DirectX::Mouse::Get().GetState());
}

DirectX::Mouse::ButtonStateTracker::ButtonState HCMouse::GetButtonState(HCMouse::MouseButton button)
{
	switch (button)
	{
	case HCMouse::LBUTTON:
		return leftButton;
	case HCMouse::MBUTTON:
		return middleButton;
	case HCMouse::RBUTTON:
		return rightButton;
	case HCMouse::BUTTON1:
		return xButton1;
	case HCMouse::BUTTON2:
		return xButton2;
	}
	
	COM_THROW_IF_FAILED(false, "wrong mouseButtonInput");
	return DirectX::Mouse::ButtonStateTracker::ButtonState::UP;
}

LRESULT HCMouse::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return LRESULT();
}
