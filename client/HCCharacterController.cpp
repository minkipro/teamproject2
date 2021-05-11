#include "stdafx.h"
#include "HCCharacterController.h"

HC::CharacterController::CharacterController(DirectX::XMFLOAT3* position)
{
	m_position = position;
}

void HC::CharacterController::Update()
{
	auto keyboard = HCDEVICE(HCKeyboard);
	auto timer = HCDEVICE(HCTimer);

	auto state = DirectX::Keyboard::Get().GetState();
	
	if (state.W)
	{
		m_position->y -= timer->GetDeltatime();
	}
	if (state.A)
	{
		m_position->x -= timer->GetDeltatime();
	}
	if (state.S)
	{
		m_position->y += timer->GetDeltatime();
	}
	if (state.D)
	{
		m_position->x +=timer->GetDeltatime();
	}
}
