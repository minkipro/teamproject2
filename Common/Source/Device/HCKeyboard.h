#pragma once
#include "HCDevice.h"
#include <Keyboard.h>

class HCKeyboard : public HCDevice, DirectX::Keyboard::KeyboardStateTracker
{
public:
	HCKeyboard() = default;
	virtual ~HCKeyboard() = default;

	virtual void		Init();
	virtual void		Update();
	virtual std::string GetDeviceName() const { return typeid(HCKeyboard).name(); }

private:

};

