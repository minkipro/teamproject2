#include "stdafx.h"
#include "DevScene.h"
#include "HCDevice.h"

DevScene::DevScene()
{

}

void DevScene::Init()
{
	auto graphic = HCDEVICE(HCGraphic);

	IHCResource* texture = nullptr;
	IHCResource* texture2 = nullptr;

	HCGraphicPipeLine* testPipeLine = nullptr;
	IHCShader* vs = nullptr;
	IHCShader* gs = nullptr;
	IHCShader* ps = nullptr;

	m_test.Size = { 100,100 };
	m_test.Position = { 100,100, 0.2f };
	m_test.TextureIndex = graphic->GetTextureIndex(L"knight.png");

	graphic->CreateGraphicPipeLine("testPipe", &testPipeLine);
	graphic->CreateShader("testVS", HC::SHADER_TYPE::VS, L"./../Common/Shader/PointToPlaneSahder.hlsl", "VS", &vs);
	graphic->CreateShader("testGS", HC::SHADER_TYPE::GS, L"./../Common/Shader/PointToPlaneSahder.hlsl", "GS", &gs);
	graphic->CreateShader("testPS", HC::SHADER_TYPE::PS, L"./../Common/Shader/PointToPlaneSahder.hlsl", "PS", &ps);
	
	testPipeLine->m_primitive = HC::PRIMITIVE_TOPOLOGY::POINT;
	testPipeLine->SelectInputSample<RenderPoint>();
	testPipeLine->SetShader(HC::SHADER_TYPE::VS, vs);
	testPipeLine->SetShader(HC::SHADER_TYPE::GS, gs);
	testPipeLine->SetShader(HC::SHADER_TYPE::PS, ps);

	graphic->NumberingGraphicPipeLineSlot(0, testPipeLine);

	testPipeLine->RenderReserveObject(&m_test);
}

void DevScene::Update()
{
	auto timer = HCDEVICE(HCTimer);
	m_test.Position.x += 0.1f* timer->GetDeltatime();
}