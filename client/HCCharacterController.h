#pragma once
#include <functional>
#include "Device\HCKeyboard.h"
#include "Device\HCMouse.h"
#include "HCCharacter.h"

enum class HCINPUT_DEVICE_TYPE
{
	KEYBOARD,
	MOUSE,
	COUNT
};

struct HCCHARACTER_CONTROL_FUNC
{
	HCCHARACTER_CONTROL_FUNC() = default;
	HCCHARACTER_CONTROL_FUNC(HCKEYBOARD_KEY_TYPE key, HCKEYBOARD_KEY_STATE state, std::function<void(HC2DCharacter*, float)> func)
	{
		ControlFunc = func;
		Keyboard.Key = key;
		Keyboard.KeyState = state;
	}
	HCCHARACTER_CONTROL_FUNC(HCMOUSE_BUTTON_TYPE button, HCMOUSE_BUTTON_STATE state, std::function<void(HC2DCharacter*, float)> func)
	{
		ControlFunc = func;
		Mouse.Button = button;
		Mouse.ButtonState = state;
	}

	std::function<void(HC2DCharacter*, float)> ControlFunc;

	union
	{
		struct
		{
			HCKEYBOARD_KEY_TYPE	 Key;
			HCKEYBOARD_KEY_STATE KeyState;
		} Keyboard;				 
								 
		struct					 
		{						 
			HCMOUSE_BUTTON_TYPE	 Button;
			HCMOUSE_BUTTON_STATE ButtonState;
		} Mouse;
	};
};

class HCCharacterController
{
public:
	HCCharacterController() = default;
	virtual ~HCCharacterController() = default;

	virtual void	Update();

	void			AddControlData(HCINPUT_DEVICE_TYPE inputDeviceType, const HCCHARACTER_CONTROL_FUNC& data);
	void			SetCharacter(HC2DCharacter* character);

protected:
	std::vector<HCCHARACTER_CONTROL_FUNC>	m_controlFuncs[static_cast<size_t>(HCINPUT_DEVICE_TYPE::COUNT)];
	HC2DCharacter*							m_currChar = nullptr;
};
