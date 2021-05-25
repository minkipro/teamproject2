#pragma once
#include "HCSceneObject.h"
#include "Graphics/HCInputDataSamples.h"

namespace HC
{
	class CharacterController;
	class Character : public HC::SceneObject
	{
	public:
		Character(const wchar_t* textureName);
		virtual ~Character() override;
		virtual void Update() override;

	public:
		RenderPoint m_renderPoint;
		int m_spriteNum;
		CharacterController* m_characterController;
		
	};
}
