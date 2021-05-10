#include "stdafx.h"
#include "DevScene.h"
#include "HCDevice.h"

DevScene::DevScene()
{

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

	m_test.Size = { 100,100 };
	m_test.Position = { 100,100, 0.2f };
	m_test.TextureIndex = graphic->GetTextureIndex(L"Texture/PIPOYA FREE RPG Character Sprites NEKONIN/pipo-nekonin001.png");
	m_test.Uv = { 0.0f, 0.0f, 1.0f / 3.0f, 1.0f / 4.0f };
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

	testPipeLine2->RenderReserveObject(&m_test);
}

void DevScene::Update()
{
	auto timer = HCDEVICE(HCTimer);
	//m_test.Position.x += 0.1f* timer->GetDeltatime();
}