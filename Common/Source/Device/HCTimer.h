#pragma once
#include "HCDevice.h"
class HCTimer : public IHCDevice
{
public:
	HCTimer();
	virtual void		Init() override;
	virtual void		Update() override;
	virtual std::string GetDeviceName() const { return typeid(HCTimer).name(); };

	float GetDeltatime();
private:
	static float m_deltaTime;
};