#pragma once
#include <Keyboard.h>
#include "HCDevice.h"
#include <queue>

enum class HCKEYBOARD_KEY_STATE
{
	UP,      
	HELD,    
	RELEASED,
	PRESSED, 
};

typedef DirectX::Keyboard::Keys HCKEYBOARD_KEY_TYPE;

class HCKeyboard : public IHCDevice, public DirectX::Keyboard::KeyboardStateTracker
{
public:
	HCKeyboard() = default;
	virtual ~HCKeyboard() = default;

	virtual void		Init();
	virtual void		Update();
	virtual std::string GetDeviceName() const { return typeid(HCKeyboard).name(); }
	virtual LRESULT		WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	bool				IsKeyCurrState(HCKEYBOARD_KEY_TYPE key, HCKEYBOARD_KEY_STATE state);
private:
};

