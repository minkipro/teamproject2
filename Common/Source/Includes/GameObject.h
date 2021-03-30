#pragma once
#include <vector>
#include "HCDevice.h"

class IHCDevice;

class GameObject
{
public:
	virtual void Init(std::vector<IHCDevice*> devices) = 0;
	virtual void Update() = 0;

protected:

};