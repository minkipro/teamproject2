#pragma once
#include "HCDevice.h"
#include <Mouse.h>

class HCMouse : public HCDevice, DirectX::Mouse::ButtonStateTracker
{
public:
	HCMouse() = default;
	virtual ~HCMouse() = default;

	virtual void		Init();
	virtual void		Update();
	virtual std::string GetDeviceName() const { return typeid(HCMouse).name(); }

private:
};