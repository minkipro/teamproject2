#include "stdafx.h"
#include "DevScene.h"
#include "HCDevice.h"

DevScene::DevScene()
{

}

void DevScene::Init()
{
	auto graphic = HCDEVICE(HCGraphic);


	m_model.Initialize()
	IHCTexture* texture = nullptr;
	IHCTexture* texture2 = nullptr;
	HCTextureBuffer* texBuffer = nullptr;

	HCGraphicPipeLine* testPipeLine = nullptr;
	IHCShader* vs = nullptr;
	IHCShader* gs = nullptr;
	IHCShader* ps = nullptr;

	m_test.
	//m_test.Size = { 100,100 };
	//m_test.Position = { 100,100, 0.2f };
	//m_test.TextureIndex = 1;

	/*graphic->CreateTexture(L"character.png", &texture);
	graphic->CreateTexture(L"knight.png", &texture2);
	graphic->CreateTextureBuffer("testTexBuffer", &texBuffer);*/

	texBuffer->SetTexture(0, texture);
	texBuffer->SetTexture(1, texture2);

	graphic->CreateGraphicPipeLine("testPipe", &testPipeLine);
	graphic->CreateShader("VS", HC::SHADERTYPE::VS, L"./../Common/Shader/SkeletonShader.hlsl", "VS", &vs);
	graphic->CreateShader("PS", HC::SHADERTYPE::PS, L"./../Common/Shader/SkeletonShader.hlsl", "PS", &ps);

	testPipeLine->m_primitive = HC::PRIMITIVE_TOPOLOGY::TRIANGLELIST;
	testPipeLine->
	testPipeLine->SelectInputSample<RenderPoint>();
	testPipeLine->SetShader(HC::SHADERTYPE::VS, vs);
	testPipeLine->SetShader(HC::SHADERTYPE::PS, ps);

	graphic->NumberingGraphicPipeLineSlot(0, testPipeLine);

	testPipeLine->RenderReserveObject("testTexBuffer", &m_test);
}

void DevScene::Update()
{
	auto timer = HCDEVICE(HCTimer);
	m_test.Position.x += 0.1f * timer->GetDeltatime();
}