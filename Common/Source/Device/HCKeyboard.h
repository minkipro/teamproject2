#pragma once
#include <Keyboard.h>
#include "HCDevice.h"
#include <functional>

typedef DirectX::Keyboard::Keys HCKEYBOARD_KEY_TYPE;

enum class HCKEYBOARD_KEY_STATE
{
	PRESSED, 
	PRESS_HELD,    
	HELD,    
	RELEASED,
};

struct HCKEYBOARD_CONTROL_FUNC
{
	HCKEYBOARD_CONTROL_FUNC() = default;
	HCKEYBOARD_CONTROL_FUNC(HCKEYBOARD_KEY_TYPE key, HCKEYBOARD_KEY_STATE state, std::function<void(void*, float)> func)
	{
		KeyType = key;
		ControlFunc = func;
		KeyState = state;
	}

	std::function<void(void*, float)>	ControlFunc = nullptr;
	HCKEYBOARD_KEY_TYPE					KeyType = HCKEYBOARD_KEY_TYPE::None;
	HCKEYBOARD_KEY_STATE				KeyState = HCKEYBOARD_KEY_STATE::RELEASED;
};

class HCKeyboard : public IHCDevice, public DirectX::Keyboard::KeyboardStateTracker
{
public:
	HCKeyboard() = default;
	virtual ~HCKeyboard() = default;

	virtual void		Init();
	virtual void		Update();
	virtual std::string GetDeviceName() const { return typeid(HCKeyboard).name(); }

	void				SetBaseKeyFunc(HCKEYBOARD_KEY_TYPE key, std::function<void(float)> func);
	void				SetObject(void* object, const std::vector<HCKEYBOARD_CONTROL_FUNC>* funcs);

	bool __cdecl		IsKeyHeld(HCKEYBOARD_KEY_TYPE key) const noexcept { return held.IsKeyDown(key); }
	bool				IsKeyCurrState(HCKEYBOARD_KEY_TYPE key, HCKEYBOARD_KEY_STATE state);

private:
	DirectX::Keyboard::State				held;
	std::function<void(float)>				m_baseKeyFuncs[UCHAR_MAX] = {};
	bool									m_usedBaseKeyFuncStates[UCHAR_MAX] = {};
	std::vector<HCKEYBOARD_CONTROL_FUNC>	m_currObjectFuncs;
	void*									m_currObject = nullptr;
};

