#include "stdafx.h"
#include "DevScene.h"
#include "HCDevice.h"
#include "HCCharacter.h"
#include "HCTileMap.h"
#include "Graphics\HCCameraManager.h"
#include "GlobalOption.h"
#include "HCCharacterControllerByKeyboard.h"

DevScene::DevScene()
{
	m_mainPass = {};
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

	HC::GRAPHIC_RESOURCE_DESC mainpassCBDesc;
	mainpassCBDesc.Type = HC::GRAPHIC_RESOURCE_TYPE::GRAPHIC_RESOURCE_CONSTANT_BUFFER;
	mainpassCBDesc.Stride = sizeof(m_mainPass);

	graphic->CreateResource(mainpassCBDesc, m_mainPassCB);

	m_testPipeLine = std::make_shared<HCGraphicPipeLine>();

	m_testPipeLine->SetVertexType(typeid(HCOnePointExtToRect).hash_code(), sizeof(HCOnePointExtToRect), &HCOnePointExtToRect::InputLayout);

	graphic->CreateShader(HC::SHADER_TYPE::HCSHADER_VS, L"./../Common/Shader/PointToPlaneSahder.hlsl", "VS", m_testPipeLine->m_shaders[HC::SHADER_TYPE::HCSHADER_VS]);
	graphic->CreateShader(HC::SHADER_TYPE::HCSHADER_GS, L"./../Common/Shader/PointToPlaneSahder.hlsl", "GS", m_testPipeLine->m_shaders[HC::SHADER_TYPE::HCSHADER_GS]);
	graphic->CreateShader(HC::SHADER_TYPE::HCSHADER_PS, L"./../Common/Shader/PointToPlaneSahder.hlsl", "PS", m_testPipeLine->m_shaders[HC::SHADER_TYPE::HCSHADER_PS]);

	m_testPipeLine->m_primitive = HC::PRIMITIVE_TOPOLOGY::POINT;

	/*HC::Character* character = new HC::Character(L"Texture/PIPOYA FREE RPG Character Sprites NEKONIN/sp_3x4_pipo-nekonin001.png");

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
	character->m_characterController = new HC::CharacterControllerByKeyboard(&character->m_renderInfos.Position, &character->m_renderInfos.TextureIndex, character->m_spriteNum, animationIndex);
	
	m_sceneObjects.push_back(character);*/

	m_sceneObjects.push_back(new HCTileMap(100.0f, 100.0f, 100, 100));
	m_sceneObjects.back()->Init();

	DirectX::XMMATRIX orthoP = DirectX::XMMatrixOrthographicOffCenterLH(
		-static_cast<float>(HC::GO.WIN.WindowsizeX) * 0.5f, static_cast<float>(HC::GO.WIN.WindowsizeX) * 0.5f,
		static_cast<float>(HC::GO.WIN.WindowsizeY) * 0.5f, -static_cast<float>(HC::GO.WIN.WindowsizeY) * 0.5f,
		0, 1.0f);

	DirectX::XMStoreFloat4x4(&m_mainPass.OrthoMatrix, orthoP);
	
	DirectX::XMStoreFloat4x4(&m_mainPass.ViewMatrix, DirectX::XMMatrixIdentity());
}

void DevScene::Update()
{
	HC::CameraManager* cameraManager = HC::CameraManager::Get();
	cameraManager->Update();
	DirectX::XMStoreFloat4x4(&m_mainPass.ViewMatrix, cameraManager->GetMatrix());

	m_mainPassCB->Map();
	m_mainPassCB->CpuDataCopyToGpu(&m_mainPass);
	m_mainPassCB->UnMap();

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
	auto graphic = HCDEVICE(HCGraphic);
	graphic->SetPipeLineObject(m_testPipeLine.get());
	graphic->SetConstantBuffer(m_mainPassCB, 0);

	size_t objectNum = m_sceneObjects.size();
	for (size_t i = 0; i < objectNum; i++)
	{
		if (m_sceneObjects[i])
		{
			m_sceneObjects[i]->Render();
		}
	}
}
