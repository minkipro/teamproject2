#include "stdafx.h"
#include "HCCharacter.h"
#include "HCCharacterController.h"
using namespace HC;

Character::Character()
{
	auto graphic = HCDEVICE(HCGraphic);

	m_renderPoint.Size = { 100,100 };
	m_renderPoint.Position = { 100,100, 0.2f };
	TextureData textureData = graphic->GetTextureIndex(L"Texture/PIPOYA FREE RPG Character Sprites NEKONIN/sp_3x4_pipo-nekonin001.png");
	m_renderPoint.TextureIndex = textureData.textureIndex;
	
	std::vector<int> animationIndex[(int)CharacterController::CharacterState::COUNT];
	animationIndex[(int)CharacterController::CharacterState::IDLE].push_back(1*3 +0);
	animationIndex[(int)CharacterController::CharacterState::UP].push_back(0*3+3 );
	animationIndex[(int)CharacterController::CharacterState::UP].push_back(1 * 3 + 3 );
	animationIndex[(int)CharacterController::CharacterState::UP].push_back(2 * 3 + 3 );
	animationIndex[(int)CharacterController::CharacterState::LEFT].push_back(0 * 3 + 1);
	animationIndex[(int)CharacterController::CharacterState::LEFT].push_back(1 * 3 + 1);
	animationIndex[(int)CharacterController::CharacterState::LEFT].push_back(2 * 3 + 1);
	animationIndex[(int)CharacterController::CharacterState::DOWN].push_back(0 * 3 + 0);
	animationIndex[(int)CharacterController::CharacterState::DOWN].push_back(1 * 3 + 0);
	animationIndex[(int)CharacterController::CharacterState::DOWN].push_back(2 * 3 + 0);
	animationIndex[(int)CharacterController::CharacterState::RIGHT].push_back(0 * 3 + 2);
	animationIndex[(int)CharacterController::CharacterState::RIGHT].push_back(1 * 3 + 2);
	animationIndex[(int)CharacterController::CharacterState::RIGHT].push_back(2 * 3 + 2);
	m_characterController = new HC::CharacterController(&m_renderPoint.Position, &m_renderPoint.TextureIndex, textureData.spriteNum, animationIndex);
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
