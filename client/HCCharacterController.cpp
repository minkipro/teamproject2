#include "stdafx.h"
#include "HCCharacterController.h"

void HCCharacterController::Update()
{
	if (m_currChar)
	{
		auto keyDeivce = HCDEVICE(HCKeyboard);
		auto mouseDeivce = HCDEVICE(HCMouse);
		float deltaTime = HCDEVICE(HCTimer)->GetDeltatime();

		for (auto& it : m_controlFuncs[static_cast<size_t>(HCINPUT_DEVICE_TYPE::KEYBOARD)])
		{
			if (keyDeivce->IsKeyCurrState(it.Keyboard.Key, it.Keyboard.KeyState))
			{
				it.ControlFunc(m_currChar, deltaTime);
			}
		}

		for (auto& it : m_controlFuncs[static_cast<size_t>(HCINPUT_DEVICE_TYPE::MOUSE)])
		{
			if (mouseDeivce->GetButtonState(it.Mouse.Button) == it.Mouse.ButtonState)
			{
				it.ControlFunc(m_currChar, deltaTime);
			}
		}
	}
}

void HCCharacterController::AddControlData(HCINPUT_DEVICE_TYPE inputDeviceType, const HCCHARACTER_CONTROL_FUNC& data)
{
	m_controlFuncs[static_cast<size_t>(inputDeviceType)].push_back(data);
}

void HCCharacterController::SetCharacter(HC2DCharacter* character)
{
	m_currChar = character;
}
