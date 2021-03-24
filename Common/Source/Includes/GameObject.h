#pragma once
#include <vector>

class HCDevice;

class GameObject
{
public:
	virtual void Init(std::vector<HCDevice*> devices) = 0;
	virtual void Update() = 0;

protected:

};

