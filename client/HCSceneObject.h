#pragma once
#include "Graphics\HCGraphic.h"

class HCSceneObject
{
public:
	virtual ~HCSceneObject() = 0 {};
	virtual void Init() = 0;
	virtual void Update() = 0;
	virtual void Render() = 0;
};
