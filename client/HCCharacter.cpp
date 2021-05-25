#include "stdafx.h"
#include "HCCharacter.h"
#include "HCCharacterController.h"
using namespace HC;

Character::Character(const wchar_t* textureName)
{
	auto graphic = HCDEVICE(HCGraphic);

	m_renderPoint.Size = { 128,128 };
	m_renderPoint.Position = { 100,100, 0.2f };
	TextureData textureData = graphic->GetTextureIndex(textureName);
	m_renderPoint.TextureIndex = textureData.textureIndex;
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
	auto graphic = HCDEVICE(HCGraphic);

	m_characterController->Update();

	HCGraphicPipeLine* pipeLine;
	graphic->GetGraphicPipeLine("testPipe", &pipeLine);

	pipeLine->RenderReserveObject(&m_renderPoint, m_renderPoint.TextureIndex);
}
