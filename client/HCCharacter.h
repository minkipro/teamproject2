#pragma once
#include "HCSceneObject.h"
#include "Graphics/HCInputDataSamples.h"

namespace HC
{
	class Character : public HC::SceneObject
	{
	public:
		Character();
		virtual ~Character() override {};
		virtual void Update() override;

	private:
		RenderPointUV m_test;
	};
}
