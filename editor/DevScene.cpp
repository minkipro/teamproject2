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
	HCTextureBuffer* texBuffer = nullptr;

	HCGraphicPipeLine* testPipeLine = nullptr;
	IHCShader* vs = nullptr;
	IHCShader* gs = nullptr;
	IHCShader* ps = nullptr;

	//m_test.Size = { 100,100 };
	//m_test.Position = { 100,100, 0.2f };
	//m_test.TextureIndex = 1;

	graphic->CreateTexture(L"character.png", &texture);
	graphic->CreateTexture(L"knight.png", &texture2);
	graphic->CreateTextureBuffer("testTexBuffer", &texBuffer);

	texBuffer->SetTexture(0, texture);
	texBuffer->SetTexture(1, texture2);

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

	testPipeLine->RenderReserveObject("testTexBuffer", &m_test);
}

void DevScene::Update()
{
	auto timer = HCDEVICE(HCTimer);
	m_test.Position.x += 0.1f * timer->GetDeltatime();
}