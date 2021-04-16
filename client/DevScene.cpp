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
	IHCShader* vs = nullptr;
	IHCShader* gs = nullptr;
	IHCShader* ps = nullptr;

	m_test.Size = { 100,100 };
	m_test.Position = { 100,100, 0.2f };
	m_test.TextureIndex = graphic->GetTextureIndex(L"knight.png");

	graphic->CreateGraphicPipeLine("testPipe", &testPipeLine);
	graphic->CreateShader("testVS", HC::SHADERTYPE::VS, L"./../Common/Shader/PointToPlaneSahder.hlsl", "VS", &vs);
	graphic->CreateShader("testGS", HC::SHADERTYPE::GS, L"./../Common/Shader/PointToPlaneSahder.hlsl", "GS", &gs);
	graphic->CreateShader("testPS", HC::SHADERTYPE::PS, L"./../Common/Shader/PointToPlaneSahder.hlsl", "PS", &ps);
	
	testPipeLine->m_Primitive = HC::PRIMITIVE_TOPOLOGY::POINT;
	testPipeLine->SelectInputSample<RenderPoint>();
	testPipeLine->SetShader(HC::SHADERTYPE::VS, vs);
	testPipeLine->SetShader(HC::SHADERTYPE::GS, gs);
	testPipeLine->SetShader(HC::SHADERTYPE::PS, ps);

	graphic->NumberingGraphicPipeLineSlot(0, testPipeLine);

	testPipeLine->RenderReserveObject(&m_test);
}

void DevScene::Update()
{
	auto timer = HCDEVICE(HCTimer);
	m_test.Position.x += 0.1f* timer->GetDeltatime();
}