#pragma once
#include "Graphics\HCGraphic.h"

class HCSceneObject
{
public:
	virtual ~HCSceneObject() = default;
	virtual void Init() = 0;
	virtual void Update(float deltaTime) = 0;
	virtual void Render() = 0;
};
