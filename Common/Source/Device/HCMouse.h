#pragma once
#include <Mouse.h>
#include "HCDevice.h"

class HCMouse : public IHCDevice, public DirectX::Mouse::ButtonStateTracker
{
public:
	enum MouseButton : unsigned char
	{
		LBUTTON = 0,
		MBUTTON,
		RBUTTON,
		BUTTON1,
		BUTTON2,
		HCSHADER_COUNT
	};
	HCMouse() = default;
	virtual ~HCMouse() = default;

	virtual void		Init();
	virtual void		Update();
	virtual std::string GetDeviceName() const { return typeid(HCMouse).name(); }

	DirectX::Mouse::ButtonStateTracker::ButtonState GetButtonState(MouseButton button);
	virtual LRESULT		WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
private:
};