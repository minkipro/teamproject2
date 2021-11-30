#include "stdafx.h"
#include "DevScene.h"
#include "HCDevice.h"
#include "Graphics\HCCameraManager.h"
#include "GlobalOption.h"
#include "HCTileMap.h"
#include "HCCharacterControlTest.h"

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

	HCTileMap* ptrTilemap = new HCTileMap(100.0f, 100.0f, 10, 1);
	m_sceneObjects.push_back(ptrTilemap);
	m_sceneObjects.back()->Init();

	auto charControlTestOb = new HCCharacterControlTest;
	charControlTestOb->Init();
	m_sceneObjects.push_back(charControlTestOb);
	
}

void DevScene::Update()
{
	float currDeltaTime = HCDEVICE(HCTimer)->GetDeltatime();

	HC::CameraManager* cameraManager = HC::CameraManager::Get();
	cameraManager->Update();

	DirectX::XMMATRIX orthoP = DirectX::XMMatrixOrthographicOffCenterLH(
		-static_cast<float>(HC::GO.WIN.WindowsizeX) * 0.5f, static_cast<float>(HC::GO.WIN.WindowsizeX) * 0.5f,
		static_cast<float>(HC::GO.WIN.WindowsizeY) * 0.5f, -static_cast<float>(HC::GO.WIN.WindowsizeY) * 0.5f,
		0, 1.0f);

	DirectX::XMStoreFloat4x4(&m_mainPass.ViewMatrix, cameraManager->GetMatrix());
	DirectX::XMStoreFloat4x4(&m_mainPass.OrthoMatrix, orthoP);

	m_mainPassCB->Map();
	m_mainPassCB->CopyCpuDataToGpu(&m_mainPass);
	m_mainPassCB->UnMap();

	size_t objectNum = m_sceneObjects.size();
	for (size_t i = 0; i < objectNum; i++)
	{
		if (m_sceneObjects[i])
		{
			m_sceneObjects[i]->Update(currDeltaTime);
		}
	}
}

void DevScene::Render()
{
	auto graphic = HCDEVICE(HCGraphic);

	static float pickingDefault[4] = { -1.0f,-1.0f,-1.0f,-1.0f };
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
