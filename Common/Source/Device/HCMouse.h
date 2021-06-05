#pragma once
#include <Mouse.h>
#include "HCDevice.h"

enum class HCMOUSE_BUTTON_TYPE
{
	LBUTTON,
	MBUTTON,
	RBUTTON,
	BUTTON1,
	BUTTON2,
	COUNT
};

typedef DirectX::Mouse::ButtonStateTracker::ButtonState HCMOUSE_BUTTON_STATE;

class HCMouse : public IHCDevice, public DirectX::Mouse::ButtonStateTracker
{
public:
	HCMouse() = default;
	virtual ~HCMouse() = default;

	virtual void			Init();
	virtual void			Update();
	virtual std::string		GetDeviceName() const { return typeid(HCMouse).name(); }

	HCMOUSE_BUTTON_STATE	GetButtonState(HCMOUSE_BUTTON_TYPE button);

private:
};