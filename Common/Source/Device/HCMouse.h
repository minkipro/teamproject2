#pragma once
#include <Mouse.h>
#include "HCDevice.h"

class HCMouse : public IHCDevice, DirectX::Mouse::ButtonStateTracker
{
public:
	HCMouse() = default;
	virtual ~HCMouse() = default;

	virtual void		Init();
	virtual void		Update();
	virtual std::string GetDeviceName() const { return typeid(HCMouse).name(); }
	virtual LRESULT		WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
private:
};