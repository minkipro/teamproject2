#include "stdafx.h"
#include "DevScene.h"
#include "HCDevice.h"
#include "HCCharacter.h"
#include "HCTileMap.h"
#include "HCCharacterControllerByKeyboard.h"
DevScene::DevScene()
{

}
DevScene::~DevScene()
{
	size_t objectNum = m_sceneObjects.size();

	for (size_t i = 0; i < objectNum; i++)
	{
		if (m_sceneObjects[i])
		{
			delete m_sceneObjects[i];
			m_sceneObjects[i] = nullptr;
		}
	}
}
void DevScene::Init()
{
	auto graphic = HCDEVICE(HCGraphic);

	std::shared_ptr<IHCShader> vs;
	std::shared_ptr<IHCShader> gs;
	std::shared_ptr<IHCShader> ps;

	testPipeLine = std::make_shared<HCGraphicPipeLine>();
	graphic->CreateShader("testVS", HC::SHADER_TYPE::HCSHADER_VS, L"./../Common/Shader/PointToPlaneSahder.hlsl", "VS", vs);
	graphic->CreateShader("testGS", HC::SHADER_TYPE::HCSHADER_GS, L"./../Common/Shader/PointToPlaneSahder.hlsl", "GS", gs);
	graphic->CreateShader("testPS", HC::SHADER_TYPE::HCSHADER_PS, L"./../Common/Shader/PointToPlaneSahder.hlsl", "PS", ps);
	
	testPipeLine->m_primitive = HC::PRIMITIVE_TOPOLOGY::POINT;

	testPipeLine->SetShader(HC::SHADER_TYPE::HCSHADER_VS, vs);
	testPipeLine->SetShader(HC::SHADER_TYPE::HCSHADER_GS, gs);
	testPipeLine->SetShader(HC::SHADER_TYPE::HCSHADER_PS, ps);

	graphic->NumberingGraphicPipeLineSlot(0, testPipeLine);

	HC::Character* character = new HC::Character(L"Texture/PIPOYA FREE RPG Character Sprites NEKONIN/sp_3x4_pipo-nekonin001.png");

	std::vector<int> animationIndex[(int)HC::CharacterControllerByKeyboard::CharacterState::HCSHADER_COUNT];
	animationIndex[(int)HC::CharacterControllerByKeyboard::CharacterState::IDLE].push_back(1 + 0 * 3);
	animationIndex[(int)HC::CharacterControllerByKeyboard::CharacterState::UP].push_back(0 + 3 * 3);
	animationIndex[(int)HC::CharacterControllerByKeyboard::CharacterState::UP].push_back(1 + 3 * 3);
	animationIndex[(int)HC::CharacterControllerByKeyboard::CharacterState::UP].push_back(2 + 3 * 3);
	animationIndex[(int)HC::CharacterControllerByKeyboard::CharacterState::LEFT].push_back(0 + 1 * 3);
	animationIndex[(int)HC::CharacterControllerByKeyboard::CharacterState::LEFT].push_back(1 + 1 * 3);
	animationIndex[(int)HC::CharacterControllerByKeyboard::CharacterState::LEFT].push_back(2 + 1 * 3);
	animationIndex[(int)HC::CharacterControllerByKeyboard::CharacterState::DOWN].push_back(0 + 0 * 3);
	animationIndex[(int)HC::CharacterControllerByKeyboard::CharacterState::DOWN].push_back(1 + 0 * 3);
	animationIndex[(int)HC::CharacterControllerByKeyboard::CharacterState::DOWN].push_back(2 + 0 * 3);
	animationIndex[(int)HC::CharacterControllerByKeyboard::CharacterState::RIGHT].push_back(0 + 2 * 3);
	animationIndex[(int)HC::CharacterControllerByKeyboard::CharacterState::RIGHT].push_back(1 + 2 * 3);
	animationIndex[(int)HC::CharacterControllerByKeyboard::CharacterState::RIGHT].push_back(2 + 2 * 3);
	character->m_characterController = new HC::CharacterControllerByKeyboard(&character->m_renderPoint.Position, &character->m_renderPoint.TextureIndex, character->m_spriteNum, animationIndex);
	
	m_sceneObjects.push_back(new HC::TileMap(128.0f, 128.0f, 100, 100));
	m_sceneObjects.push_back(character);
}

void DevScene::Update()
{
	size_t objectNum = m_sceneObjects.size();
	for (size_t i = 0; i < objectNum; i++)
	{
		if (m_sceneObjects[i])
		{
			m_sceneObjects[i]->Update();
		}
	}
}

void DevScene::Render()
{
	size_t objectNum = m_sceneObjects.size();
	for (size_t i = 0; i < objectNum; i++)
	{
		if (m_sceneObjects[i])
		{
			m_sceneObjects[i]->Render(testPipeLine.get());
		}
	}
}
