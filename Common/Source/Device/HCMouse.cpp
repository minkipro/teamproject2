#include "stdafx.h"
#include "HCMouse.h"

DirectX::XMFLOAT3* HCMouse::s_targetPos = nullptr;
DirectX::XMFLOAT2 HCMouse::s_offsetVec = {};
DirectX::XMFLOAT2 HCMouse::s_mousePos = {};

void HCMouse::Init()
{
}

void HCMouse::Update()
{
	auto state = DirectX::Mouse::Get().GetState();
	DirectX::Mouse::ButtonStateTracker::Update(state);
	s_mousePos.x = static_cast<float>(state.x);
	s_mousePos.y = static_cast<float>(state.y);

	if (leftButton == HCMOUSE_BUTTON_STATE::RELEASED || leftButton == HCMOUSE_BUTTON_STATE::UP)
	{
		s_targetPos = nullptr;
	}

	if (s_targetPos)
	{
		s_targetPos->x = s_mousePos.x + s_offsetVec.x;
		s_targetPos->y = s_mousePos.y + s_offsetVec.y;
	}
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

void HCMouse::SetMoveableClickedObject(DirectX::XMFLOAT3* pos)
{
	s_targetPos = pos;
	s_offsetVec.x = pos->x - s_mousePos.x;
	s_offsetVec.y = pos->y - s_mousePos.y;
}
