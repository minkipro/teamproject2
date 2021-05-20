#include "stdafx.h"
#include "DevScene.h"
#include "HCDevice.h"
#include "HCCharacter.h"

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

	IHCResource* texture = nullptr;
	IHCResource* texture2 = nullptr;

	HCGraphicPipeLine* testPipeLine = nullptr;
	HCGraphicPipeLine* testPipeLine2 = nullptr;
	IHCShader* vs = nullptr;
	IHCShader* gs = nullptr;
	IHCShader* ps = nullptr;
	IHCShader* vs2 = nullptr;
	IHCShader* gs2 = nullptr;


	graphic->CreateGraphicPipeLine("testPipe", &testPipeLine);
	graphic->CreateShader("testVS", HC::SHADER_TYPE::VS, L"./../Common/Shader/PointToPlaneSahder.hlsl", "VS", &vs);
	graphic->CreateShader("testGS", HC::SHADER_TYPE::GS, L"./../Common/Shader/PointToPlaneSahder.hlsl", "GS", &gs);
	graphic->CreateShader("testPS", HC::SHADER_TYPE::PS, L"./../Common/Shader/PointToPlaneSahder.hlsl", "PS", &ps);
	
	testPipeLine->m_primitive = HC::PRIMITIVE_TOPOLOGY::POINT;
	testPipeLine->SelectInputSample<RenderPoint>();

	testPipeLine->SetShader(HC::SHADER_TYPE::VS, vs);
	testPipeLine->SetShader(HC::SHADER_TYPE::GS, gs);
	testPipeLine->SetShader(HC::SHADER_TYPE::PS, ps);

	graphic->CreateGraphicPipeLine("testPipe2", &testPipeLine2);
	graphic->CreateShader("testVS2", HC::SHADER_TYPE::VS, L"./../Common/Shader/PointToUVPlane.hlsl", "VS", &vs2);
	graphic->CreateShader("testGS2", HC::SHADER_TYPE::GS, L"./../Common/Shader/PointToUVPlane.hlsl", "GS", &gs2);

	testPipeLine2->m_primitive = HC::PRIMITIVE_TOPOLOGY::POINT;
	testPipeLine2->SelectInputSample<RenderPointUV>();
	testPipeLine2->SetShader(HC::SHADER_TYPE::VS, vs2);
	testPipeLine2->SetShader(HC::SHADER_TYPE::GS, gs2);
	testPipeLine2->SetShader(HC::SHADER_TYPE::PS, ps);
	graphic->NumberingGraphicPipeLineSlot(0, testPipeLine);
	graphic->NumberingGraphicPipeLineSlot(1, testPipeLine2);
	m_sceneObjects.push_back(new HC::Character);
	//m_sceneObjects.push_back(new )
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