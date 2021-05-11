#include "stdafx.h"
#include "HCCharacter.h"
#include "HCCharacterController.h"
using namespace HC;

Character::Character()
{
	auto graphic = HCDEVICE(HCGraphic);

	m_test.Size = { 100,100 };
	m_test.Position = { 100,100, 0.2f };
	m_test.TextureIndex = graphic->GetTextureIndex(L"Texture/PIPOYA FREE RPG Character Sprites NEKONIN/pipo-nekonin001.png");
	m_test.Uv = { 0.0f, 0.0f, 1.0f / 3.0f, 1.0f / 4.0f };

	HCGraphicPipeLine* pipeLine;
	graphic->GetGraphicPipeLine("testPipe2", &pipeLine);

	pipeLine->RenderReserveObject(&m_test);
	m_characterController = new HC::CharacterController(&m_test.Position);
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
