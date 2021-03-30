#pragma once
#include <Keyboard.h>
#include "HCDevice.h"
#include <queue>

class HCKeyboard : public IHCDevice, DirectX::Keyboard::KeyboardStateTracker
{
public:
	HCKeyboard() = default;
	virtual ~HCKeyboard() = default;

	virtual void		Init();
	virtual void		Update();
	virtual std::string GetDeviceName() const { return typeid(HCKeyboard).name(); }
	virtual LRESULT		WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
private:
	bool autoRepeat = false;
	bool keyStates[256];
	std::queue<KeyboardEvent> keyBuffer;
	std::queue<unsigned char> charBuffer;
};

