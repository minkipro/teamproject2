#pragma once
#include <Keyboard.h>
#include "HCDevice.h"

class HCKeyboard : public IHCDevice, DirectX::Keyboard::KeyboardStateTracker
{
public:
	HCKeyboard() = default;
	virtual ~HCKeyboard() = default;

	virtual void		Init();
	virtual void		Update();
	virtual std::string GetDeviceName() const { return typeid(HCKeyboard).name(); }

private:

};

