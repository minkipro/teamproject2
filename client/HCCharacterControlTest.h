#pragma once
#include "HCCharacterController.h"

class HCCharacterControlTest :public HCSceneObject
{
public:
	virtual void Init();
	virtual void Update(float deltaTime);
	virtual void Render();

private:
	HC2DCharacter				m_character;
	HCCharacterController		m_controler;
	std::vector<HC2DAnimation>	m_animations;
};