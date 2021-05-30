#include "stdafx.h"
#include "HCCharacter.h"
#include "HCCharacterController.h"
using namespace HC;

Character::Character(const wchar_t* textureName)
{
	auto graphic = HCDEVICE(HCGraphic);

	m_renderInfos.Size = { 128,128 };
	m_renderInfos.Position = { 100,100, 0.2f };
	HCTextureData textureData = graphic->GetTextureIndex(textureName);
	m_renderInfos.TextureIndex = textureData.textureIndex;
	m_spriteNum = textureData.spriteNum;

	m_characterController = nullptr;
}

Character::~Character()
{
	if (m_characterController)
	{
		delete m_characterController;
		m_characterController = nullptr;
	}
}

void Character::Update()
{
	m_characterController->Update();
}

void HC::Character::Render()
{
}
