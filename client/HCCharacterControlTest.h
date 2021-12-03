#pragma once
#include "HCCharacter.h"

class HCCharacterControlTest :public HCSceneObject
{
public:
	virtual void Init(const wchar_t* textureName);
	virtual void Update(float deltaTime);
	virtual void Render();

public:
	HC2DCharacter				m_character;
	std::vector<HC2DAnimation>	m_animations;
};