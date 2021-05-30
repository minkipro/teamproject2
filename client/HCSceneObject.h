#pragma once
#include "Graphics\HCGraphic.h"

namespace HC
{
	class SceneObject
	{
	public:
		virtual ~SceneObject() = 0 {};
		virtual void Update() = 0;
		virtual void Render() = 0;
	};
}
