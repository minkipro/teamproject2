#include "stdafx.h"
#include "HCMouse.h"

void HCMouse::Init()
{
}

void HCMouse::Update()
{
	DirectX::Mouse::ButtonStateTracker::Update(DirectX::Mouse::Get().GetState());
}

DirectX::Mouse::ButtonStateTracker::ButtonState HCMouse::GetButtonState(HCMOUSE_BUTTON_TYPE button)
{
	switch (button)
	{
	case HCMOUSE_BUTTON_TYPE::LBUTTON:
		return leftButton;
	case HCMOUSE_BUTTON_TYPE::MBUTTON:
		return middleButton;
	case HCMOUSE_BUTTON_TYPE::RBUTTON:
		return rightButton;
	case HCMOUSE_BUTTON_TYPE::BUTTON1:
		return xButton1;
	case HCMOUSE_BUTTON_TYPE::BUTTON2:
		return xButton2;
	}
	
	COM_THROW_IF_FAILED(false, "wrong mouseButtonInput");
	return HCMOUSE_BUTTON_STATE::UP;
}

LRESULT HCMouse::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return LRESULT();
}
