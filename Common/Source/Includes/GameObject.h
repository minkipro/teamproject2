#pragma once
#include <vector>
#include "HCDevice.h"

class IHCDevice;

class GameObject
{
public:
	virtual void Init() = 0;
	virtual void Update() = 0;

protected:

};