#include "stdafx.h"
#include "DevScene.h"
#include "HCCharacter.h"

DevScene::DevScene()
{

}

DevScene::~DevScene()
{
	size_t sceneObjectSize = m_sceneObjects.size();
	for (size_t i = 0; i < sceneObjectSize; i++)
	{
		if (m_sceneObjects[i])
		{
			delete m_sceneObjects[i];
			m_sceneObjects[i] = nullptr;
		}
	}
	m_sceneObjects.clear();
}

void DevScene::Init()
{
	auto graphic = HCDEVICE(HCGraphic);

	IHCTexture* texture = nullptr;
	IHCTexture* texture2 = nullptr;

	HCGraphicPipeLine* testPipeLine = nullptr;
	HCGraphicPipeLine* testPipeLine2 = nullptr;
	IHCShader* vs = nullptr;
	IHCShader* vs2 = nullptr;
	IHCShader* gs = nullptr;
	IHCShader* gs2 = nullptr;
	IHCShader* ps = nullptr;

	graphic->CreateGraphicPipeLine("testPipe", &testPipeLine);
	graphic->CreateShader("testVS", HC::SHADERTYPE::VS, L"./../Common/Shader/PointToPlaneSahder.hlsl", "VS", &vs);
	graphic->CreateShader("testGS", HC::SHADERTYPE::GS, L"./../Common/Shader/PointToPlaneSahder.hlsl", "GS", &gs);
	graphic->CreateShader("testPS", HC::SHADERTYPE::PS, L"./../Common/Shader/PointToPlaneSahder.hlsl", "PS", &ps);
	
	

	testPipeLine->m_primitive = HC::PRIMITIVE_TOPOLOGY::POINT;
	testPipeLine->SelectInputSample<RenderPoint>();
	testPipeLine->SetShader(HC::SHADERTYPE::VS, vs);
	testPipeLine->SetShader(HC::SHADERTYPE::GS, gs);
	testPipeLine->SetShader(HC::SHADERTYPE::PS, ps);

	graphic->CreateGraphicPipeLine("testPipe2", &testPipeLine2);
	graphic->CreateShader("testVS2", HC::SHADERTYPE::VS, L"./../Common/Shader/PointToUVPlane.hlsl", "VS", &vs2);
	graphic->CreateShader("testGS2", HC::SHADERTYPE::GS, L"./../Common/Shader/PointToUVPlane.hlsl", "GS", &gs2);

	testPipeLine2->m_primitive = HC::PRIMITIVE_TOPOLOGY::POINT;
	testPipeLine2->SelectInputSample<RenderPointUV>();
	testPipeLine2->SetShader(HC::SHADERTYPE::VS, vs2);
	testPipeLine2->SetShader(HC::SHADERTYPE::GS, gs2);
	testPipeLine2->SetShader(HC::SHADERTYPE::PS, ps);

	graphic->NumberingGraphicPipeLineSlot(0, testPipeLine);
	graphic->NumberingGraphicPipeLineSlot(1, testPipeLine2);

	m_sceneObjects.push_back(new HC::Character);
}

void DevScene::Update()
{
	auto timer = HCDEVICE(HCTimer);
	
	size_t sceneObjectSize = m_sceneObjects.size();
	for (size_t i = 0; i < sceneObjectSize; i++)
	{
		m_sceneObjects[i]->Update();
	}
}